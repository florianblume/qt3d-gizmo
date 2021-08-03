#include "qt3dgizmo.h"
#include "qt3dgizmo_p.h"

#include <QDebug>

#include <QSurfaceFormat>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DInput/QMouseEvent>

Qt3DGizmoPrivate::Qt3DGizmoPrivate() {

}

Ray Qt3DGizmoPrivate::generate3DRayFromScreenToInfinity(int x, int y) {
    float winZ0 = 0.0f;
    float winZ1 = 1.0f;
    int yCorrected = m_windowSize.height() - y;

    QVector3D origin = QVector3D(x, yCorrected, winZ0);
    QVector3D destination = QVector3D(x, yCorrected, winZ1);

    // TODO is using window size correct here?
    QVector3D unprojectedOrigin = origin.unproject(m_camera->viewMatrix(),
                                                   m_camera->projectionMatrix(),
                                                   QRect(0, 0, m_windowSize.width(), m_windowSize.height()));
    QVector3D unprojectedDestination = destination.unproject(m_camera->viewMatrix(),
                                                             m_camera->projectionMatrix(),
                                                             QRect(0, 0, m_windowSize.width(), m_windowSize.height()));

    return Ray(unprojectedOrigin, unprojectedDestination);
}

QVector3D Qt3DGizmoPrivate::applyTranslationConstraint(const QVector3D &position, const QVector3D &intersectionPosition,
                                                       Handle::AxisConstraint axisConstraint) {
    QVector3D result = position;

    // Only need to apply constraints for single constraints since
    // we intersect with a plane but only want one direction
    if(axisConstraint == Handle::XTrans) {
        result.setX(intersectionPosition.x());
    }
    else if(axisConstraint == Handle::YTrans ) {
        result.setY(intersectionPosition.y());
    }
    else if(axisConstraint == Handle::ZTrans ) {
        result.setZ(intersectionPosition.z());
    } else {
        result = intersectionPosition;
    }

    return result;
}

Plane Qt3DGizmoPrivate::initializeTranslationPlane(const QVector3D &position,
                                                   Handle::AxisConstraint axisConstraint) {
    //QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_delegateTransform->matrix();

    Handle::AxisConstraint newTranslationConstraint = axisConstraint;
    Handle::AxisConstraint newPlaneTranslationConstraint = axisConstraint;

    newTranslationConstraint = axisConstraint;
    newPlaneTranslationConstraint = axisConstraint;

    m_axisConstraint = newTranslationConstraint;

    QVector3D planeNormal = computePlaneNormal(m_rayFromClickPosition, newPlaneTranslationConstraint);

    return Plane(position, planeNormal);
}

// Called by initializeTranslationPlane
QVector3D Qt3DGizmoPrivate::computePlaneNormal(const Ray &ray, Handle::AxisConstraint axisConstraint) {
    QVector3D normal = ray.start - ray.end;

    if (axisConstraint == Handle::XTrans) {
        normal = QVector3D(0, normal.y(), normal.z());
    } else if (axisConstraint == Handle::YTrans) {
        normal = QVector3D(normal.x(), 0, normal.z());
    } else if (axisConstraint == Handle::ZTrans) {
        normal = QVector3D(normal.x(), normal.y(), 0);
    } else if (axisConstraint == Handle::XZTrans ) {
        normal = QVector3D(0, normal.y(), 0);
    } else if (axisConstraint == Handle::YZTrans) {
        normal = QVector3D(normal.x(), 0, 0);
    } else {//if(TransformAxes == XYTRANS {
        normal = QVector3D(0, 0, normal.z());
    }

    return normal;
}

void Qt3DGizmoPrivate::initialize(Qt3DRender::QPickEvent *event,
                                  Handle::AxisConstraint axisConstraint) {
    // We set this here to true since this function is only called when
    // a handle was pressed and we cannot set it to true in the MouseHandler
    // since then the update function will translate whenever the mouse
    // is pressed anywhere
    m_mouseDownOnHandle = true;
    // TODO: Differentiate between translation and rotation
    m_axisConstraint = axisConstraint;
    m_rayFromClickPosition = generate3DRayFromScreenToInfinity(event->position().x(),
                                                               event->position().y());
    m_translationDisplacement = QVector3D();
    m_translationPlane = initializeTranslationPlane(event->worldIntersection(), axisConstraint);
}

