#ifndef HANDLE_H
#define HANDLE_H

#include <QObject>
#include <QColor>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QPhongAlphaMaterial>

class Handle : public Qt3DCore::QEntity {

    Q_OBJECT

public:
    Handle(Qt3DCore::QNode *parent, const QVector3D &position,
           const QString &label, const QColor &color);
    Qt3DCore::QTransform *transform();

public Q_SLOTS:
    void setCamera(Qt3DRender::QCamera *camera);
    void setLabel(const QString &label);
    void setColor(const QColor &color);
    void setHighlightOnHover(bool highlightOnHover);
    void invertTextRotation(const QMatrix4x4 &viewMatix);

Q_SIGNALS:
    void pressed(Qt3DRender::QPickEvent *event);
    void moved(Qt3DRender::QPickEvent *event);

private Q_SLOTS:
    void onMoved();
    void onExited();

protected:
    QString m_label;
    QColor m_color;
    bool m_highlightOnHover = true;
    float m_hightlightColorOffset = 150;
    QObject *m_cameraConnectionContext = Q_NULLPTR;
    Qt3DCore::QTransform *m_transform;
    Qt3DRender::QObjectPicker *m_picker;
    Qt3DExtras::QText2DEntity *m_labelEntity;
    Qt3DCore::QTransform *m_labelEntityTransform;
    Qt3DExtras::QPhongAlphaMaterial *m_material;
};

#endif // HANDLE_H
