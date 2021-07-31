#ifndef ROTATIONHANDLE_H
#define ROTATIONHANDLE_H

#include "handle.h"

#include <QObject>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QPhongMaterial>

class RotationHandle : public Handle {

    Q_OBJECT

public:
    RotationHandle(Qt3DCore::QNode *parent, const QVector3D &position, const QString &label, const QColor &color);

private:
    Qt3DExtras::QTorusMesh *m_torusMesh;
    Qt3DExtras::QPhongMaterial *m_material;
};

#endif // ROTATIONHANDLE_H