void Qt3DGizmoPrivate::update(int x, int y) {
    m_rayFromClickPosition = generate3DRayFromScreenToInfinity(x, y);
    QPair<int, QVector3D> intersection = m_rayFromClickPosition.intersects(m_translationPlane);

    if (intersection.first == 0) {
        // No intersection
    } else if (intersection.first == 1) {
        // There is an intersection
    } else if (intersection.first == 2) {
        // Segment lies in plane
    }

    if (m_currentMode == Qt3DGizmo::Translation) {
        m_currentTranslationPosition = applyTranslationConstraint(m_translationPlane.position,
                                                                  intersection.second,
                                                                  m_axisConstraint);
        m_translationDisplacement = m_currentTranslationPosition - m_translationPlane.position;
        m_delegateTransform->setTranslation(m_delegateTransform->translation() + m_translationDisplacement);
        m_translationPlane.position = m_currentTranslationPosition;
    }
}

void Qt3DGizmoPrivate::removeHighlightsFromHanldes() {
    for (Handle *handle : m_translationHandles) {
        handle->setIsDragged(false);
    }
    for (Handle *handle : m_rotationHandles) {
        handle->setIsDragged(false);
    }
}

void Qt3DGizmoPrivate::onMouseRelease() {
    m_mouseDownOnHandle = false;
}

