#include "handle.h"
#include "flatmaterial.h"

#include <QtMath>

Handle::Handle(Qt3DCore::QNode *parent, const QVector3D &position, const QColor &color)
    : Qt3DCore::QEntity(parent)
    , m_color(color)
    , m_transform(new Qt3DCore::QTransform) {
    //, m_picker(new TransparentObjectPicker) {
    addComponent(m_transform);
    m_transform->setTranslation(position);
    //addComponent(m_picker);

    // For the subclasses to add
    m_phongMaterial = new Qt3DExtras::QPhongMaterial();
    m_phongMaterial->setAmbient(color);
    m_phongMaterial->setShininess(0.0f);

    m_flatMaterial = new FlatMaterial();
    m_flatMaterial->setColor(color);

    m_highlightColor = QColor(255, 255, 180);

    /*
    m_computeCommand = new Qt3DRender::QComputeCommand();
    m_computeCommand->setRunType(Qt3DRender::QComputeCommand::Continuous);
    addComponent(m_computeCommand);
    // Problem is 1D (compute the two rays), i.e. Y and Z must be 1
    m_computeCommand->setWorkGroupX(1);
    m_computeCommand->setWorkGroupY(1);
    m_computeCommand->setWorkGroupZ(1);
    */
}


void Handle::setHighlighted(bool highlighted) {
    if (highlighted) {
        m_phongMaterial->setAmbient(m_highlightColor);
        m_flatMaterial->setColor(m_highlightColor);
    } else {
        m_phongMaterial->setAmbient(m_color);
        m_flatMaterial->setColor(m_color);
    }
}

void Handle::setColor(const QColor &color) {
    m_color = color;
}

void Handle::setHighlightColor(const QColor &color) {
    m_highlightColor = color;
}

void Handle::setFlatAppearance(bool flatAppearance) {
    // TODO remove check, this happens in the main class already
    if (m_flatAppearance != flatAppearance) {
        // Appearance actually changed
        m_flatAppearance = flatAppearance;
        handleAppearanceChange();
    }
}

void Handle::setEnabled(bool enabled) {
    m_enabled = enabled;
    // We probably don't need that
    // m_picker->setEnabled(enabled);
    if (!enabled) {
        // If the user wants to disable the gizmo,
        // we can simply deactivate every material
        m_flatMaterial->setEnabled(enabled);
        m_phongMaterial->setEnabled(enabled);
    } else {
        // Else we need to check for the last state
        // and enable the respective material
        if (m_flatAppearance) {
            m_flatMaterial->setEnabled(true);
        } else {
            m_phongMaterial->setEnabled(true);
        }
    }
}

QPair<bool, QVector3D> Handle::intersectionWithRay(const Ray &ray) {
    // m_picker automatically retrieves all geometries related to
    // the QEntity it is attached to, so we don't have to do
    // anything else here
    if (m_enabled) {
        //return m_picker->intersectionWithRay(ray);
    } else {
        return QPair<bool, QVector3D>(false, QVector3D());
    }
}

Qt3DCore::QTransform *Handle::transform() const {
    return m_transform;
}
