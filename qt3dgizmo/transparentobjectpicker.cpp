#include "transparentobjectpicker.h"
#include "ray.h"

#include <Qt3DRender/QBuffer>
#include <Qt3DInput/QMouseEvent>

TransparentObjectPicker::TransparentObjectPicker(Qt3DCore::QNode *parent): Qt3DCore::QComponent(parent) {
    connect(this, &Qt3DCore::QComponent::addedToEntity,
            this, &TransparentObjectPicker::setEntity);
}

QPair<bool, QVector3D> TransparentObjectPicker::intersectionWithRay(const Ray &ray) {
    QList<Qt3DRender::QGeometry*> geometries = gatherGeometries();
    for (int i = 0; i < geometries.size(); i++) {
        Qt3DRender::QGeometry *geometry = geometries[i];
        if (ray.intersectsBoundingBox(geometry->minExtent(), geometry->maxExtent())) {
            return QPair<bool, QVector3D>(true, QVector3D());
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
    }
    return QPair<bool, QVector3D>(false, QVector3D());
}

void TransparentObjectPicker::setEntity(Qt3DCore::QEntity *entity) {
    m_entity = entity;
}

QList<Qt3DRender::QGeometry*> TransparentObjectPicker::gatherGeometries() {
    return visitNode(m_entity);
}

QList<Qt3DRender::QGeometry *> TransparentObjectPicker::visitNode(Qt3DCore::QNode *node) {
    // We can skip node in the very first call (i.e. we do not need to check if node
    // is a geometry) since the first node is always a QEntity
    QList<Qt3DRender::QGeometry*> geometries;
    Qt3DCore::QNodeVector nodeVector = node->childNodes();
    for (int i = 0; i < nodeVector.size(); i++) {
        Qt3DCore::QNode *childNode = nodeVector[i];
        auto* derived = dynamic_cast<Qt3DRender::QGeometry*>(childNode);
        if (derived) {
            geometries.append(derived);
        } else {
            geometries.append(visitNode(childNode));
        }
    }
    return geometries;
}
