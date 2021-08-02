#ifndef PLANETRANSLATIONHANDLE_H
#define PLANETRANSLATIONHANDLE_H

#include "handle.h"
#include <QObject>

#include <Qt3DExtras/QPlaneMesh>

class PlaneTranslationHandle : public Handle {

    Q_OBJECT

public:
    PlaneTranslationHandle(Qt3DCore::QNode *parent, const QVector3D &position, const QColor &color);

private:
    Qt3DCore::QEntity *m_planeEntity;
    Qt3DCore::QTransform *m_planeTransform;
    Qt3DExtras::QPlaneMesh *m_planeMesh;
};

#endif // PLANETRANSLATIONHANDLE_H
