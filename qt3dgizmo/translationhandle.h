#ifndef TRANSLATIONHANDLE_H
#define TRANSLATIONHANDLE_H

#include "handle.h"

#include <QObject>

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QConeMesh>

class TranslationHandle : public Handle {

    Q_OBJECT

public:

enum Type {
    Arrow,
    Plane
};

Q_ENUM(Type)

public:
    TranslationHandle(Qt3DCore::QNode *parent, Type type,
                      const QVector3D &position, const QString &label,
                      const QColor &color);

private:
    Type m_type;
    Qt3DExtras::QCylinderMesh *m_cylinderMesh;
    Qt3DExtras::QConeMesh *m_coneMesh;
};

#endif // TRANSLATIONHANDLE_H
