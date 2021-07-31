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

Qt3DGizmo::Qt3DGizmo(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , d_ptr(new Qt3DGizmoPrivate) {
    Q_D(Qt3DGizmo);
    d->m_translationHandleX = new TranslationHandle(this, TranslationHandle::Arrow, {0, 0, 0}, "x", Qt::blue);
    d->m_translationHandleX->transform()->setRotationZ(90);
    d->m_translationHandles.append(d->m_translationHandleX);
    d->m_translationHandleY = new TranslationHandle(this, TranslationHandle::Arrow, {0, 0, 0}, "y", Qt::green);
    d->m_translationHandles.append(d->m_translationHandleY);
    d->m_translationHandleZ = new TranslationHandle(this, TranslationHandle::Arrow, {0, 0, 0}, "z", Qt::red);
    d->m_translationHandleZ->transform()->setRotationX(90);
    d->m_translationHandles.append(d->m_translationHandleZ);

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
    Q_D(Qt3DGizmo);
    d->m_currentMode = mode;
}

void Qt3DGizmo::initialize(int x, int y, TranslationConstraint translationConstraint) {
    Q_D(Qt3DGizmo);
    d->m_currentConstraint = translationConstraint;
    // TODO Shoot ray and continue
}

void Qt3DGizmo::update(int x, int y) {
    Q_D(Qt3DGizmo);
    // TODO calculate difference
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
