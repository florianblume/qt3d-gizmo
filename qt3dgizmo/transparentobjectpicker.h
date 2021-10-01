#ifndef TRANSPARENTOBJECTPICKER_H
#define TRANSPARENTOBJECTPICKER_H

#include <QObject>
#include <QEvent>
#include <QMatrix4x4>
#include <Qt3DCore/QEntity>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DRender/QPickEvent>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>

class TransparentObjectPicker : public Qt3DInput::QMouseHandler {

    Q_OBJECT

public:
    explicit TransparentObjectPicker(Qt3DCore::QNode *parent = nullptr);

public slots:
    void setViewMatrix(const QMatrix4x4 &viewMatrix);
    void setProjectionMatrix(const QMatrix4x4 &projectionMatrix);
    void setViewport(const QRect &viewport);
    void setWindowSize(const QSize &windowSize);

signals:
    // Qt3DRender::QPickEvent is overkill but already registered as a meta class
    // and I'm lazy (without meta no signals and slots)
    void moved(Qt3DRender::QPickEvent *event);
    void exited();
    void pressed(Qt3DRender::QPickEvent *event);

private slots:
    void setEntity(Qt3DCore::QEntity *entity);
    void onExited();
    void onPressed(Qt3DInput::QMouseEvent *mouse);
    void onPositionChanged(Qt3DInput::QMouseEvent *mouse);

private:
    QList<Qt3DRender::QGeometry*> gatherGeometries();

private:
    // Needed for unprojection
    QSize m_windowSize;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;
    QRect m_viewport;

    Qt3DInput::QMouseDevice *m_mouseDevice;
    QScopedPointer<Qt3DRender::QPickEvent> m_pickEvent;
    bool m_mouseOnObject = false;
    QVector3D m_worldIntersection;
    // Not needed by the gizmo
    QVector3D m_localIntersection;
    // Not needed yet
    float m_distance = 0;

    // The entity we use to check for geometries
    Qt3DCore::QEntity *m_entity;
};

#endif // TRANSPARENTOBJECTPICKER_H
