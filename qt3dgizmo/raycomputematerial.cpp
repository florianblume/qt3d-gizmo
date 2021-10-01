#include "raycomputematerial.h"

#include <QUrl>
#include <QVector>
#include <QVector3D>
#include <Qt3DRender/QGraphicsApiFilter>

RayComputeMaterial::RayComputeMaterial()
    : m_mouseXParameter(new Qt3DRender::QParameter)
    , m_mouseYParameter(new Qt3DRender::QParameter)
    , m_rayBufferParameter(new Qt3DRender::QParameter)
    , m_shaderProgram(new Qt3DRender::QShaderProgram)
    , m_effect(new Qt3DRender::QEffect)
    , m_technique(new Qt3DRender::QTechnique)
    , m_renderPass(new Qt3DRender::QRenderPass)
    , m_filterKey(new Qt3DRender::QFilterKey)
    , m_rayBuffer(new Qt3DRender::QBuffer) {

    m_shaderProgram->setComputeShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/compute_ray.vert"))));
    m_effect->addTechnique(m_technique);
    m_technique->addRenderPass(m_renderPass);

    m_mouseXParameter->setName("mouseX");
    m_mouseXParameter->setValue(0);
    m_renderPass->addParameter(m_mouseXParameter);
    m_mouseYParameter->setName("mouseY");
    m_mouseYParameter->setValue(0);
    m_renderPass->addParameter(m_mouseYParameter);

    m_rayBuffer->setAccessType(Qt3DRender::QBuffer::ReadWrite);
    m_rayBuffer->setObjectName("buffer");
    QByteArray verticesData;
    // 2 QVector3Ds -> 3 floats each a 4 bytes = 24 bytes
    // but arrays are 0-terminated, i.e. we need 4 bytes
    // more and skipp the one index -> 28 bytes
    verticesData.resize(28);
    m_rayBuffer->setData(verticesData);

    m_rayBufferParameter->setName("rays");
    m_rayBufferParameter->setValue(QVariant::fromValue(m_rayBuffer));
    m_renderPass->addParameter(m_rayBufferParameter);

    m_renderPass->setShaderProgram(m_shaderProgram);

    // Maybe add later
    //m_technique->addFilterKey(m_filterKey);
    m_filterKey->setName("renderingStyle");
    m_filterKey->setValue("compute");
    m_technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    m_technique->graphicsApiFilter()->setMajorVersion(4);
    m_technique->graphicsApiFilter()->setMinorVersion(3);
    setEffect(m_effect);

    connect(m_rayBuffer, &Qt3DRender::QBuffer::dataChanged,
            this, &RayComputeMaterial::bufferDataChanged);
}

void RayComputeMaterial::setMouseCoordinates(int x, int y) {
    m_mouseXParameter->setValue(x);
    m_mouseYParameter->setValue(y);
}

void RayComputeMaterial::bufferDataChanged() {
    float *verticesPtr = reinterpret_cast<float*>(m_rayBuffer->data().data());
    QVector3D start(verticesPtr[0], verticesPtr[1], verticesPtr[2]);
    // Skip one index because arrays are 0-terminated
    QVector3D end(verticesPtr[4], verticesPtr[5], verticesPtr[6]);
    qDebug() << start << end;
    // TODO need to also get the camera position to pass it to the handles
    // so that they can compute the scaling factor and apply it before
    // compute intersections
}
