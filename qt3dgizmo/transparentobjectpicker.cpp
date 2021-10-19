#include "transparentobjectpicker.h"
#include "ray.h"

#include <Qt3DRender/QBuffer>
#include <Qt3DInput/QMouseEvent>
#include <Qt3DRender/QGraphicsApiFilter>

TransparentObjectPicker::TransparentObjectPicker(Qt3DCore::QNode *parent)
  : Qt3DRender::QMaterial(parent)
  , m_mouseXParameter(new Qt3DRender::QParameter)
  , m_mouseYParameter(new Qt3DRender::QParameter)
  , m_intersectionBufferParameter(new Qt3DRender::QParameter)
  , m_intersectionBuffer(new Qt3DRender::QBuffer)
  , m_verticesParameter(new Qt3DRender::QParameter)
  , m_vertexByteStrideParameter(new Qt3DRender::QParameter)
  , m_vertexOffsetParameter(new Qt3DRender::QParameter)
  , m_indicesParameter(new Qt3DRender::QParameter)
  , m_requestComputeParameter(new Qt3DRender::QParameter)
  , m_shaderProgram(new Qt3DRender::QShaderProgram)
  , m_effect(new Qt3DRender::QEffect)
  , m_technique(new Qt3DRender::QTechnique)
  , m_renderPass(new Qt3DRender::QRenderPass)
  , m_filterKey(new Qt3DRender::QFilterKey) {

    m_shaderProgram->setComputeShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/intersect_ray.comp"))));
    m_effect->addTechnique(m_technique);
    m_technique->addRenderPass(m_renderPass);

    m_mouseXParameter->setName("mouseX");
    m_mouseXParameter->setValue(0);
    m_renderPass->addParameter(m_mouseXParameter);
    m_mouseYParameter->setName("mouseY");
    m_mouseYParameter->setValue(0);
    m_renderPass->addParameter(m_mouseYParameter);

    m_intersectionBuffer->setAccessType(Qt3DRender::QBuffer::ReadWrite);
    m_intersectionBuffer->setSyncData(true);
    m_intersectionBuffer->setObjectName("intersectionBuffer");
    QByteArray intersectionData;
    // 2 QVector3Ds -> 3 floats each a 4 bytes = 24 bytes
    intersectionData.resize(24);
    m_intersectionBuffer->setData(intersectionData);

    m_intersectionBufferParameter->setName("result");
    m_intersectionBufferParameter->setValue(QVariant::fromValue(m_intersectionBuffer));
    m_renderPass->addParameter(m_intersectionBufferParameter);

    m_requestComputeParameter->setName("computeRequested");
    m_requestComputeParameter->setValue(QVariant(false));
    m_renderPass->addParameter(m_requestComputeParameter);

    m_verticesParameter->setName("vertices");
    m_renderPass->addParameter(m_verticesParameter);

    m_vertexByteStrideParameter->setName("vertexByteStride");
    m_renderPass->addParameter(m_vertexByteStrideParameter);

    m_vertexOffsetParameter->setName("vertexOffset");
    m_renderPass->addParameter(m_vertexOffsetParameter);

    m_indicesParameter->setName("indices");
    m_renderPass->addParameter(m_indicesParameter);

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

    connect(m_intersectionBuffer, &Qt3DRender::QBuffer::dataChanged,
            this, &TransparentObjectPicker::onIntersectionBufferDataChanged);
}

void TransparentObjectPicker::setGeometry(Qt3DRender::QGeometry *geometry) {
    for (int i = 0; i < geometry->attributes().size(); i++) {
        Qt3DRender::QAttribute *attribute = geometry->attributes()[i];
        if (attribute->name() == Qt3DRender::QAttribute::defaultPositionAttributeName()) {
            m_verticesParameter->setValue(attribute->buffer()->data());
            m_vertexByteStrideParameter->setValue(attribute->byteStride());
            m_vertexOffsetParameter->setValue(attribute->byteOffset());

        } else if (attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute) {
            m_indicesParameter->setValue(attribute->buffer()->data());
        }
    }
    /*
    int byteOffsetPos = m_positionAttribute->byteOffset();
    int byteStridePos = m_positionAttribute->byteStride();
    Qt3DRender::QBuffer *vertexBuffer = m_positionAttribute->buffer();
    int numberOfTriangles = m_indexAttribute->count() / 3;
    QByteArray data = vertexBuffer->data();
    for (int i = 0; i < numberOfTriangles; i++) {
        int idxPos = 0;

        // Get x, y, z positions for 1st vertex
        QByteArray pos0x = data.mid(idxPos + 0 * sizeof(float), sizeof(float));
        QByteArray pos0y = data.mid(idxPos + 1 * sizeof(float), sizeof(float));
        QByteArray pos0z = data.mid(idxPos + 2 * sizeof(float), sizeof(float));

        // Get x, y, z positions for 2nd vertex
        QByteArray pos1x = data.mid(idxPos + 1 * byteStridePos + 0 * sizeof(float), sizeof(float));
        QByteArray pos1y = data.mid(idxPos + 1 * byteStridePos + 1 * sizeof(float), sizeof(float));
        QByteArray pos1z = data.mid(idxPos + 1 * byteStridePos + 2 * sizeof(float), sizeof(float));

        QByteArray pos2x = data.mid(idxPos + 2 * byteStridePos + 0 * sizeof(float), sizeof(float));
        QByteArray pos2y = data.mid(idxPos + 2 * byteStridePos + 1 * sizeof(float), sizeof(float));
        QByteArray pos2z = data.mid(idxPos + 2 * byteStridePos + 2 * sizeof(float), sizeof(float));

        QPair<bool, QVector3D> intersection = ray.intersectsTriangle(QVector3D(pos0x.toInt(), pos0y.toInt(), pos0z.toInt()),
                                                                     QVector3D(pos1x.toInt(), pos1y.toInt(), pos1z.toInt()),
                                                                     QVector3D(pos2x.toInt(), pos2y.toInt(), pos2z.toInt()));
        if (intersection.first) {
            return QPair<bool, QVector3D>(true, intersection.second);
        }
    }
    */
}

void TransparentObjectPicker::setMousePosition(int x, int y) {
    m_mousePosition = QPoint(x, y);
    m_mouseXParameter->setValue(x);
    m_mouseYParameter->setValue(y);
}

void TransparentObjectPicker::onIntersectionBufferDataChanged() {
    float *intersectionPtr = reinterpret_cast<float*>(m_intersectionBuffer->data().data());
    QVector3D worldIntersection(intersectionPtr[0], intersectionPtr[1], intersectionPtr[2]);
    // Skip one index because arrays are 0-terminated
    QVector3D localIntersection(intersectionPtr[3], intersectionPtr[4], intersectionPtr[5]);

    // For testing
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch()
    );
   //qDebug() << start << end << ms.count();
    emit intersected(worldIntersection, localIntersection, m_mousePosition);
}
