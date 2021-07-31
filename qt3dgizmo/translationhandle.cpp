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
    m_material = new Qt3DExtras::QPhongAlphaMaterial;
    m_material->setAmbient(color);
    m_material->setAlpha(1.f);
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

void TranslationHandle::invertTextRotation(const QMatrix4x4 &viewMatrix) {
    float values[9];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            values[(i * 3) + j]  = viewMatrix.row(i)[j];
        }
    }
    QMatrix3x3 rotationMatrix = QMatrix3x3(values);
    QQuaternion rotation = QQuaternion::fromRotationMatrix(rotationMatrix);
    m_labelEntityTransform->setRotation((rotation * m_transform->rotation()).inverted());
}

void TranslationHandle::setCamera(Qt3DRender::QCamera *camera) {
    if (m_cameraConnectionContext) {
        // Delete context to remove old camera connection
        delete m_cameraConnectionContext;
    }
    m_cameraConnectionContext = new QObject();
    invertTextRotation(camera->viewMatrix());
    connect(camera, &Qt3DRender::QCamera::viewMatrixChanged,
           m_cameraConnectionContext, [this, camera](){
        invertTextRotation(camera->viewMatrix());
    });
}

void TranslationHandle::onPressed(Qt3DRender::QPickEvent *event) {

}

void TranslationHandle::onMoved(Qt3DRender::QPickEvent *event) {

}
