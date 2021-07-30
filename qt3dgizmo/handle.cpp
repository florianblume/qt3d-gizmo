#include "handle.h"

Handle::Handle(const QVector3D &position, const QString &label, const QColor &color)
    : m_color(color)
    , m_transform(new Qt3DCore::QTransform)
    , m_picker(new Qt3DRender::QObjectPicker)
    , m_labelEntity(new Qt3DExtras::QText2DEntity)
    , m_labelEntityTransform(new Qt3DCore::QTransform) {
    addComponent(m_transform);
    m_transform->setTranslation(position);
    addComponent(m_picker);
    m_labelEntity->setParent(this);
    m_labelEntity->addComponent(m_labelEntityTransform);
    m_labelEntity->setText(label);
}

QString Handle::label() {
    return m_label;
}

void Handle::setLabel(const QString &label) {
    m_label = label;
    m_labelEntity->setText(label);
}

QColor Handle::color() {
    return m_color;
}

void Handle::setColor(const QColor &color) {
    m_color = color;
}

bool Handle::highlightOnHover() {
    return m_highlightOnHover;
}

void Handle::setHighlightOnHover(bool highlightOnHover) {
    m_highlightOnHover = highlightOnHover;
}

Qt3DCore::QTransform *Handle::transform() {
    return m_transform;
}
