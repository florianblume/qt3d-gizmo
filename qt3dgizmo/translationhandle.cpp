#include "translationhandle.h"

TranslationHandle::TranslationHandle(Type type, const QVector3D &position, const QString &label, const QColor &color)
    : Handle(position, label, color)
    , m_type(type) {
    switch (type) {
    case Arrow:
        m_material = new Qt3DExtras::QPhongAlphaMaterial;
        m_material->setAmbient(color);
        m_cylinderMesh = new Qt3DExtras::QCylinderMesh;
        m_cylinderMesh->setRings(100);
        m_cylinderMesh->setSlices(1);
        addComponent(m_cylinderMesh);
        addComponent(m_material);
        break;
    case Plane:
        break;
    default:
        // Nothing to do here, can never occur
        break;
    }
}

void TranslationHandle::onPressed(Qt3DRender::QPickEvent *event) {

}

void TranslationHandle::onMoved(Qt3DRender::QPickEvent *event) {

}
