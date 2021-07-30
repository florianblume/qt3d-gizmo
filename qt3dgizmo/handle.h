#ifndef HANDLE_H
#define HANDLE_H

#include <QObject>
#include <QColor>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DExtras/QText2DEntity>

class Handle : public Qt3DCore::QEntity {

    Q_OBJECT

    Q_PROPERTY(QString m_label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QColor m_color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool m_highlightOnHover READ highlightOnHover WRITE setHighlightOnHover NOTIFY highlightOnHoverChanged)

public:
    Handle(const QVector3D &position, const QString &label, const QColor &color);
    QString label();
    QColor color();
    bool highlightOnHover();
    Qt3DCore::QTransform *transform();

public Q_SLOTS:
    void setLabel(const QString &label);
    void setColor(const QColor &color);
    void setHighlightOnHover(bool highlightOnHover);

Q_SIGNALS:
    void pressed(const QVector3D &position);
    void labelChanged(const QString &label);
    void colorChanged(const QColor &color);
    void highlightOnHoverChanged(bool highlightOnHover);

protected Q_SLOTS:
    virtual void onPressed(Qt3DRender::QPickEvent *event) = 0;
    virtual void onMoved(Qt3DRender::QPickEvent *event) = 0;

protected:
    QString m_label;
    QColor m_color;
    bool m_highlightOnHover = true;
    Qt3DCore::QTransform *m_transform;
    Qt3DRender::QObjectPicker *m_picker;
    Qt3DExtras::QText2DEntity *m_labelEntity;
    Qt3DCore::QTransform *m_labelEntityTransform;
};

#endif // HANDLE_H
