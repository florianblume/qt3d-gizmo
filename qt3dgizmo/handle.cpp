#include "handle.h"

#include <QtMath>

Handle::Handle(Qt3DCore::QNode *parent, const QVector3D &position, const QString &label, const QColor &color)
    : Qt3DCore::QEntity(parent)
    , m_color(color)
    , m_transform(new Qt3DCore::QTransform)
    , m_picker(new Qt3DRender::QObjectPicker)
    , m_labelEntity(new Qt3DExtras::QText2DEntity)
    , m_labelEntityTransform(new Qt3DCore::QTransform)
    , m_mouseDevice(new Qt3DInput::QMouseDevice)
    , m_mouseHandler(new Qt3DInput::QMouseHandler) {
    addComponent(m_transform);
    m_transform->setTranslation(position);
    m_labelEntity->setParent(this);
    m_labelEntity->addComponent(m_labelEntityTransform);
    m_labelEntity->setText(label);
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
}

void Handle::invertTextRotation(const QMatrix4x4 &viewMatrix) {
    float values[9];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            values[(i * 3) + j]  = viewMatrix.row(i)[j];
        }
    }
    QMatrix3x3 rotationMatrix = QMatrix3x3(values);
    QQuaternion rotation = QQuaternion::fromRotationMatrix(rotationMatrix);
    m_labelEntityTransform->setRotation((rotation * m_transform->rotation()).inverted());
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
    if (m_cameraConnectionContext) {
        // Delete context to remove old camera connection
        delete m_cameraConnectionContext;
    }
    m_cameraConnectionContext = new QObject();
    invertTextRotation(camera->viewMatrix());
    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
           m_cameraConnectionContext, [this, camera](){
        invertTextRotation(camera->viewMatrix());
    });
}

void Handle::setLabel(const QString &label) {
    m_label = label;
    m_labelEntity->setText(label);
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
