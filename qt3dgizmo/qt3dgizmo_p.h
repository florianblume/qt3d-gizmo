#ifndef QT3DGIZMO_P_H
#define QT3DGIZMO_P_H

#include "qt3dgizmo.h"
#include "plane.h"
#include "ray.h"
#include "constants.h"
#include "rotationhandle.h"
#include "arrowtranslationhandle.h"
#include "planetranslationhandle.h"
#include "raycomputematerial.h"

#include <QVector3D>
#include <QList>
#include <QPair>
#include <QtMath>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DCore/QEntity>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DRender/QViewport>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QComputeCommand>

class Qt3DGizmoPrivate : public QObject {

public:
    // Struct to store triplets of camera, viewport, surface
    // since we need all three for ray casting
    struct CameraViewportSurface {
        Qt3DRender::QCamera *camera;
        Qt3DRender::QViewport *viewport;
        QSurface *surface;
    };

    Qt3DGizmoPrivate();

    Q_DECLARE_PUBLIC(Qt3DGizmo)

    static Plane initializeTranslationPlane(const Ray &clickRay, const QVector3D &position,
                                            Handle::AxisConstraint translationConstraint);
    static Plane initializeRotationPlane(const QVector3D &position,
                                         Handle::AxisConstraint translationConstraint);
    static QVector3D computePlaneNormal(const Ray &ray, Handle::AxisConstraint translationConstraint);
    QVector3D applyTranslationConstraint(const QVector3D &position, const QVector3D &intersectionPosition,
                                         Handle::AxisConstraint translationConstraint);
    void initialize(Qt3DRender::QPickEvent *event, Handle::AxisConstraint axisConstraint);
    void update(int x, int y);
    void removeHighlightsFromHanldes();
    void adjustScaleToCameraDistance();

    // Configurable properties
    float m_scale = 1.f;
    bool m_scaleToCameraDistance = true;
    bool m_hideMouseWhileTransforming = true;
    bool m_currentlyHidingMouse = false;
    bool m_flatAppearance = true;

    Qt3DGizmo::TransformationMode m_currentMode = Qt3DGizmo::TransformationMode::Translation;

    CameraViewportSurface m_cameraViewportSurfaceTriplet;
    bool m_detectFramegraphAutomatically = false;

    Handle::AxisConstraint m_axisConstraint;

    bool m_mouseDownOnHandle = false;

    QColor m_handleHighlightColor = QColor(255, 255, 180);

    Ray m_rayFromClickPosition;
    Plane m_plane;
    QVector3D m_currentTranslationPosition;
    QVector3D m_translationDisplacement;
    // Last position on one of the rotation handles
    QVector3D m_lastPositionOnRotationHandle;
    // Rotation before the user starts rotating
    QQuaternion m_initialOrientation;

    // To handle mouse movement and clicks
    // Events will be delegated to the compute material
    // and the resulting ray to the handles
    Qt3DInput::QMouseDevice *m_mouseDevice;
    Qt3DInput::QMouseHandler *m_mouseHandler;
    RayComputeMaterial *m_computeMaterial;
    Qt3DRender::QComputeCommand *m_computeCommand;

    Qt3DCore::QTransform *m_delegateTransform;
    Qt3DCore::QTransform *m_ownTransform;
    QMetaObject::Connection m_delegateTransformTranslationChangedConnection;
    // Probably don't need this anymore since we'll take care of scaling
    // in the shaders to account for different viewports and different cameras
    QMetaObject::Connection m_delegateTransformAdjustScaleConnection;

    QMetaObject::Connection m_cameraViewMatrixChangedConnection;
    Qt3DRender::QCamera *m_camera = nullptr;
    Qt3DRender::QViewport *m_viewport;

    // TODO Put in own class to make it work with new approach
    // Sphere to switch modes
    float m_sphereHighlightScale = 1.4f;
    Qt3DCore::QEntity *m_sphereEntity;
    Qt3DExtras::QPhongMaterial *m_spherePhongMaterial;
    FlatMaterial *m_sphereFlatMaterial;
    QColor m_sphereNormalColor = QColor(50, 50, 50, 50);
    Qt3DExtras::QSphereMesh *m_sphereMesh;
    Qt3DRender::QObjectPicker *m_sphereObjectPicker;
    Qt3DCore::QTransform *m_sphereTransform;

    ArrowTranslationHandle *m_translationHandleX;
    ArrowTranslationHandle *m_translationHandleY;
    ArrowTranslationHandle *m_translationHandleZ;
    PlaneTranslationHandle *m_translationHandleXY;
    PlaneTranslationHandle *m_translationHandleYZ;
    PlaneTranslationHandle *m_translationHandleXZ;
    QList<Handle*> m_translationHandles;

    RotationHandle *m_rotationHandleX;
    RotationHandle *m_rotationHandleY;
    RotationHandle *m_rotationHandleZ;
    QList<Handle*> m_rotationHandles;

public slots:
    void onMouseRelease();

private:
    Qt3DGizmo *q_ptr;
};

#endif // QT3DGIZMO_P_H
