#ifndef TRANSPARENTOBJECTPICKER_H
#define TRANSPARENTOBJECTPICKER_H

#include "ray.h"

#include <QObject>
#include <QList>
#include <QMatrix4x4>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QComponent>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DCore/QTransform>

class TransparentObjectPicker : public Qt3DCore::QComponent {

    Q_OBJECT

public:
    explicit TransparentObjectPicker(Qt3DCore::QNode *parent = nullptr);

public slots:
    QPair<bool, QVector3D> intersectionWithRay(const Ray &ray);

private:
    void setEntity(Qt3DCore::QEntity *entity);
    QList<Qt3DRender::QGeometry*> gatherGeometries();
    QList<Qt3DRender::QGeometry*> visitNode(Qt3DCore::QNode *node);

private:
    // The entity we use to check for geometries
    Qt3DCore::QEntity *m_entity;
    QList<Qt3DRender::QGeometry*> m_geometries;
    QList<Qt3DCore::QTransform*> m_transforms;
};

#endif // TRANSPARENTOBJECTPICKER_H
