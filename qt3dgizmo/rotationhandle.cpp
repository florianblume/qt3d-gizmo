#include "rotationhandle.h"

RotationHandle::RotationHandle(Qt3DCore::QNode *parent, const QVector3D &position, const QColor &color)
    : Handle(parent, position, color) {
    Qt3DCore::QEntity *entity = new Qt3DCore::QEntity(this);
    entity->addComponent(m_material);
    m_torusMesh = new Qt3DExtras::QTorusMesh;
    m_torusMesh->setRadius(1.0f);
    m_torusMesh->setMinorRadius(0.01f);
    m_torusMesh->setRings(100);
    m_torusMesh->setSlices(100);
    entity->addComponent(m_torusMesh);
}
