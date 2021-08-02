#include "arrowtranslationhandle.h"

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DRender/QParameter>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QConeMesh>

ArrowTranslationHandle::ArrowTranslationHandle(Qt3DCore::QNode *parent, const QVector3D &position,
                                               const QString &label, const QColor &color)
    : Handle(parent, position, color) {

    m_cylinderEntity = new Qt3DCore::QEntity(this);
    m_cylinderMesh = new Qt3DExtras::QCylinderMesh();
    m_cylinderMesh->setRadius(0.01);
    m_cylinderMesh->setLength(1);
    m_cylinderMesh->setRings(2);
    m_cylinderTransform = new Qt3DCore::QTransform;
    m_cylinderTransform->setTranslation(QVector3D(0.0f, 0.5f, 0.0f));
    m_cylinderEntity->addComponent(m_cylinderMesh);
    m_cylinderEntity->addComponent(m_material);
    m_cylinderEntity->addComponent(m_cylinderTransform);

    m_coneEntity = new Qt3DCore::QEntity(this);
    m_coneMesh = new Qt3DExtras::QConeMesh();
    m_coneMesh->setRings(2);
    m_coneMesh->setTopRadius(0);
    m_coneMesh->setBottomRadius(0.05);
    m_coneMesh->setHasBottomEndcap(true);
    m_coneMesh->setLength(0.2f);
    m_coneTransform = new Qt3DCore::QTransform;
    m_coneTransform->setTranslation(QVector3D(0.f, 1.0f, 0.f));
    m_coneEntity->addComponent(m_coneMesh);
    m_coneEntity->addComponent(m_material);
    m_coneEntity->addComponent(m_coneTransform);

    m_labelEntity = new Qt3DExtras::QText2DEntity(m_coneEntity);
    m_labelEntity->setText(label);
    m_labelEntity->setColor(color);
    m_labelEntity->setFont(QFont("Monospace", 1, 5));
    m_labelEntityTransform = new Qt3DCore::QTransform;
    //m_labelEntityTransform->setTranslation(QVector3D(0.2f, 0, 0));
    m_labelEntity->addComponent(m_labelEntityTransform);
    m_labelEntity->setWidth(5);
    m_labelEntity->setHeight(5);
    m_labelEntityTransform->setScale(0.07);
}


void ArrowTranslationHandle::invertTextRotation(const QMatrix4x4 &viewMatrix) {
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

void ArrowTranslationHandle::setCamera(Qt3DRender::QCamera *camera) {
    Handle::setCamera(camera);
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

void ArrowTranslationHandle::setColor(const QColor &color) {
    Handle::setColor(color);
    m_labelEntity->setColor(color);
}


void ArrowTranslationHandle::setLabel(const QString &label) {
    m_label = label;
    m_labelEntity->setText(label);
}