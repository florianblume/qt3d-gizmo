#ifndef QT3DGIZMO_P_H
#define QT3DGIZMO_P_H

#include "qt3dgizmo.h"
#include "rotationhandle.h"
#include "translationhandle.h"

#include <QVector3D>
#include <QList>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>

class Qt3DGizmoPrivate : public QObject {

public:

enum RotationAxis {
    X,
    Y,
    Z
};

public:
    Qt3DGizmoPrivate();

    Q_DECLARE_PUBLIC(Qt3DGizmo)

    void generate3DRayFromScreenToInfinity(int x, int y);
    void createPlane(const QVector3D &position, Qt3DGizmo::TranslationConstraint translationConstraint);

    Qt3DGizmo::Mode m_currentMode = Qt3DGizmo::Mode::Translation;

    Qt3DCore::QTransform *m_delegateTransform;
    Qt3DCore::QTransform *m_ownTransform;

    Qt3DRender::QCamera *m_camera;

    QSize m_windowSize;

    Qt3DGizmo::TranslationConstraint m_currentConstraint;
    RotationAxis m_currentRotationAxis;

    QVector3D m_origin;
    QVector3D m_destination;

    TranslationHandle *m_translationHandleX;
    QObject *m_translationHandleXCameraConnectionContext;
    TranslationHandle *m_translationHandleY;
    QObject *m_translationHandleYCameraConnectionContext;
    TranslationHandle *m_translationHandleZ;
    QObject *m_translationHandleZCameraConnectionContext;

    QList<TranslationHandle*> m_translationHandles;
    QList<QObject*> m_translationHandlesCameraConnectionContexts;

    RotationHandle *m_rotationHandleX;
    RotationHandle *m_rotationHandleY;
    RotationHandle *m_rotationHandleZ;

private:
    Qt3DGizmo *q_ptr;
};

#endif // QT3DGIZMO_P_H
