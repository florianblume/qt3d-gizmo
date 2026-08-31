#include "planetranslationhandle.h"

PlaneTranslationHandle::PlaneTranslationHandle(Qt3DCore::QNode *parent,
                                               AxisConstraint constraint,
                                               const QVector3D &position,
                                               const QColor &color,
                                               int pickingPriority)
    : Handle(parent, constraint, position, color, pickingPriority) {

    m_transform->setTranslation(position);
    m_planeFrontEntity = new Qt3DCore::QEntity(this);
    m_planeMeshFront = new Qt3DExtras::QPlaneMesh;
    m_planeMeshFront->setWidth(0.2f);
    m_planeMeshFront->setHeight(0.2f);
    m_planeFrontEntity->addComponent(m_planeMeshFront);
    m_planeFrontEntity->addComponent(m_flatMaterial);

    m_planeHighlightEntity = new Qt3DCore::QEntity(this);
    m_planeHighlightTransform = new Qt3DCore::QTransform;
    m_planeHighlightTransform->setTranslation(QVector3D(0.f, 0.f, 0.001f));
    m_planeHighlightMesh = new Qt3DExtras::QPlaneMesh;
    m_planeHighlightMesh->setWidth(0.22f);
    m_planeHighlightMesh->setHeight(0.22f);
    m_planeHighlightEntity->addComponent(m_planeHighlightTransform);
    m_planeHighlightEntity->addComponent(m_planeHighlightMesh);
    addHighlightEntity(m_planeHighlightEntity);

    m_planeBackEntity = new Qt3DCore::QEntity(this);
    m_planeBackTransform = new Qt3DCore::QTransform;
    m_planeMeshBack = new Qt3DExtras::QPlaneMesh;
    m_planeMeshBack->setWidth(0.2f);
    m_planeMeshBack->setHeight(0.2f);
    m_planeBackTransform = new Qt3DCore::QTransform;
    m_planeBackTransform->setRotationX(180);
    m_planeBackEntity->addComponent(m_planeBackTransform);
    m_planeBackEntity->addComponent(m_planeMeshBack);
}

void PlaneTranslationHandle::handleAppearanceChange() {
    if (m_flatAppearance) {
        m_planeFrontEntity->removeComponent(m_phongMaterial);
        m_planeFrontEntity->addComponent(m_flatMaterial);
    } else {
        m_planeFrontEntity->removeComponent(m_flatMaterial);
        m_planeFrontEntity->addComponent(m_phongMaterial);
    }
}
