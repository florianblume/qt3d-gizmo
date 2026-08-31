#include "rotationhandle.h"

RotationHandle::RotationHandle(Qt3DCore::QNode *parent,
                               AxisConstraint constraint,
                               const QVector3D &position,
                               const QColor &color,
                               int pickingPriority)
    : Handle(parent, constraint, position, color, pickingPriority) {
    m_torusEntity = new Qt3DCore::QEntity(this);
    m_torusMesh = new Qt3DExtras::QTorusMesh;
    m_torusMesh->setRadius(1.0f);
    m_torusMesh->setMinorRadius(0.01f);
    m_torusMesh->setRings(100);
    m_torusMesh->setSlices(100);
    m_torusEntity->addComponent(m_torusMesh);
    m_torusEntity->addComponent(m_flatMaterial);

    m_torusHighlightEntity = new Qt3DCore::QEntity(this);
    m_torusHighlightMesh = new Qt3DExtras::QTorusMesh;
    m_torusHighlightMesh->setRadius(1.01f);
    m_torusHighlightMesh->setMinorRadius(0.015f);
    m_torusHighlightMesh->setRings(100);
    m_torusHighlightMesh->setSlices(100);
    m_torusHighlightEntity->addComponent(m_torusHighlightMesh);
    addHighlightEntity(m_torusHighlightEntity);
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
