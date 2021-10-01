#include "transparentobjectpicker.h"
#include "ray.h"

#include <Qt3DRender/QBuffer>
#include <Qt3DInput/QMouseEvent>

TransparentObjectPicker::TransparentObjectPicker(Qt3DCore::QNode *parent)
    : Qt3DInput::QMouseHandler(parent)
    , m_mouseDevice(new Qt3DInput::QMouseDevice) {
    m_mouseDevice->setParent(this);
    setSourceDevice(m_mouseDevice);
    connect(this, &Qt3DCore::QComponent::addedToEntity,
            this, &TransparentObjectPicker::setEntity);
    // TODO this doesn't work since exited is called when the mouse leaves the widget
    connect(this, &Qt3DInput::QMouseHandler::exited,
            this, &TransparentObjectPicker::onExited);
    connect(this, &Qt3DInput::QMouseHandler::pressed,
            this, &TransparentObjectPicker::onPressed);
    connect(this, &Qt3DInput::QMouseHandler::positionChanged,
            this, &TransparentObjectPicker::onPositionChanged);
}

void TransparentObjectPicker::setViewMatrix(const QMatrix4x4 &viewMatrix) {

}

void TransparentObjectPicker::setProjectionMatrix(const QMatrix4x4 &projectionMatrix) {

}

void TransparentObjectPicker::setViewport(const QRect &viewport) {

}

void TransparentObjectPicker::setWindowSize(const QSize &windowSize) {

}

void TransparentObjectPicker::setEntity(Qt3DCore::QEntity *entity) {
    m_entity = entity;
}

void TransparentObjectPicker::onExited() {
    emit exited();
}

void TransparentObjectPicker::onPressed(Qt3DInput::QMouseEvent *mouse) {
    // TODO:
    // 1. Check for variable that stores if mouse is over the object
    // 2. If true, emit pressed event with event object
    if (m_mouseOnObject) {
        m_pickEvent.reset(new Qt3DRender::QPickEvent(QPointF(mouse->x(), mouse->y()),
                                                     m_worldIntersection,
                                                     m_localIntersection,
                                                     m_distance));
        emit pressed(m_pickEvent.data());
    }
}

void TransparentObjectPicker::onPositionChanged(Qt3DInput::QMouseEvent *mouse) {
    // TODO: Get camera, viewport, surface and target from nodes grahp
    Ray ray = Ray::generate3DRayFromScreenToInfinity(mouse->x(), mouse->y(),
                                                     m_windowSize,
                                                     m_viewMatrix,
                                                     m_projectionMatrix);
    QList<Qt3DRender::QGeometry*> geometries = gatherGeometries();
    for (int i = 0; i < geometries.size(); i++) {
        Qt3DRender::QGeometry *geometry = geometries[i];
        if (ray.intersectsBoundingBox(geometry->minExtent(), geometry->maxExtent())) {
            int byteOffsetPos = 0;//m_positionAttribute->byteOffset();
            int byteStridePos = 0;//m_positionAttribute->byteStride();
            Qt3DRender::QBuffer *vertexBuffer = 0;//m_positionAttribute->buffer();
            int numberOfTriangles = 0;//m_indexAttribute->count() / 3;
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
                    m_mouseOnObject = true;
                    // We do not need distance
                    m_distance = 0;
                    m_worldIntersection = intersection.second;
                    // We do not need localIntersection
                    m_localIntersection = QVector3D();
                    m_pickEvent.reset(new Qt3DRender::QPickEvent(QPointF(mouse->x(), mouse->y()),
                                                                 m_worldIntersection,
                                                                 m_localIntersection,
                                                                 m_distance));
                    emit moved(m_pickEvent.data());
                } else if (m_mouseOnObject) {
                    // Mouse was on object the last call to positionChanged
                    m_mouseOnObject = false;
                    emit exited();
                }
            }
        }
    }
}

QList<Qt3DRender::QGeometry*> TransparentObjectPicker::gatherGeometries() {
    return QList<Qt3DRender::QGeometry*>();
}
