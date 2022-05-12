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
  , m_isIntersectedBufferParameter(new Qt3DRender::QParameter)
  , m_isIntersectedBuffer(new Qt3DRender::QBuffer)
  , m_vertexBufferParameter(new Qt3DRender::QParameter)
  , m_vertexBuffer(new Qt3DRender::QBuffer)
  , m_vertexByteStrideParameter(new Qt3DRender::QParameter)
  , m_vertexByteOffsetParameter(new Qt3DRender::QParameter)
  , m_indexBufferParameter(new Qt3DRender::QParameter)
  , m_indexBuffer(new Qt3DRender::QBuffer)
  , m_requestComputeParameter(new Qt3DRender::QParameter)
  , m_shaderProgram(new Qt3DRender::QShaderProgram)
  , m_effect(new Qt3DRender::QEffect)
  , m_technique(new Qt3DRender::QTechnique)
  , m_renderPass(new Qt3DRender::QRenderPass)
  , m_filterKey(new Qt3DRender::QFilterKey) {

    m_shaderProgram->setComputeShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/compute_ray.comp"))));
    m_effect->addTechnique(m_technique);
    m_technique->addRenderPass(m_renderPass);

    m_mouseXParameter->setName("mouseX");
    m_mouseXParameter->setValue(-1);
    m_renderPass->addParameter(m_mouseXParameter);

    m_mouseYParameter->setName("mouseY");
    m_mouseYParameter->setValue(-1);
    m_renderPass->addParameter(m_mouseYParameter);

    m_intersectionBuffer->setAccessType(Qt3DRender::QBuffer::ReadWrite);
    m_intersectionBuffer->setSyncData(true);
    QByteArray intersectionData;
    // 2 QVector3Ds -> 3 floats each 4 bytes = 24 bytes
    intersectionData.resize(64);
    m_intersectionBuffer->setData(intersectionData);

    m_intersectionBufferParameter->setName("intersectionBuffer");
    m_intersectionBufferParameter->setValue(QVariant::fromValue(m_intersectionBuffer));
    m_renderPass->addParameter(m_intersectionBufferParameter);

    m_vertexBufferParameter->setName("vertexBuffer");
    m_vertexBufferParameter->setValue(QVariant(0));
    m_renderPass->addParameter(m_vertexBufferParameter);

    m_vertexByteStrideParameter->setName("vertexByteStride");
    m_vertexByteStrideParameter->setValue(QVariant(0));
    m_renderPass->addParameter(m_vertexByteStrideParameter);

    m_vertexByteOffsetParameter->setName("vertexByteOffset");
    m_vertexByteOffsetParameter->setValue(QVariant(0));
    m_renderPass->addParameter(m_vertexByteOffsetParameter);

    m_indexBufferParameter->setName("indexBuffer");
    m_indexBufferParameter->setValue(QVariant(0));
    m_renderPass->addParameter(m_indexBufferParameter);

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

    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < m_geometry->attributes().size(); i++) {
        Qt3DRender::QAttribute *attribute = m_geometry->attributes()[i];
        if (attribute->name() == Qt3DRender::QAttribute::defaultPositionAttributeName()) {
            //qDebug() << attribute->buffer()->data().mid(10 * sizeof(float), sizeof(float)).toInt();
            m_vertexBufferParameter->setValue(QVariant::fromValue(attribute->buffer()));
            m_vertexByteStrideParameter->setValue(attribute->byteStride());
            m_vertexByteOffsetParameter->setValue(attribute->byteOffset());
            m_positionAttribute = attribute;

        } else if (attribute->attributeType() == Qt3DRender::QAttribute::IndexAttribute) {
            //qDebug() << attribute->buffer()->data();
            //qDebug() << attribute->count();
            m_indexAttribute = attribute;
            //indexByteArray.resize(attribute->count());
            //m_indexBufferParameter->setValue(QVariant::fromValue(attribute->buffer()));
        }
    }

    int indexData[m_indexAttribute->count()];
    if (m_positionAttribute && m_indexAttribute) {
        int byteOffsetPos = m_positionAttribute->byteOffset();
        int byteStridePos = m_positionAttribute->byteStride();
        Qt3DRender::QBuffer *vertexBuffer = m_positionAttribute->buffer();
        QByteArray data = vertexBuffer->data();
        float *floatData = reinterpret_cast<float*>(data.data());
        quint16 *intData = reinterpret_cast<quint16*>(m_indexAttribute->buffer()->data().data());
        int *intData2 = reinterpret_cast<int*>(m_indexAttribute->buffer()->data().data());
        for (int i = 0; i < m_indexAttribute->count(); i += 3) {


            indexData[i]     = intData[i];
            indexData[i + 1] = intData[i + 1];
            indexData[i + 2] = intData[i + 2];

            //qDebug() << (int) intData2[0] << (int) intData2[1] << (int) intData2[2];
            //break;
        }
    }

    char *carray = (char *) &indexData;
    m_indexBuffer->setData(QByteArray(carray, m_indexAttribute->count()));
    m_indexBufferParameter->setValue(QVariant::fromValue(m_indexBuffer));
}

void TransparentObjectPicker::onIntersectionBufferDataChanged(const QByteArray &bytes) {
    const float *intersectionPtr = reinterpret_cast<const float*>(bytes.data());
    for (int i = 0; i < 4; i++) {
        QString line = "";
        for (int j = 0; j < 4; j++) {
            line += QString::number(intersectionPtr[i * 4 + j]) + " ";
        }
        qDebug() << line;
    }

    // For testing
    using namespace std::chrono;
    milliseconds ms = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch()
    );
    //qDebug() << ms.count();
    //emit intersected(worldIntersection, localIntersection, m_mousePosition);
}
