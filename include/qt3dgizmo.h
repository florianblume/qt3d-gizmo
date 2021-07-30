#ifndef QT3DGIZMO_H
#define QT3DGIZMO_H

#include <QSize>
#include <QMatrix4x4>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QNode>
#include <Qt3DCore/QTransform>

class Qt3DGizmoPrivate;

class Qt3DGizmo : public Qt3DCore::QEntity {

    Q_OBJECT

    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize NOTIFY windowSizeChanged)
    Q_PROPERTY(Qt3DCore::QTransform* delegateTransform READ delegateTransform WRITE setDelegateTransform NOTIFY delegateTransformChanged)
    Q_PROPERTY(QMatrix4x4 viewMatrix READ viewMatrix WRITE setViewMatrix NOTIFY viewMatrixChanged)
    Q_PROPERTY(QMatrix4x4 projectionMatrix READ projectionMatrix WRITE setProjectionMatrix NOTIFY projectionMatrixChanged)

public:

enum TranslationConstraint {
    X,
    Y,
    Z,
    XY,
    XZ,
    YZ
};

Q_ENUM(TranslationConstraint)

enum Mode {
    Translation,
    Rotation
};

Q_ENUM(Mode)

public:
    explicit Qt3DGizmo(Qt3DCore::QNode *parent = nullptr);
    Mode mode();
    QSize windowSize();
    Qt3DCore::QTransform *delegateTransform();
    QMatrix4x4 viewMatrix();
    QMatrix4x4 projectionMatrix();

public Q_SLOTS:
    void setMode(Mode mode);
    void initialize(int x, int y, TranslationConstraint translationConstraint);
    void update(int x, int y);
    void setWindowSize(const QSize &size);
    void setWindowWidth(int width);
    void setWindowHeight(int height);
    void setDelegateTransform(Qt3DCore::QTransform *transform);
    void setViewMatrix(const QMatrix4x4 &viewMatrix);
    void setProjectionMatrix(const QMatrix4x4 &projectionMatrix);

Q_SIGNALS:
    void modeChanged(Mode mode);
    void windowSizeChanged(const QSize &size);
    void delegateTransformChanged(Qt3DCore::QTransform *transform);
    void viewMatrixChanged(const QMatrix4x4 &viewMatrix);
    void projectionMatrixChanged(const QMatrix4x4 &projectionMatrix);

protected:
    Qt3DGizmoPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(Qt3DGizmo)

};

#endif // QT3DGIZMO_H
