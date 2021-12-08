#include "arrowtranslationhandle.h"

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DRender/QParameter>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QConeMesh>

ArrowTranslationHandle::ArrowTranslationHandle(Qt3DCore::QNode *parent,
                                               AxisConstraint constraint,
                                               const QVector3D &position,
                                               const QColor &color)
    : TransformHandle(parent, constraint, position, color) {

    m_cylinderEntity = new Qt3DCore::QEntity(this);
    m_cylinderMesh = new Qt3DExtras::QCylinderMesh();
    m_cylinderMesh->setRadius(0.01);
    m_cylinderMesh->setLength(0.8);
    m_cylinderMesh->setRings(2);
    m_cylinderTransform = new Qt3DCore::QTransform;
    m_cylinderTransform->setTranslation(QVector3D(0.0f, 0.4f, 0.0f));
    m_cylinderEntity->addComponent(m_cylinderMesh);
    m_cylinderEntity->addComponent(m_flatMaterial);
    m_cylinderEntity->addComponent(m_cylinderTransform);
    //m_picker->setGeometry(m_cylinderMesh->geometry());

    m_coneEntity = new Qt3DCore::QEntity(this);
    m_coneMesh = new Qt3DExtras::QConeMesh();
    m_coneMesh->setRings(2);
    m_coneMesh->setTopRadius(0);
    m_coneMesh->setBottomRadius(0.05);
    m_coneMesh->setHasBottomEndcap(true);
    m_coneMesh->setLength(0.2f);
    m_coneTransform = new Qt3DCore::QTransform;
    m_coneTransform->setTranslation(QVector3D(0.f, 0.9f, 0.f));
    m_coneEntity->addComponent(m_coneMesh);
    m_coneEntity->addComponent(m_flatMaterial);
    m_coneEntity->addComponent(m_coneTransform);
}

Qt3DRender::QGeometry *ArrowTranslationHandle::geometry() {
    return m_cylinderMesh->geometry();
}

void ArrowTranslationHandle::setColor(const QColor &color) {
    Handle::setColor(color);
}

void ArrowTranslationHandle::setEnabled(bool enabled) {
    Handle::setEnabled(enabled);
}

void ArrowTranslationHandle::handleAppearanceChange() {
    if (m_flatAppearance) {
        m_cylinderEntity->removeComponent(m_phongMaterial);
        m_cylinderEntity->addComponent(m_flatMaterial);
        m_coneEntity->removeComponent(m_phongMaterial);
        m_coneEntity->addComponent(m_flatMaterial);
    } else {
        m_cylinderEntity->removeComponent(m_flatMaterial);
        m_cylinderEntity->addComponent(m_phongMaterial);
        m_coneEntity->removeComponent(m_flatMaterial);
        m_coneEntity->addComponent(m_phongMaterial);
    }
}