Qt3DGizmo::Qt3DGizmo(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , d_ptr(new Qt3DGizmoPrivate) {
    Q_D(Qt3DGizmo);

    d->m_mouseDevice = new Qt3DInput::QMouseDevice(this);
    d->m_mouseHandler = new Qt3DInput::QMouseHandler;
    d->m_mouseHandler->setSourceDevice(d->m_mouseDevice);
    addComponent(d->m_mouseHandler);
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::released,
            this, [d](){
        d->m_mouseDownOnHandle = false;
        d->removeHighlightsFromHanldes();
    });
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::positionChanged,
            this, [d](Qt3DInput::QMouseEvent *e){
        if (d->m_mouseDownOnHandle) {
            d->update(e->x(), e->y());
        }
    });
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::exited,
            this, [d](){
        d->removeHighlightsFromHanldes();
    });

    d->m_ownTransform = new Qt3DCore::QTransform;
    addComponent(d->m_ownTransform);

    d->m_sphereEntity = new Qt3DCore::QEntity(this);
    d->m_sphereMaterial = new Qt3DExtras::QPhongMaterial;
    d->m_sphereMaterial->setAmbient(QColor(50, 50, 50, 50));
    d->m_sphereMaterial->setShininess(0.0f);
    d->m_sphereMesh = new Qt3DExtras::QSphereMesh;
    d->m_sphereMesh->setRadius(0.05);
    d->m_sphereMesh->setRings(50);
    d->m_sphereMesh->setSlices(50);
    d->m_sphereObjectPicker = new Qt3DRender::QObjectPicker;
    d->m_sphereObjectPicker->setDragEnabled(true);
    d->m_sphereObjectPicker->setHoverEnabled(true);
    connect(d->m_sphereObjectPicker, &Qt3DRender::QObjectPicker::clicked,
            this, [this, d](){
        if (d->m_currentMode == Translation) {
            this->setMode(Rotation);
        } else {
            this->setMode(Translation);
        }
    });
    connect(d->m_sphereObjectPicker, &Qt3DRender::QObjectPicker::moved,
            this, [d](){
        d->m_sphereTransform->setScale(1.4f);
    });
    connect(d->m_sphereObjectPicker, &Qt3DRender::QObjectPicker::exited,
            this, [d](){
        d->m_sphereTransform->setScale(1.0f);
    });

    d->m_sphereTransform = new Qt3DCore::QTransform;
    d->m_sphereEntity->addComponent(d->m_sphereMaterial);
    d->m_sphereEntity->addComponent(d->m_sphereMesh);
    d->m_sphereEntity->addComponent(d->m_sphereObjectPicker);
    d->m_sphereEntity->addComponent(d->m_sphereTransform);

    d->m_translationHandleX = new ArrowTranslationHandle(this, Handle::XTrans, {0, 0, 0}, "x", Qt::blue);
    d->m_translationHandleX->transform()->setRotationZ(-90);
    connect(d->m_translationHandleX, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleY = new ArrowTranslationHandle(this, Handle::YTrans, {0, 0, 0}, "y", Qt::green);
    d->m_translationHandles.append(d->m_translationHandleY);
    connect(d->m_translationHandleY, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleZ = new ArrowTranslationHandle(this, Handle::ZTrans, {0, 0, 0}, "z", Qt::red);
    d->m_translationHandleZ->transform()->setRotationX(90);
    connect(d->m_translationHandleZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleXY = new PlaneTranslationHandle(this, Handle::XYTrans, {0.3f, 0.3f, 0}, Qt::yellow);
    d->m_translationHandleXY->transform()->setRotationX(90);
    connect(d->m_translationHandleXY, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleYZ = new PlaneTranslationHandle(this, Handle::YZTrans, {0, 0.3f, 0.3f}, Qt::yellow);
    d->m_translationHandleYZ->transform()->setRotationZ(90);
    connect(d->m_translationHandleYZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleXZ = new PlaneTranslationHandle(this, Handle::XZTrans, {0.3f, 0, 0.3f}, Qt::yellow);
    connect(d->m_translationHandleXZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandles.append({d->m_translationHandleX,
                                    d->m_translationHandleY,
                                    d->m_translationHandleZ,
                                    d->m_translationHandleXY,
                                    d->m_translationHandleYZ,
                                    d->m_translationHandleXZ});

    d->m_rotationHandleX = new RotationHandle(this, Handle::XTrans, QVector3D(0, 0, 0), Qt::blue);
    d->m_rotationHandleX->transform()->setRotationY(90);
    d->m_rotationHandleY = new RotationHandle(this, Handle::YTrans, QVector3D(0, 0, 0), Qt::green);
    d->m_rotationHandleY->transform()->setRotationX(90);
    d->m_rotationHandleZ = new RotationHandle(this, Handle::ZTrans, QVector3D(0, 0, 0), Qt::red);

    d->m_rotationHandles.append({d->m_rotationHandleX,
                                 d->m_rotationHandleY,
                                 d->m_rotationHandleZ});
    for (Handle* handle : d->m_rotationHandles) {
        handle->setEnabled(false);
    }
}

Qt3DGizmo::Mode Qt3DGizmo::mode() const {
    Q_D(const Qt3DGizmo);
    return d->m_currentMode;
}

QSize Qt3DGizmo::windowSize() const {
    Q_D(const Qt3DGizmo);
    return d->m_windowSize;
}

Qt3DCore::QTransform *Qt3DGizmo::delegateTransform() const {
    Q_D(const Qt3DGizmo);
    return d->m_delegateTransform;
}

Qt3DRender::QCamera *Qt3DGizmo::camera() const {
    Q_D(const Qt3DGizmo);
    return d->m_camera;
}

void Qt3DGizmo::setMode(Mode mode) {
    // Gets called externally or by clicking the sphere
    Q_D(Qt3DGizmo);
    d->m_currentMode = mode;
    for (Handle *translationHandle : d->m_translationHandles) {
        translationHandle->setEnabled(mode == Translation);
    }
    for (Handle *rotationHandle : d->m_rotationHandles) {
        rotationHandle->setEnabled(mode == Rotation);
    }
}

void Qt3DGizmo::setWindowSize(const QSize &size) {
    Q_D(Qt3DGizmo);
    d->m_windowSize = size;
    Q_EMIT windowSizeChanged(d->m_windowSize);
}

void Qt3DGizmo::setWindowWidth(int width) {
    Q_D(Qt3DGizmo);
    d->m_windowSize = QSize(width, d->m_windowSize.height());
    Q_EMIT windowSizeChanged(d->m_windowSize);
}

void Qt3DGizmo::setWindowHeight(int height) {
    Q_D(Qt3DGizmo);
    d->m_windowSize = QSize(d->m_windowSize.width(), height);
    Q_EMIT windowSizeChanged(d->m_windowSize);
}

void Qt3DGizmo::setDelegateTransform(Qt3DCore::QTransform *transform) {
    Q_D(Qt3DGizmo);
    d->m_delegateTransform = transform;
    disconnect(d->m_delegateTransformConnection);
    d->m_delegateTransformConnection = connect(d->m_delegateTransform, &Qt3DCore::QTransform::translationChanged,
                                               d->m_ownTransform, &Qt3DCore::QTransform::setTranslation);
    Q_EMIT delegateTransformChanged(transform);
}

void Qt3DGizmo::setCamera(Qt3DRender::QCamera *camera) {
    Q_D(Qt3DGizmo);
    d->m_camera = camera;
    d->m_translationHandleX->setCamera(camera);
    d->m_translationHandleY->setCamera(camera);
    d->m_translationHandleZ->setCamera(camera);
    Q_EMIT cameraChanged(camera);
}
