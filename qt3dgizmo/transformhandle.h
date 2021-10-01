#ifndef TRANSFORMHANDLE_H
#define TRANSFORMHANDLE_H

#include "handle.h"

class TransformHandle : public Handle {

public:
    TransformHandle(Qt3DCore::QNode *parent,
                    AxisConstraint constraint,
                    const QVector3D &position,
                    const QColor &color);
    AxisConstraint axisConstraint();

protected:
    AxisConstraint m_axisConstraint;
};

#endif // TRANSFORMHANDLE_H
