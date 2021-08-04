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

class Handle : public Qt3DCore::QEntity {

public:
    enum AxisConstraint {
        XTrans,
        YTrans,
        ZTrans,
        XYTrans,
        XZTrans,
        YZTrans
    };

    Q_ENUM(AxisConstraint)

    Q_OBJECT

public:
    Handle(Qt3DCore::QNode *parent, AxisConstraint constraint, const QVector3D &position, const QColor &color);
    Qt3DCore::QTransform *transform() const;
    AxisConstraint axisConstraint();

public Q_SLOTS:
    void setCamera(Qt3DRender::QCamera *camera);
    void setColor(const QColor &color);
    void setHighlightColor(const QColor &color);
    void setHighlightOnHover(bool highlightOnHover);
    void setIsDragged(bool isDragged);

Q_SIGNALS:
    void pressed(Qt3DRender::QPickEvent *event, AxisConstraint constraint);

private Q_SLOTS:
    void onMoved();
    void onExited();
    void onPressed(Qt3DRender::QPickEvent *event);
    void onReleased();

protected:
    void setHighlighted(bool highlighted);

protected:
    AxisConstraint m_axisConstraint;
    QColor m_color;
    QColor m_highlightColor;
    Qt3DRender::QCamera *m_camera;
    bool m_highlightOnHover;
    bool m_isDragged;
    Qt3DCore::QTransform *m_transform;
    Qt3DRender::QObjectPicker *m_picker;
    Qt3DExtras::QPhongMaterial *m_material;
};

#endif // HANDLE_H
