#ifndef ROTATIONHANDLE_H
#define ROTATIONHANDLE_H

#include "handle.h"

#include <QObject>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QPhongAlphaMaterial>

class RotationHandle : public Handle {

    Q_OBJECT

public:
    RotationHandle(Qt3DCore::QNode *parent, const QVector3D &position, const QString &label, const QColor &color);

Q_SIGNALS:
    void pressed(const QVector3D &position);

private:
    Qt3DExtras::QTorusMesh *m_torusMesh;
    Qt3DExtras::QPhongAlphaMaterial *m_material;
};

#endif // ROTATIONHANDLE_H
