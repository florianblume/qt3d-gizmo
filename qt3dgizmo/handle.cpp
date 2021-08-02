#include "handle.h"

#include <QtMath>

Handle::Handle(Qt3DCore::QNode *parent, const QVector3D &position, const QColor &color)
    : Qt3DCore::QEntity(parent)
    , m_color(color)
    , m_transform(new Qt3DCore::QTransform)
    , m_picker(new Qt3DRender::QObjectPicker)
    , m_mouseDevice(new Qt3DInput::QMouseDevice)
    , m_mouseHandler(new Qt3DInput::QMouseHandler) {
    addComponent(m_transform);
    m_transform->setTranslation(position);
    m_picker->setDragEnabled(true);
    m_picker->setHoverEnabled(true);
    addComponent(m_picker);
    connect(m_picker, &Qt3DRender::QObjectPicker::moved,
            this, &Handle::moved);
    connect(m_picker, &Qt3DRender::QObjectPicker::moved,
            this, &Handle::onMoved);
    connect(m_picker, &Qt3DRender::QObjectPicker::exited,
            this, &Handle::onExited);
    connect(m_picker, &Qt3DRender::QObjectPicker::pressed,
            this, &Handle::pressed);

    m_mouseDevice->setParent(this);
    m_mouseHandler->setSourceDevice(m_mouseDevice);
    addComponent(m_mouseHandler);
    connect(m_mouseHandler, &Qt3DInput::QMouseHandler::exited,
            [](){qDebug() << "test";});

    // For the subclasses to add
    m_material = new Qt3DExtras::QPhongMaterial();
    m_material->setAmbient(color);
    m_material->setShininess(0.0f);
}

void Handle::onMoved() {
    if (m_highlightOnHover) {
        QColor hoverColor = QColor(qMin(255.f, (m_color.red() + m_hightlightColorOffset)),
                                   qMin(255.f, (m_color.green() + m_hightlightColorOffset)),
                                   qMin(255.f, (m_color.blue() + m_hightlightColorOffset)),
                                   255);
        m_material->setAmbient(hoverColor);
    }
}

void Handle::onExited() {
    if (m_highlightOnHover) {
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

Qt3DCore::QTransform *Handle::transform() {
    return m_transform;
}
