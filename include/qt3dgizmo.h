#ifndef QT3DGIZMO_H
#define QT3DGIZMO_H

#include <QSize>
#include <QMatrix4x4>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QNode>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>

class Qt3DGizmoPrivate;

class Qt3DGizmo : public Qt3DCore::QEntity {

public:
    enum Mode {
        Translation,
        Rotation
    };

    Q_OBJECT

    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize NOTIFY windowSizeChanged)
    Q_PROPERTY(Qt3DCore::QTransform* delegateTransform READ delegateTransform WRITE setDelegateTransform NOTIFY delegateTransformChanged)
    Q_PROPERTY(Qt3DRender::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
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
    // TODO font
    // TODO show axis labels
    // TODO label size

    Q_ENUM(Mode)

public:
    explicit Qt3DGizmo(Qt3DCore::QNode *parent = nullptr);
    Mode mode() const;
    QSize windowSize() const;
    Qt3DCore::QTransform *delegateTransform() const;
    Qt3DRender::QCamera *camera() const;

public Q_SLOTS:
    void setMode(Qt3DGizmo::Mode mode);
    void setWindowSize(const QSize &size);
    void setWindowWidth(int width);
    void setWindowHeight(int height);
    void setDelegateTransform(Qt3DCore::QTransform *transform);
    void setCamera(Qt3DRender::QCamera *camera);

Q_SIGNALS:
    void modeChanged(Mode mode);
    void windowSizeChanged(const QSize &size);
    void delegateTransformChanged(Qt3DCore::QTransform *transform);
    void cameraChanged(Qt3DRender::QCamera *camera);

protected:
    Qt3DGizmoPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(Qt3DGizmo)

};

#endif // QT3DGIZMO_H
