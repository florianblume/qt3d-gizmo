#include "rotationhandle.h"

RotationHandle::RotationHandle(Qt3DCore::QNode *parent, const QVector3D &position, const QString &label, const QColor &color)
    : Handle(parent, position, label, color) {
    m_material = new Qt3DExtras::QPhongMaterial();
    m_material->setAmbient(color);
    m_material->setShininess(0.0f);
    addComponent(m_material);
    m_torusMesh = new Qt3DExtras::QTorusMesh;
    m_torusMesh->setRadius(1.0f);
    m_torusMesh->setMinorRadius(0.01f);
    m_torusMesh->setRings(100);
    m_torusMesh->setSlices(100);
    addComponent(m_torusMesh);
}
