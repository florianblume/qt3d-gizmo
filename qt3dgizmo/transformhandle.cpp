#include "transformhandle.h"

TransformHandle::TransformHandle(Qt3DCore::QNode *parent,
                                 AxisConstraint constraint,
                                 const QVector3D &position,
                                 const QColor &color)
    : Handle(parent, position, color)
    , m_axisConstraint(constraint) {

}

Handle::AxisConstraint TransformHandle::axisConstraint() {
    return m_axisConstraint;
}
