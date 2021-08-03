#include "handle.h"

#include <QtMath>

Handle::Handle(Qt3DCore::QNode *parent, AxisConstraint constraint, const QVector3D &position, const QColor &color)
    : Qt3DCore::QEntity(parent)
    , m_axisConstraint(constraint)
    , m_color(color)
    , m_transform(new Qt3DCore::QTransform)
    , m_picker(new Qt3DRender::QObjectPicker) {
    addComponent(m_transform);
    m_transform->setTranslation(position);
    m_picker->setDragEnabled(true);
    m_picker->setHoverEnabled(true);
    addComponent(m_picker);
    connect(m_picker, &Qt3DRender::QObjectPicker::moved,
            this, &Handle::onMoved);
    connect(m_picker, &Qt3DRender::QObjectPicker::exited,
            this, &Handle::onExited);
    connect(m_picker, &Qt3DRender::QObjectPicker::pressed,
            this, &Handle::onPressed);

    // For the subclasses to add
    m_material = new Qt3DExtras::QPhongMaterial();
    m_material->setAmbient(color);
    m_material->setShininess(0.0f);
}

void Handle::onMoved() {
    if (m_highlightOnHover && !m_isDragged) {
        setHighlighted(true);
    }
}

void Handle::onExited() {
    if (m_highlightOnHover && !m_isDragged) {
        setHighlighted(false);
    }
}

void Handle::onPressed(Qt3DRender::QPickEvent *event) {
    m_isDragged = true;
    setHighlighted(true);
    Q_EMIT pressed(event, m_axisConstraint);
}

void Handle::onReleased() {
    m_isDragged = false;
    setHighlighted(false);
}

void Handle::setHighlighted(bool highlighted) {
    if (highlighted) {
        QColor hoverColor = QColor(qMin(255.f, (m_color.red() + m_hightlightColorOffset)),
                                   qMin(255.f, (m_color.green() + m_hightlightColorOffset)),
                                   qMin(255.f, (m_color.blue() + m_hightlightColorOffset)),
                                   255);
        m_material->setAmbient(hoverColor);
    } else {
        m_material->setAmbient(m_color);
    }
}

void Handle::setCamera(Qt3DRender::QCamera *camera) {
    m_camera = camera;
}

void Handle::setColor(const QColor &color) {
    m_color = color;
}

void Handle::setHighlightOnHover(bool highlightOnHover) {
    m_highlightOnHover = highlightOnHover;
}

void Handle::setIsDragged(bool isDragged) {
    m_isDragged = isDragged;
    setHighlighted(isDragged);
}

Qt3DCore::QTransform *Handle::transform() const {
    return m_transform;
}

Handle::AxisConstraint Handle::axisConstraint() {
    return m_axisConstraint;
}
