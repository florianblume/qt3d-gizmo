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
  , m_vertexBufferParameter(new Qt3DRender::QParameter)
  , m_vertexBuffer(new Qt3DRender::QBuffer)
  , m_vertexByteStrideParameter(new Qt3DRender::QParameter)
  , m_vertexOffsetParameter(new Qt3DRender::QParameter)
  , m_indexBufferParameter(new Qt3DRender::QParameter)
  , m_requestComputeParameter(new Qt3DRender::QParameter)
  , m_shaderProgram(new Qt3DRender::QShaderProgram)
  , m_effect(new Qt3DRender::QEffect)
  , m_technique(new Qt3DRender::QTechnique)
  , m_renderPass(new Qt3DRender::QRenderPass)
  , m_filterKey(new Qt3DRender::QFilterKey) {

    m_shaderProgram->setComputeShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/compute_ray.comp"))));
    m_effect->addTechnique(m_technique);
    m_technique->addRenderPass(m_renderPass);


    m_intersectionBuffer->setAccessType(Qt3DRender::QBuffer::ReadWrite);
    m_intersectionBuffer->setSyncData(true);
    QByteArray intersectionData;
    // 2 QVector3Ds -> 3 floats each a 4 bytes = 24 bytes
    intersectionData.resize(12);
    m_intersectionBuffer->setData(intersectionData);

    m_intersectionBufferParameter->setName("resultBuffer");
    m_intersectionBufferParameter->setValue(QVariant::fromValue(m_intersectionBuffer));
    m_renderPass->addParameter(m_intersectionBufferParameter);

    m_vertexBuffer->setAccessType(Qt3DRender::QBuffer::ReadWrite);
    m_vertexBuffer->setSyncData(true);
    QByteArray vertexData;
    // 2 QVector3Ds -> 3 floats each a 4 bytes = 24 bytes
    vertexData.resize(12);
    float *vertexDataPtr = reinterpret_cast<float*>(vertexData.data());
    *vertexDataPtr++ = 0;
    *vertexDataPtr++ = 5;
    *vertexDataPtr++ = 10;
    m_vertexBuffer->setData(intersectionData);

    m_vertexBufferParameter->setName("vertexBuffer");
    m_vertexBufferParameter->setValue(QVariant::fromValue(m_vertexBuffer));
    m_renderPass->addParameter(m_vertexBufferParameter);

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
    m_geometry = geometry;
    for (int i = 0; i < m_geometry->attributes().size(); i++) {
        Qt3DRender::QAttribute *attribute = m_geometry->attributes()[i];
        if (attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute ||
            attribute->attributeType() == Qt3DRender::QAttribute::VertexAttribute) {
           connect(attribute->buffer(), &Qt3DRender::QBuffer::dataChanged,
                   this, &TransparentObjectPicker::onGeometryDataChanged);
           attribute->buffer()->setSyncData(true);
           attribute->buffer()->setUsage(Qt3DRender::QBuffer::DynamicRead);
        }
    }
    //onGeometryDataChanged();
}

void TransparentObjectPicker::setMousePosition(int x, int y) {
    m_mousePosition = QPoint(x, y);
    m_mouseXParameter->setValue(x);
    m_mouseYParameter->setValue(y);
}

