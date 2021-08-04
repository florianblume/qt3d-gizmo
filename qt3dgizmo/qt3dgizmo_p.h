#ifndef QT3DGIZMO_P_H
#define QT3DGIZMO_P_H

#include "qt3dgizmo.h"
#include "rotationhandle.h"
#include "arrowtranslationhandle.h"
#include "planetranslationhandle.h"

#include <QVector3D>
#include <QList>
#include <QPair>
#include <QtMath>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DRender/QViewport>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QMouseHandler>

#define EPSILON 2.5e-8

class Plane {

public:
    Plane() {
    }

    Plane(const QVector3D &position, const QVector3D &normal) {
        this->normal = normal;
        this->position = position;
    }

    // TODO ATTENTION! This is OpenGL normal with y being up and down
    QVector3D normal = QVector3D(0.f, 1.f, 0.f);
    QVector3D position;
};

class Ray {

public:
    Ray() {
    }

    Ray(const QVector3D &start, const QVector3D &end)
        : start(start), end(end) {
    }

    QPair<int, QVector3D> intersects(const Plane &plane) const {
        int result = 0;

        QVector3D intersectPoint;

        QVector3D u = start - end;
        QVector3D w = start - plane.position;

        float     D = QVector3D::dotProduct(plane.normal, u);
        float     N = -QVector3D::dotProduct(plane.normal, w);

        if (qAbs(D) < EPSILON)		   // segment is parallel to plane
        {
            if (N == 0)                // segment lies in plane
                result = 2;
            else
                result = 0;            // no intersection
        }

        float s = N / D;
        if (s < 0 || s > 1)
            result = 0;                 // no intersection

        intersectPoint = start + u * s; // compute segment intersect point

        result = 1;
        return qMakePair(result, intersectPoint);
    }

    QVector3D start;
    QVector3D end;
};

class Qt3DGizmoPrivate : public QObject {

public:
    Qt3DGizmoPrivate();

    Q_DECLARE_PUBLIC(Qt3DGizmo)

    // Not static since we need the camera matrices
    Ray generate3DRayFromScreenToInfinity(int x, int y);
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

    Qt3DGizmo::Mode m_currentMode = Qt3DGizmo::Mode::Translation;

    QSize m_windowSize;

    Handle::AxisConstraint m_axisConstraint;

    bool m_mouseDownOnHandle = false;

    Ray m_rayFromClickPosition;
    Plane m_plane;
    QVector3D m_currentTranslationPosition;
    QVector3D m_translationDisplacement;
    // Last position on one of the rotation handles
    QVector3D m_lastPositionOnRotationHandle;

    Qt3DInput::QMouseDevice *m_mouseDevice;
    Qt3DInput::QMouseHandler *m_mouseHandler;

    Qt3DCore::QTransform *m_delegateTransform;
    Qt3DCore::QTransform *m_ownTransform;
    QMetaObject::Connection m_delegateTransformConnection;

    Qt3DRender::QCamera *m_camera;
    Qt3DRender::QViewport *m_viewport;

    // Sphere to switch modes
    float m_sphereHighlightScale = 1.4f;
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
    QList<Handle*> m_translationHandles;

    RotationHandle *m_rotationHandleX;
    RotationHandle *m_rotationHandleY;
    RotationHandle *m_rotationHandleZ;
    QList<Handle*> m_rotationHandles;

public Q_SLOTS:
    void onMouseRelease();

private:
    Qt3DGizmo *q_ptr;
};

#endif // QT3DGIZMO_P_H
