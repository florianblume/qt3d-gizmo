#include "rotationhandle.h"

RotationHandle::RotationHandle(Qt3DCore::QNode *parent, AxisConstraint constraint,
                               const QVector3D &position, const QColor &color)
    : Handle(parent, constraint, position, color) {
    m_torusEntity = new Qt3DCore::QEntity(this);
    m_torusMesh = new Qt3DExtras::QTorusMesh;
    m_torusMesh->setRadius(1.0f);
    m_torusMesh->setMinorRadius(0.01f);
    m_torusMesh->setRings(100);
    m_torusMesh->setSlices(100);
    m_torusEntity->addComponent(m_torusMesh);
    m_torusEntity->addComponent(m_flatMaterial);
}

void RotationHandle::handleAppearanceChange() {
    if (m_flatAppearance) {
        m_torusEntity->removeComponent(m_phongMaterial);
        m_torusEntity->addComponent(m_flatMaterial);
    } else {
        m_torusEntity->removeComponent(m_flatMaterial);
        m_torusEntity->addComponent(m_phongMaterial);
    }
}
