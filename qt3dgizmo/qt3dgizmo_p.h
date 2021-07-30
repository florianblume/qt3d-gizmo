#ifndef QT3DGIZMO_P_H
#define QT3DGIZMO_P_H

#include "qt3dgizmo.h"
#include "rotationhandle.h"
#include "translationhandle.h"

#include <QVector3D>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

class Qt3DGizmoPrivate : public QObject {

public:

enum RotationAxis {
    X,
    Y,
    Z
};

public:
    Qt3DGizmoPrivate();

    void generate3DRayFromScreenToInfinity(int x, int y);
    void createPlane(const QVector3D &position, Qt3DGizmo::TranslationConstraint translationConstraint);

    Qt3DGizmo::Mode m_currentMode = Qt3DGizmo::Mode::Translation;

    Qt3DCore::QTransform *m_delegateTransform;
    Qt3DCore::QTransform *m_ownTransform;

    QSize m_windowSize;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;

    Qt3DGizmo::TranslationConstraint m_currentConstraint;
    RotationAxis m_currentRotationAxis;

    QVector3D m_origin;
    QVector3D m_destination;

    TranslationHandle *m_translationHandleX;
    TranslationHandle *m_translationHandleY;
    TranslationHandle *m_translationHandleZ;

    RotationHandle *m_rotationHandleX;
    RotationHandle *m_rotationHandleY;
    RotationHandle *m_rotationHandleZ;
};

#endif // QT3DGIZMO_P_H
