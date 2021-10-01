#ifndef QT3DGIZMO_H
#define QT3DGIZMO_H

#include <QSize>
#include <QMatrix4x4>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QNode>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QViewport>
#include <QSurface>

class Qt3DGizmoPrivate;

class Qt3DGizmo : public Qt3DCore::QEntity {

public:
    enum TransformationMode {
        Translation,
        Rotation
    };

    Q_OBJECT

    Q_PROPERTY(TransformationMode transformationMode READ transformationMode WRITE setTransformationMode NOTIFY transformationModeChanged)
    Q_PROPERTY(Qt3DCore::QTransform* delegateTransform READ delegateTransform WRITE
               setDelegateTransform NOTIFY delegateTransformChanged)

    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(bool scaleToCameraDistance READ scaleToCameraDistance WRITE
               setScaleToCameraDistance NOTIFY scaleToCameraDistanceChanged)
    Q_PROPERTY(bool hideMouseWhileTransforming READ hideMouseWhileTransforming
               WRITE setHideMouseWhileTransforming NOTIFY hideMouseWhileTransformingChanged)
    Q_PROPERTY(bool flatAppearance READ flatAppearance WRITE setFlatAppearance NOTIFY flatAppearanceChanged)

    // TODO line width
    // TODO plane transform size
    // TODO plane transform offset
    // TODO arrow translation handle length
    // TODO rotation handle diameter
    // TODO X color
    // TODO y color
    // TODO z color
    // TODO xy color
    // TODO xz color
    // TODO yz color
    // TODO r1 color
    // TODO r2 color
    // TODO r3 color
    // TODO highlight color
    // TODO highlight color absolute or relative
    // TODO font for axis labels
    // TODO show translation axis labels
    // TODO label size
    // TODO set position on delegate transform
    // TODO set allowed modes
    // TODO switch between 3D and 2D appearance
    // TODO set allow mode switching
    // TODO hide mouse when rotating/translating
    // TODO show translation handles
    // TODO show rotation handles
    // TODO show only active handles

    Q_ENUM(TransformationMode)

public:
    explicit Qt3DGizmo(Qt3DCore::QNode *parent = nullptr);
    Q_INVOKABLE TransformationMode transformationMode() const;
    Q_INVOKABLE Qt3DCore::QTransform *delegateTransform() const;

    Q_INVOKABLE float scale() const;
    Q_INVOKABLE bool scaleToCameraDistance() const;
    Q_INVOKABLE bool hideMouseWhileTransforming() const;
    Q_INVOKABLE bool flatAppearance() const;

public slots:
    void setEnabled(bool enabled);
    void setDetectFramegraphAutomatically(bool detectFramegraphAutomatically);
    void setCameraViewportSurface(Qt3DRender::QCamera *camera,
                                  Qt3DRender::QViewport *viewport,
                                  QSurface *surface);
    void setTransformationMode(Qt3DGizmo::TransformationMode mode);
    void setDelegateTransform(Qt3DCore::QTransform *transform);
    void setScale(float scale);
    void setScaleToCameraDistance(bool scaleToCameraDistance);
    void setHideMouseWhileTransforming(bool hideMouseWhileTransforming);
    void setFlatAppearance(bool flatAppearance);

signals:
    void transformationModeChanged(TransformationMode mode);
    void delegateTransformChanged(Qt3DCore::QTransform *transform);
    void scaleChanged(float scale);
    void scaleToCameraDistanceChanged(bool scaleToCameraDistance);
    void hideMouseWhileTransformingChanged(bool hideMouseWhileTransforming);
    void flatAppearanceChanged(bool flatAppearance);

protected:
    Qt3DGizmoPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(Qt3DGizmo)

};

#endif // QT3DGIZMO_H
