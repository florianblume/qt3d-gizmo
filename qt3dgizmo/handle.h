#ifndef HANDLE_H
#define HANDLE_H

#include <QObject>
#include <QColor>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QMouseHandler>

class Handle : public Qt3DCore::QEntity {

    Q_OBJECT

public:
    Handle(Qt3DCore::QNode *parent, const QVector3D &position, const QColor &color);
    Qt3DCore::QTransform *transform() const;

public Q_SLOTS:
    void setCamera(Qt3DRender::QCamera *camera);
    void setColor(const QColor &color);
    void setHighlightOnHover(bool highlightOnHover);

Q_SIGNALS:
    void pressed(Qt3DRender::QPickEvent *event);
    void moved(Qt3DRender::QPickEvent *event);

private Q_SLOTS:
    void onMoved();
    void onExited();

protected:
    QColor m_color;
    Qt3DRender::QCamera *m_camera;
    bool m_highlightOnHover = true;
    float m_hightlightColorOffset = 180;
    Qt3DCore::QTransform *m_transform;
    Qt3DRender::QObjectPicker *m_picker;
    Qt3DExtras::QPhongMaterial *m_material;
    Qt3DInput::QMouseDevice *m_mouseDevice;
    Qt3DInput::QMouseHandler *m_mouseHandler;
};

#endif // HANDLE_H
