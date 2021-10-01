#include "modeswitcherhandle.h"

ModeSwitcherHandle::ModeSwitcherHandle(Qt3DCore::QNode *parent,
                                       const QVector3D &position,
                                       const QColor &color)
    : Handle(parent, position, color) {
    m_sphereEntity = new Qt3DCore::QEntity(this);
    m_sphereMesh = new Qt3DExtras::QSphereMesh();
    m_sphereMesh->setRadius(0.05);
    m_sphereMesh->setRings(50);
    m_sphereMesh->setSlices(50);
    m_sphereEntity->addComponent(m_sphereMesh);
    m_sphereEntity->addComponent(m_flatMaterial);
    m_flatMaterial->setColor(m_defaultColor);
    m_phongMaterial->setAmbient(m_defaultColor);
}

void ModeSwitcherHandle::handleAppearanceChange() {
    if (m_flatAppearance) {
        m_sphereEntity->removeComponent(m_flatMaterial);
        m_sphereEntity->addComponent(m_phongMaterial);
    } else {
        m_sphereEntity->removeComponent(m_phongMaterial);
        m_sphereEntity->addComponent(m_flatMaterial);
    }
}
