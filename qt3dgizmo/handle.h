#ifndef HANDLE_H
#define HANDLE_H

#include "flatmaterial.h"
#include "transparentobjectpicker.h"
#include "ray.h"

#include <QObject>
#include <QColor>
#include <QPair>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DExtras/QPhongMaterial>

class Handle : public Qt3DCore::QEntity {

public:
    enum Type {
        Transform,
        ModeSwitcher
    };

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
    Handle(Qt3DCore::QNode *parent, const QVector3D &position, const QColor &color);
    Qt3DCore::QTransform *transform() const;

public slots:
    void setColor(const QColor &color);
    void setHighlightColor(const QColor &color);
    void setFlatAppearance(bool flatAppearance);
    virtual void setEnabled(bool enabled);
    QPair<bool, QVector3D> intersectionWithRay(const Ray &ray);
    void setHighlighted(bool highlighted);

signals:
    void pressed(Qt3DRender::QPickEvent *event, AxisConstraint constraint);

protected:
    virtual void handleAppearanceChange() = 0;

protected:
    bool m_enabled = true;
    QColor m_color;
    QColor m_highlightColor;
    Qt3DCore::QTransform *m_transform;
    TransparentObjectPicker *m_picker;
    FlatMaterial *m_flatMaterial;
    Qt3DExtras::QPhongMaterial *m_phongMaterial;
    bool m_flatAppearance = true;
};

#endif // HANDLE_H
