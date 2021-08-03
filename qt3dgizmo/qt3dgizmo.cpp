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
                                                       AxisConstraint axisConstraint) {
    QVector3D result = position;

    // Only need to apply constraints for single constraints since
    // we intersect with a plane but only want one direction
    if(axisConstraint == XTrans) {
        result.setX(intersectionPosition.x());
    }
    else if(axisConstraint == YTrans ) {
        result.setY(intersectionPosition.y());
    }
    else if(axisConstraint == ZTrans ) {
        result.setZ(intersectionPosition.z());
    } else {
        result = intersectionPosition;
    }

    return result;
}

Plane Qt3DGizmoPrivate::initializeTranslationPlane(const QVector3D &position,
                                                   AxisConstraint axisConstraint) {
    //QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_delegateTransform->matrix();

    m_translationPlane.position = position;

    AxisConstraint newTranslationConstraint = axisConstraint;
    AxisConstraint newPlaneTranslationConstraint = axisConstraint;

    newTranslationConstraint = axisConstraint;
    newPlaneTranslationConstraint = axisConstraint;

    setPlaneOrientation(newPlaneTranslationConstraint);
    m_axisConstraint = newTranslationConstraint;
}

// Called by initializeTranslationPlane
void Qt3DGizmoPrivate::setPlaneOrientation(AxisConstraint axisConstraint) {
    QVector3D normal = m_rayFromClickPosition.start - m_rayFromClickPosition.end;
    if (axisConstraint == XTrans) {
        m_translationPlane.normal = QVector3D(0, normal.y(), normal.z());
    } else if (axisConstraint == YTrans) {
        m_translationPlane.normal = QVector3D(normal.x(), 0, normal.z());
    } else if (axisConstraint == ZTrans) {
        m_translationPlane.normal = QVector3D(normal.x(), normal.y(), 0);
    } else if (axisConstraint == XZTrans ) {
        m_translationPlane.normal = QVector3D(0, normal.y(), 0);
    } else if (axisConstraint == YZTrans) {
        m_translationPlane.normal = QVector3D(normal.x(), 0, 0);
    } else {//if(TransformAxes == XYTRANS {
        m_translationPlane.normal = QVector3D(0, 0, normal.z());
    }
}

void Qt3DGizmoPrivate::initialize(int x, int y, const QVector3D &position,
                                             AxisConstraint axisConstraint) {
    // TODO: Differentiate between translation and rotation
    m_axisConstraint = axisConstraint;
    m_rayFromClickPosition = generate3DRayFromScreenToInfinity(x, y);
    m_lastTranslationPosition = position;
    m_translationDisplacement = QVector3D();
    m_translationPlane = initializeTranslationPlane(position, axisConstraint);
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

    m_currentTranslationPosition = applyTranslationConstraint(m_lastTranslationPosition,
                                                              intersection.second,
                                                              m_axisConstraint);
    m_translationDisplacement = m_currentTranslationPosition - m_lastTranslationPosition;
    m_delegateTransform->setTranslation(m_delegateTransform->translation() + 0.01 * m_translationDisplacement);
    m_lastTranslationPosition = m_currentTranslationPosition;
}

void Qt3DGizmoPrivate::onMouseRelease() {
    m_mouseDown = false;
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
        d->m_mouseDown = false;
    });
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::positionChanged,
            this, [d](Qt3DInput::QMouseEvent *e){
        if (d->m_mouseDown) {
            d->update(e->x(), e->y());
        }
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

    d->m_translationHandleX = new ArrowTranslationHandle(this, {0, 0, 0}, "x", Qt::blue);
    d->m_translationHandleX->transform()->setRotationZ(-90);
    connect(d->m_translationHandleX, &Handle::pressed,
            this, [d](Qt3DRender::QPickEvent *e){
        d->m_mouseDown = true;
        d->initialize(e->position().x(), e->position().y(), e->worldIntersection(), Qt3DGizmoPrivate::XTrans);
    });
    d->m_translationHandles.append(d->m_translationHandleX);

    d->m_translationHandleY = new ArrowTranslationHandle(this, {0, 0, 0}, "y", Qt::green);
    d->m_translationHandles.append(d->m_translationHandleY);
    connect(d->m_translationHandleY, &Handle::pressed,
            this, [d](Qt3DRender::QPickEvent *e){
        d->m_mouseDown = true;
        d->initialize(e->position().x(), e->position().y(), e->localIntersection(), Qt3DGizmoPrivate::YTrans);
    });

    d->m_translationHandleZ = new ArrowTranslationHandle(this, {0, 0, 0}, "z", Qt::red);
    d->m_translationHandleZ->transform()->setRotationX(90);
    connect(d->m_translationHandleZ, &Handle::pressed,
            this, [d](Qt3DRender::QPickEvent *e){
        d->m_mouseDown = true;
        d->initialize(e->position().x(), e->position().y(), e->localIntersection(), Qt3DGizmoPrivate::ZTrans);
    });

    d->m_translationHandleXY = new PlaneTranslationHandle(this, {0.3f, 0.3f, 0}, Qt::yellow);
    d->m_translationHandleXY->transform()->setRotationX(90);
    connect(d->m_translationHandleXY, &Handle::pressed,
            this, [d](Qt3DRender::QPickEvent *e){
        d->m_mouseDown = true;
        d->initialize(e->position().x(), e->position().y(), e->localIntersection(), Qt3DGizmoPrivate::XYTrans);
    });

    d->m_translationHandleYZ = new PlaneTranslationHandle(this, {0, 0.3f, 0.3f}, Qt::yellow);
    d->m_translationHandleYZ->transform()->setRotationZ(90);
    connect(d->m_translationHandleYZ, &Handle::pressed,
            this, [d](Qt3DRender::QPickEvent *e){
        d->m_mouseDown = true;
        d->initialize(e->position().x(), e->position().y(), e->localIntersection(), Qt3DGizmoPrivate::YZTrans);
    });

    d->m_translationHandleXZ = new PlaneTranslationHandle(this, {0.3f, 0, 0.3f}, Qt::yellow);
    connect(d->m_translationHandleXZ, &Handle::pressed,
            this, [d](Qt3DRender::QPickEvent *e){
        d->m_mouseDown = true;
        d->initialize(e->position().x(), e->position().y(), e->localIntersection(), Qt3DGizmoPrivate::XZTrans);
    });

    d->m_translationHandles.append({d->m_translationHandleX,
                                    d->m_translationHandleY,
                                    d->m_translationHandleZ,
                                    d->m_translationHandleXY,
                                    d->m_translationHandleYZ,
                                    d->m_translationHandleXZ});

    d->m_rotationHandleX = new RotationHandle(this, QVector3D(0, 0, 0), Qt::blue);
    d->m_rotationHandleX->transform()->setRotationY(90);
    d->m_rotationHandleY = new RotationHandle(this, QVector3D(0, 0, 0), Qt::green);
    d->m_rotationHandleY->transform()->setRotationX(90);
    d->m_rotationHandleZ = new RotationHandle(this, QVector3D(0, 0, 0), Qt::red);

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
