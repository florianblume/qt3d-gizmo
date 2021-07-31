#include "qt3dgizmo.h"
#include "qt3dgizmo_p.h"

#include <QDebug>

#include <QSurfaceFormat>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>

#include <Qt3DExtras/QPhongMaterial>

Qt3DGizmoPrivate::Qt3DGizmoPrivate() {

}

void Qt3DGizmoPrivate::generate3DRayFromScreenToInfinity(int x, int y) {

}

void Qt3DGizmoPrivate::createPlane(const QVector3D &position, Qt3DGizmo::TranslationConstraint translationConstraint) {

}

void Qt3DGizmoPrivate::initializeTranslation(int x, int y, Qt3DGizmo::TranslationConstraint translationConstraint) {
    m_currentConstraint = translationConstraint;
    // TODO Shoot ray and continue
}

void Qt3DGizmoPrivate::initializeRotation(int x, int y, Qt3DGizmo::RotationConstraint translationConstraint) {

}

void Qt3DGizmoPrivate::update(int x, int y) {
    // TODO calculate difference
}

Qt3DGizmo::Qt3DGizmo(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , d_ptr(new Qt3DGizmoPrivate) {
    Q_D(Qt3DGizmo);

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

    d->m_translationHandleX = new TranslationHandle(this, TranslationHandle::Arrow, {0, 0, 0}, "x", Qt::blue);
    d->m_translationHandleX->transform()->setRotationZ(90);
    d->m_translationHandles.append(d->m_translationHandleX);
    d->m_translationHandleY = new TranslationHandle(this, TranslationHandle::Arrow, {0, 0, 0}, "y", Qt::green);
    d->m_translationHandles.append(d->m_translationHandleY);
    d->m_translationHandleZ = new TranslationHandle(this, TranslationHandle::Arrow, {0, 0, 0}, "z", Qt::red);
    d->m_translationHandleZ->transform()->setRotationX(90);
    d->m_translationHandles.append({d->m_translationHandleX,
                                    d->m_translationHandleY,
                                    d->m_translationHandleZ});

    d->m_rotationHandleX = new RotationHandle(this, QVector3D(0, 0, 0), "x", Qt::blue);
    d->m_rotationHandleX->transform()->setRotationY(90);
    d->m_rotationHandleY = new RotationHandle(this, QVector3D(0, 0, 0), "y", Qt::green);
    d->m_rotationHandleY->transform()->setRotationX(90);
    d->m_rotationHandleZ = new RotationHandle(this, QVector3D(0, 0, 0), "z", Qt::red);
    d->m_rotationHandles.append({d->m_rotationHandleX,
                                 d->m_rotationHandleY,
                                 d->m_rotationHandleZ});
    for (Handle* handle : d->m_rotationHandles) {
        handle->setEnabled(false);
    }
}

Qt3DGizmo::Mode Qt3DGizmo::mode() {
    Q_D(Qt3DGizmo);
    return d->m_currentMode;
}

QSize Qt3DGizmo::windowSize() {
    Q_D(Qt3DGizmo);
    return d->m_windowSize;
}

Qt3DCore::QTransform *Qt3DGizmo::delegateTransform() {
    Q_D(Qt3DGizmo);
    return d->m_delegateTransform;
}

Qt3DRender::QCamera *Qt3DGizmo::camera() {
    Q_D(Qt3DGizmo);
    return d->m_camera;
}

void Qt3DGizmo::setMode(Mode mode) {
    // Gets called externally or by clicking the sphere
    Q_D(Qt3DGizmo);
    d->m_currentMode = mode;
    for (TranslationHandle *translationHandle : d->m_translationHandles) {
        translationHandle->setEnabled(mode == Translation);
    }
    for (RotationHandle *rotationHandle : d->m_rotationHandles) {
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
