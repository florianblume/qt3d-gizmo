#ifndef ROTATIONHANDLE_H
#define ROTATIONHANDLE_H

#include "handle.h"

#include <QObject>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QPhongMaterial>

class RotationHandle : public Handle {

    Q_OBJECT

public:
    RotationHandle(Qt3DCore::QNode *parent, AxisConstraint constraint, const QVector3D &position, const QColor &color);

private:
    Qt3DExtras::QTorusMesh *m_torusMesh;
};

#endif // ROTATIONHANDLE_H
