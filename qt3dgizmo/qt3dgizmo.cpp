#include "qt3dgizmo.h"
#include "qt3dgizmo_p.h"

#include <QDebug>

#include <QtMath>
#include <QSurfaceFormat>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QCursor>
#include <QApplication>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DInput/QMouseEvent>

Qt3DGizmoPrivate::Qt3DGizmoPrivate() {

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

Plane Qt3DGizmoPrivate::initializeTranslationPlane(const Ray &clickRay,
                                                   const QVector3D &position,
                                                   Handle::AxisConstraint axisConstraint) {
    // TODO handle special cases

    //QMatrix4x4 modelViewMatrix = m_camera->viewMatrix() * m_delegateTransform->matrix();

    Handle::AxisConstraint newPlaneTranslationConstraint = axisConstraint;

    QVector3D planeNormal = computePlaneNormal(clickRay, newPlaneTranslationConstraint);

    return Plane(position, planeNormal);
}

Plane Qt3DGizmoPrivate::initializeRotationPlane(const QVector3D &position,
                                                Handle::AxisConstraint translationConstraint) {
    QVector3D normal;
    // Rotation can only occur in these axes
    switch (translationConstraint) {
    case Handle::XTrans:
        normal = QVector3D(1, 0, 0);
        break;
    case Handle::YTrans:
        normal = QVector3D(0, 1, 0);
        break;
    case Handle::ZTrans:
        normal = QVector3D(0, 0, 1);
        break;
    default:
        // Can never occur
        break;
    }

    return Plane(position, normal);
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
    // TODO: This functionw ill still be called by the handles' pressed signal

    // We set this here to true since this function is only called when
    // a handle was pressed and we cannot set it to true in the MouseHandler
    // since then the update function will translate whenever the mouse
    // is pressed anywhere
    m_mouseDownOnHandle = true;
    m_axisConstraint = axisConstraint;

    if (m_currentMode == Qt3DGizmo::Translation) {
        m_translationDisplacement = QVector3D();
        m_plane = initializeTranslationPlane(m_rayFromClickPosition,
                                             event->worldIntersection(),
                                             axisConstraint);
    } else {
        m_plane = initializeRotationPlane(m_delegateTransform->translation(),
                                          axisConstraint);
        QPair<int, QVector3D> intersection = m_rayFromClickPosition.intersectsPlane(m_plane);
        // We cannot store the intersection on the handle directly since it does not necessarily
        // lie directly on the rotation plane (the handles are 3D circles), i.e. we need to
        // project the ray onto the plane and store the intersection minus the plane's position
        // to obtain a vector from origin
        m_lastPositionOnRotationHandle = (intersection.second - m_plane.position).normalized();
        m_initialOrientation = m_delegateTransform->rotation();
    }
}

void Qt3DGizmoPrivate::updateMouseCoordinates(int x, int y) {
}

void Qt3DGizmoPrivate::updateTransform(int x, int y) {
    // Update will be called after receiving the ray from the compute shader and
    // m_mouseDownOnHanlde is true

    QPair<int, QVector3D> intersection = m_rayFromClickPosition.intersectsPlane(m_plane);

    if (intersection.first == 0) {
        // No intersection
    } else if (intersection.first == 1) {
        // There is an intersection
    } else if (intersection.first == 2) {
        // Segment lies in plane
        // TODO we need to handle this special case and restrict plane translation
        // to only one axis
    }

    if (m_currentMode == Qt3DGizmo::Translation) {
        m_currentTranslationPosition = applyTranslationConstraint(m_plane.position,
                                                                  intersection.second,
                                                                  m_axisConstraint);
        m_translationDisplacement = m_currentTranslationPosition - m_plane.position;
        m_delegateTransform->setTranslation(m_delegateTransform->translation() + m_translationDisplacement);
        m_plane.position = m_currentTranslationPosition;
    } else {
        QVector3D point = (intersection.second - m_plane.position).normalized();
        QVector3D pointOnRotationHandle;
        // If we do not perform this check the rotation flickers when entering and
        // exiting the rotation handle with the mouse, this checks whether the mouse
        // is within the rotation handle or outside
        if (intersection.second.length() >= point.length() ||
                qFuzzyCompare(intersection.second.length(), point.length())) {
            pointOnRotationHandle = (intersection.second - point).normalized();
        } else {
            pointOnRotationHandle = (point - intersection.second).normalized();
        }
        QQuaternion rotation = QQuaternion::rotationTo(m_lastPositionOnRotationHandle,
                                                       point);
        m_delegateTransform->setRotation(rotation * m_initialOrientation);
    }
}

void Qt3DGizmoPrivate::removeHighlightsFromHanldes() {
    for (Handle *handle : m_handles) {
        handle->setHighlighted(false);
    }
}

void Qt3DGizmoPrivate::adjustScaleToCameraDistance() {
    if (m_scaleToCameraDistance && m_camera) {
        // TODO Not completely working yet
        // TODO move to shader
        float reciprScaleOnscreen = 0.05;
        float w = ((m_camera->projectionMatrix() * m_camera->viewMatrix() * m_ownTransform->matrix()) * QVector4D(0, 0, 0, 1)).w();
        w *= reciprScaleOnscreen;
        //m_ownTransform->setScale(m_scale + w);
    }
}

void Qt3DGizmoPrivate::onMouseRelease() {
    m_mouseDownOnHandle = false;
}

Qt3DGizmo::Qt3DGizmo(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , d_ptr(new Qt3DGizmoPrivate) {
    Q_D(Qt3DGizmo);

    //d->m_computeMaterial = new RayComputeMaterial;
    //addComponent(d->m_computeMaterial);
    /*
    connect(d->m_computeMaterial, &RayComputeMaterial::rayComputed,
            this, [d](const Ray &ray){
        // Tell the rest of the program that we are still computing a ray,
        // this happens very rarely
        //qDebug() << d->m_timer.elapsed();
        d->m_rayFromClickPosition = ray;
    });
    */

    d->m_computeCommand = new Qt3DRender::QComputeCommand();
    d->m_computeCommand->setRunType(Qt3DRender::QComputeCommand::Continuous);
    addComponent(d->m_computeCommand);
    // Problem is 1D (compute the two rays), i.e. Y and Z must be 1
    d->m_computeCommand->setWorkGroupX(1);
    d->m_computeCommand->setWorkGroupY(1);
    d->m_computeCommand->setWorkGroupZ(1);

    d->m_transparentObjectPicker = new TransparentObjectPicker;
    addComponent(d->m_transparentObjectPicker);
    d->m_mouseDevice = new Qt3DInput::QMouseDevice(this);
    d->m_mouseHandler = new Qt3DInput::QMouseHandler;
    d->m_mouseHandler->setSourceDevice(d->m_mouseDevice);
    addComponent(d->m_mouseHandler);
    // TODO Add pressed handler for m_mouseHandler to call initialize method
    // which uses the compute shader to compute the current ray
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::pressed,
            this, [d](){
        // Pressed happens after we upload the new mouse coordinates
        // and receive the resulting ray from the compute shader
        // During mouse movement we look for intersections and store
        // any. Then in this function we simply set m_mouseDownOnHandle to true
    });
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::positionChanged,
            this, [d](Qt3DInput::QMouseEvent *e){
        d->m_timer.start();
        d->m_transparentObjectPicker->setMousePosition(e->x(), e->y());
        if (!d->m_mouseDownOnHandle) {
            // Iterate over handles and check for intersections and store any
            // We might not have the ray ready here yet but during the next
            // mouse movement it will have been computed
            for (int i = 0; i < d->m_handles.size(); i++) {
                QPair<bool, QVector3D> result = d->m_handles[i]->intersectionWithRay(d->m_rayFromClickPosition);
                d->m_handles[i]->setHighlighted(result.first);
                if (result.first) {
                }
            }
        }

        // The mouse down flag is set in the initialize function of the private
        // Qt3DGizmo class through the pressed events fired by the handles
        if (d->m_mouseDownOnHandle) {
            if (d->m_hideMouseWhileTransforming && !d->m_currentlyHidingMouse) {
                QCursor cursor(Qt::BlankCursor);
                QApplication::setOverrideCursor(cursor);
                d->m_currentlyHidingMouse = true;
            }
            // This does the acual transforming
            d->updateMouseCoordinates(e->x(), e->y());
        }
    });
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::released,
            this, [d](){
        d->m_mouseDownOnHandle = false;
        d->removeHighlightsFromHanldes();
        QApplication::restoreOverrideCursor();
        d->m_currentlyHidingMouse = false;
    });
    connect(d->m_mouseHandler, &Qt3DInput::QMouseHandler::exited,
            this, [d](){
        d->removeHighlightsFromHanldes();
        QApplication::restoreOverrideCursor();
        d->m_currentlyHidingMouse = false;
    });

    d->m_ownTransform = new Qt3DCore::QTransform;
    addComponent(d->m_ownTransform);

    QColor red = QColor(220, 50, 100);
    QColor green = QColor(50, 220, 100);
    QColor blue = QColor(50, 100, 220);
    QColor yellow = QColor(220, 220, 80);
    QColor gray = QColor(80, 80, 80);

    d->m_modeSwitcherHandle = new ModeSwitcherHandle(this, {0, 0, 0}, gray);

    d->m_translationHandleX = new ArrowTranslationHandle(this, Handle::XTrans, {0, 0, 0}, blue);
    d->m_translationHandleX->transform()->setRotationZ(-90);
    connect(d->m_translationHandleX, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);
    d->m_transparentObjectPicker->setGeometry(d->m_translationHandleX->geometry());

    d->m_translationHandleY = new ArrowTranslationHandle(this, Handle::YTrans, {0, 0, 0}, green);
    d->m_translationHandles.append(d->m_translationHandleY);
    connect(d->m_translationHandleY, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleZ = new ArrowTranslationHandle(this, Handle::ZTrans, {0, 0, 0}, red);
    d->m_translationHandleZ->transform()->setRotationX(90);
    connect(d->m_translationHandleZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleXY = new PlaneTranslationHandle(this, Handle::XYTrans, {0.3f, 0.3f, 0}, yellow);
    d->m_translationHandleXY->transform()->setRotationX(90);
    connect(d->m_translationHandleXY, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleYZ = new PlaneTranslationHandle(this, Handle::YZTrans, {0, 0.3f, 0.3f}, yellow);
    d->m_translationHandleYZ->transform()->setRotationZ(90);
    connect(d->m_translationHandleYZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandleXZ = new PlaneTranslationHandle(this, Handle::XZTrans, {0.3f, 0, 0.3f}, yellow);
    connect(d->m_translationHandleXZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_translationHandles.append({d->m_translationHandleX,
                                    d->m_translationHandleY,
                                    d->m_translationHandleZ,
                                    d->m_translationHandleXY,
                                    d->m_translationHandleYZ,
                                    d->m_translationHandleXZ});

    d->m_rotationHandleX = new RotationHandle(this, Handle::XTrans, QVector3D(0, 0, 0), blue);
    d->m_rotationHandleX->transform()->setRotationY(90);
    connect(d->m_rotationHandleX, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);
    d->m_rotationHandleY = new RotationHandle(this, Handle::YTrans, QVector3D(0, 0, 0), green);
    d->m_rotationHandleY->transform()->setRotationX(90);
    connect(d->m_rotationHandleY, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);
    d->m_rotationHandleZ = new RotationHandle(this, Handle::ZTrans, QVector3D(0, 0, 0), red);
    connect(d->m_rotationHandleZ, &Handle::pressed,
            d, &Qt3DGizmoPrivate::initialize);

    d->m_rotationHandles.append({d->m_rotationHandleX,
                                 d->m_rotationHandleY,
                                 d->m_rotationHandleZ});

    d->m_handles.append(d->m_modeSwitcherHandle);
    d->m_handles.append(d->m_translationHandles);
    d->m_handles.append(d->m_rotationHandles);

    for (int i = 0; i < d->m_rotationHandles.size(); i++) {
        d->m_rotationHandles[i]->setEnabled(false);
    }

    setEnabled(false);
    setScale(d->m_scale);
}

Qt3DGizmo::TransformationMode Qt3DGizmo::transformationMode() const {
    Q_D(const Qt3DGizmo);
    return d->m_currentMode;
}

Qt3DCore::QTransform *Qt3DGizmo::delegateTransform() const {
    Q_D(const Qt3DGizmo);
    return d->m_delegateTransform;
}

void Qt3DGizmo::setEnabled(bool enabled) {
    Q_D(const Qt3DGizmo);
    // Check which mode we were in and enable the respective handles
    if (d->m_currentMode == Translation) {
        for (int i = 0; i < d->m_translationHandles.size(); i++) {
            d->m_translationHandles[i]->setEnabled(enabled);
        }
    } else {
        for (int i = 0; i < d->m_rotationHandles.size(); i++) {
            d->m_rotationHandles[i]->setEnabled(enabled);
        }
    }
    d->m_modeSwitcherHandle->setEnabled(enabled);
}

float Qt3DGizmo::scale() const {
    Q_D(const Qt3DGizmo);
    return d->m_scale;
}

bool Qt3DGizmo::scaleToCameraDistance() const {
    Q_D(const Qt3DGizmo);
    return d->m_scaleToCameraDistance;
}

bool Qt3DGizmo::hideMouseWhileTransforming() const {
    Q_D(const Qt3DGizmo);
    return d->m_hideMouseWhileTransforming;
}

bool Qt3DGizmo::flatAppearance() const {
    Q_D(const Qt3DGizmo);
    return d->m_flatAppearance;
}

void Qt3DGizmo::setDetectFramegraphAutomatically(bool detectFramegraphAutomatically) {
    Q_D(Qt3DGizmo);
    if (detectFramegraphAutomatically != d->m_detectFramegraphAutomatically &&
            detectFramegraphAutomatically) {
        // Need to re-traverse the framegraph
    }
    d->m_detectFramegraphAutomatically = detectFramegraphAutomatically;
}

void Qt3DGizmo::setCameraViewportSurface(Qt3DRender::QCamera *camera,
                                         Qt3DRender::QViewport *viewport,
                                         QSurface *surface) {
    Q_D(Qt3DGizmo);
    // TODO mutex --> Proably don't need since we only have one and not multiple anymore
    d->m_cameraViewportSurfaceTriplet = {camera, viewport, surface};
}

void Qt3DGizmo::setTransformationMode(TransformationMode mode) {
    // Gets called externally or by clicking the sphere
    Q_D(Qt3DGizmo);
    d->m_currentMode = mode;
    for (int i = 0; i < d->m_translationHandles.size(); i++) {
        d->m_translationHandles[i]->setEnabled(mode == Translation);
    }
    for (int i = 0; i < d->m_rotationHandles.size(); i++) {
        d->m_rotationHandles[i]->setEnabled(mode == Rotation);
    }
}

void Qt3DGizmo::setDelegateTransform(Qt3DCore::QTransform *transform) {
    Q_D(Qt3DGizmo);
    setEnabled(true);
    d->m_delegateTransform = transform;
    d->m_ownTransform->setTranslation(transform->translation());
    disconnect(d->m_delegateTransformTranslationChangedConnection);
    disconnect(d->m_delegateTransformAdjustScaleConnection);
    d->m_delegateTransformTranslationChangedConnection = connect(
                d->m_delegateTransform, &Qt3DCore::QTransform::translationChanged,
                d->m_ownTransform, &Qt3DCore::QTransform::setTranslation);
    d->m_delegateTransformAdjustScaleConnection = connect(
                d->m_delegateTransform, &Qt3DCore::QTransform::translationChanged,
                d, &Qt3DGizmoPrivate::adjustScaleToCameraDistance);
    d->adjustScaleToCameraDistance();
    emit delegateTransformChanged(transform);
}

void Qt3DGizmo::setScale(float scale) {
    Q_D(Qt3DGizmo);
    d->m_scale = scale;
    d->adjustScaleToCameraDistance();
    emit scaleChanged(scale);
}

void Qt3DGizmo::setScaleToCameraDistance(bool scaleToCameraDistance) {
    Q_D(Qt3DGizmo);
    d->m_scaleToCameraDistance = scaleToCameraDistance;
    if (!scaleToCameraDistance) {
        d->m_ownTransform->setScale(d->m_scale);
    } else {
        d->adjustScaleToCameraDistance();
    }
    emit scaleToCameraDistanceChanged(scaleToCameraDistance);
}

void Qt3DGizmo::setHideMouseWhileTransforming(bool hideMouseWhileTransforming) {
    Q_D(Qt3DGizmo);
    d->m_hideMouseWhileTransforming = hideMouseWhileTransforming;
}

void Qt3DGizmo::setFlatAppearance(bool flatAppearance) {
    Q_D(Qt3DGizmo);
    if (d->m_flatAppearance != flatAppearance) {
        d->m_flatAppearance = flatAppearance;
        for (int i = 0; i < d->m_handles.size(); i++) {
            d->m_handles[i]->setFlatAppearance(flatAppearance);
        }
    }
    emit flatAppearanceChanged(flatAppearance);
}