void TransparentObjectPicker::onGeometryDataChanged() {
    /*
    for (int i = 0; i < m_geometry->attributes().size(); i++) {
        Qt3DRender::QAttribute *attribute = m_geometry->attributes()[i];
        if (attribute->name() == Qt3DRender::QAttribute::defaultPositionAttributeName()) {
            m_verticesParameter->setValue(QVariant::fromValue(attribute->buffer()));
            m_vertexByteStrideParameter->setValue(attribute->byteStride());
            m_vertexOffsetParameter->setValue(attribute->byteOffset());

        } else if (attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute) {
            qDebug() << attribute->buffer()->data();
            qDebug() << attribute->count();
            m_indicesParameter->setValue(QVariant::fromValue(attribute->buffer()));
            m_indicesCountParameter->setValue(attribute->count());
        }
    }
    */
    Qt3DRender::QAttribute *m_positionAttribute = 0;
    Qt3DRender::QAttribute *m_indexAttribute = 0;
    for (int i = 0; i < m_geometry->attributes().size(); i++) {
        Qt3DRender::QAttribute *attribute = m_geometry->attributes()[i];
        if (attribute->name() == Qt3DRender::QAttribute::defaultPositionAttributeName()) {
            //qDebug() << attribute->buffer()->data().mid(10 * sizeof(float), sizeof(float)).toInt();
            m_vertexBufferParameter->setValue(QVariant::fromValue(attribute->buffer()));
            //m_vertexByteStrideParameter->setValue(attribute->byteStride());
            //m_vertexOffsetParameter->setValue(attribute->byteOffset());
            m_positionAttribute = attribute;

        } else if (attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute) {
            //qDebug() << attribute->buffer()->data();
            //qDebug() << attribute->count();
            m_indexAttribute = attribute;
            //m_indexBufferParameter->setValue(QVariant::fromValue(attribute->buffer()));
        }

        if (m_positionAttribute && m_indexAttribute) {
            int byteOffsetPos = m_positionAttribute->byteOffset();
            int byteStridePos = m_positionAttribute->byteStride();
            Qt3DRender::QBuffer *vertexBuffer = m_positionAttribute->buffer();
            QByteArray data = vertexBuffer->data();
            float *floatData = reinterpret_cast<float*>(data.data());
            for (int i = 0; i < m_indexAttribute->count(); i += 3) {

                // Get x, y, z positions for 1st vertex
                float pos0x = floatData[i + 0 * sizeof(float) + byteOffsetPos];
                float pos0y = floatData[i + 1 * sizeof(float) + byteOffsetPos];
                float pos0z = floatData[i + 2 * sizeof(float) + byteOffsetPos];

                // Get x, y, z positions for 2nd vertex
                float pos1x = floatData[i + 1 * byteStridePos + 0 * sizeof(float) + byteOffsetPos];
                float pos1y = floatData[i + 1 * byteStridePos + 1 * sizeof(float) + byteOffsetPos];
                float pos1z = floatData[i + 1 * byteStridePos + 2 * sizeof(float) + byteOffsetPos];

                float pos2x = floatData[i + 2 * byteStridePos + 0 * sizeof(float) + byteOffsetPos];
                float pos2y = floatData[i + 2 * byteStridePos + 1 * sizeof(float) + byteOffsetPos];
                float pos2z = floatData[i + 2 * byteStridePos + 2 * sizeof(float) + byteOffsetPos];

                //if (pos0x > 0 && pos0y > 0)
                if (i == 0) {
                    //qDebug() << pos0x << pos0y << pos0z;
                    //qDebug() << pos1x << pos1y << pos1z;
                    //qDebug() << pos2x << pos2y << pos2z;
                }

                //qDebug() << floatPos0x << floatPos0y << floatPos0z;

                //qDebug() << QVector3D(pos0x.toInt(), pos0y.toInt(), pos0z.toInt());
                //qDebug() << QVector3D(pos1x.toInt(), pos1y.toInt(), pos1z.toInt());
                //qDebug() << QVector3D(pos2x.toInt(), pos2y.toInt(), pos2z.toInt());
            }
        }
    }
}

void TransparentObjectPicker::onIntersectionBufferDataChanged() {
    float *intersectionPtr = reinterpret_cast<float*>(m_intersectionBuffer->data().data());

    QVector3D worldIntersection(intersectionPtr[0], intersectionPtr[1], intersectionPtr[2]);
    // Skip one index because arrays are 0-terminated
    //QVector3D localIntersection(intersectionPtr[3], intersectionPtr[4], intersectionPtr[5]);

    // For testing
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch()
    );
    qDebug() << worldIntersection;
    //emit intersected(worldIntersection, localIntersection, m_mousePosition);
}
