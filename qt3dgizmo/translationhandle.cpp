#include "translationhandle.h"

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DRender/QParameter>

TranslationHandle::TranslationHandle(Qt3DCore::QNode *parent, Type type,
                                     const QVector3D &position, const QString &label,
                                     const QColor &color)
    : Handle(parent, position, label, color)
    , m_type(type) {
    Qt3DCore::QEntity *cylinderEntity = new Qt3DCore::QEntity(this);
    m_cylinderMesh = new Qt3DExtras::QCylinderMesh();
    m_cylinderMesh->setRadius(0.01);
    m_cylinderMesh->setLength(1);
    m_cylinderMesh->setRings(2);
    m_material = new Qt3DExtras::QPhongMaterial;
    m_material->setAmbient(color);
    m_material->setShininess(0.0f);
    m_material->setShareable(true);
    Qt3DCore::QTransform *cylinderTransform = new Qt3DCore::QTransform;
    cylinderTransform->setTranslation(QVector3D(0.0f, 0.5f, 0.0f));
    cylinderEntity->addComponent(m_cylinderMesh);
    cylinderEntity->addComponent(m_material);
    cylinderEntity->addComponent(cylinderTransform);

    Qt3DCore::QEntity *coneEntity = new Qt3DCore::QEntity(this);
    m_coneMesh = new Qt3DExtras::QConeMesh();
    m_coneMesh->setRings(2);
    m_coneMesh->setTopRadius(0);
    m_coneMesh->setBottomRadius(0.05);
    m_coneMesh->setHasBottomEndcap(true);
    m_coneMesh->setLength(0.2f);
    Qt3DCore::QTransform *coneTransform = new Qt3DCore::QTransform;
    coneTransform->setTranslation(QVector3D(0.f, 1.0f, 0.f));
    coneEntity->addComponent(m_coneMesh);
    coneEntity->addComponent(m_material);
    coneEntity->addComponent(coneTransform);

    m_labelEntity = new Qt3DExtras::QText2DEntity(this);
    m_labelEntity->setText(label);
    m_labelEntity->setColor(color);
    m_labelEntity->setFont(QFont("Monospace", 1, 5));
    m_labelEntityTransform = new Qt3DCore::QTransform;
    m_labelEntityTransform->setTranslation(QVector3D(0.06f, 0.85f, 0.06f));
    m_labelEntity->addComponent(m_labelEntityTransform);
    m_labelEntity->setWidth(5);
    m_labelEntity->setHeight(5);
    m_labelEntityTransform->setScale(0.07);

    //m_transform->setTranslation(QVector3D(0.f, 0.5f, 0.f));

    switch (type) {
    case Arrow:
        break;
    case Plane:
        break;
    default:
        // Nothing to do here, can never occur
        break;
    }
}
