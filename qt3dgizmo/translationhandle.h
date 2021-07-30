#ifndef TRANSLATIONHANDLE_H
#define TRANSLATIONHANDLE_H

#include "handle.h"

#include <QObject>

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QPhongAlphaMaterial>

class TranslationHandle : public Handle {

    Q_OBJECT

public:

enum Type {
    Arrow,
    Plane
};

Q_ENUM(Type)

public:
    TranslationHandle(Type type, const QVector3D &position, const QString &label, const QColor &color);

Q_SIGNALS:
    void pressed(const QVector3D &position);

private Q_SLOTS:
    void onPressed(Qt3DRender::QPickEvent *event) override;
    void onMoved(Qt3DRender::QPickEvent *event) override;

private:
    Type m_type;
    Qt3DExtras::QCylinderMesh *m_cylinderMesh;
    Qt3DExtras::QConeMesh *m_coneMesh;
    Qt3DExtras::QPhongAlphaMaterial *m_material;
};

#endif // TRANSLATIONHANDLE_H
