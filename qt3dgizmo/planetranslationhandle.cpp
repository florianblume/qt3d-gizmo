#include "planetranslationhandle.h"

PlaneTranslationHandle::PlaneTranslationHandle(Qt3DCore::QNode *parent, const QVector3D &position, const QColor &color)
    : Handle(parent, position, color) {

    m_transform->setTranslation(position);
    m_planeEntity = new Qt3DCore::QEntity(this);
    m_planeTransform = new Qt3DCore::QTransform;
    //m_planeTransform->setTranslation(position);
    m_planeMesh = new Qt3DExtras::QPlaneMesh;
    m_planeMesh->setWidth(0.2f);
    m_planeMesh->setHeight(0.2f);
    m_planeEntity->addComponent(m_planeTransform);
    m_planeEntity->addComponent(m_material);
    m_planeEntity->addComponent(m_planeMesh);
}
