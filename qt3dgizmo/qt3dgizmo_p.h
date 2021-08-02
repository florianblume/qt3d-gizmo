#ifndef QT3DGIZMO_P_H
#define QT3DGIZMO_P_H

#include "qt3dgizmo.h"
#include "rotationhandle.h"
#include "arrowtranslationhandle.h"
#include "planetranslationhandle.h"

#include <QVector3D>
#include <QList>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>

class Qt3DGizmoPrivate : public QObject {

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
    Qt3DGizmo::RotationConstraint m_currentRotationConstraint;

    QVector3D m_origin;
    QVector3D m_destination;

    // Sphere to switch modes
    Qt3DCore::QEntity *m_sphereEntity;
    Qt3DExtras::QPhongMaterial *m_sphereMaterial;
    Qt3DExtras::QSphereMesh *m_sphereMesh;
    Qt3DRender::QObjectPicker *m_sphereObjectPicker;
    Qt3DCore::QTransform *m_sphereTransform;

    ArrowTranslationHandle *m_translationHandleX;
    ArrowTranslationHandle *m_translationHandleY;
    ArrowTranslationHandle *m_translationHandleZ;
    PlaneTranslationHandle *m_translationHandleXY;
    PlaneTranslationHandle *m_translationHandleYZ;
    PlaneTranslationHandle *m_translationHandleXZ;
    QList<ArrowTranslationHandle*> m_translationHandles;

    RotationHandle *m_rotationHandleX;
    RotationHandle *m_rotationHandleY;
    RotationHandle *m_rotationHandleZ;
    QList<RotationHandle*> m_rotationHandles;

public Q_SLOTS:
    void initializeTranslation(int x, int y, Qt3DGizmo::TranslationConstraint translationConstraint);
    void initializeRotation(int x, int y, Qt3DGizmo::RotationConstraint translationConstraint);
    void update(int x, int y);

private:
    Qt3DGizmo *q_ptr;
};

#endif // QT3DGIZMO_P_H
