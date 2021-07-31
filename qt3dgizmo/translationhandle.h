#ifndef TRANSLATIONHANDLE_H
#define TRANSLATIONHANDLE_H

#include "handle.h"

#include <QObject>

#include <Qt3DRender/QCamera>
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
    TranslationHandle(Qt3DCore::QNode *parent, Type type,
                      const QVector3D &position, const QString &label,
                      const QColor &color);

Q_SIGNALS:
    void pressed(const QVector3D &position);

public Q_SLOTS:
    void invertTextRotation(const QMatrix4x4 &viewMatrix) override;
    void setCamera(Qt3DRender::QCamera *camera);

private Q_SLOTS:
    void onPressed(Qt3DRender::QPickEvent *event) override;
    void onMoved(Qt3DRender::QPickEvent *event) override;

private:
    Type m_type;
    QObject *m_cameraConnectionContext = Q_NULLPTR;
    Qt3DExtras::QCylinderMesh *m_cylinderMesh;
    Qt3DExtras::QConeMesh *m_coneMesh;
    Qt3DExtras::QPhongAlphaMaterial *m_material;
};

#endif // TRANSLATIONHANDLE_H
