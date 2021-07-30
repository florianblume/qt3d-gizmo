#include "rotationhandle.h"

RotationHandle::RotationHandle(const QVector3D &position, const QString &label, const QColor &color)
    : Handle(position, label, color) {

}

void RotationHandle::onPressed(Qt3DRender::QPickEvent *event) {

}

void RotationHandle::onMoved(Qt3DRender::QPickEvent *event) {

}
