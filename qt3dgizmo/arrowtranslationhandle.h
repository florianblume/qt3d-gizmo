#ifndef ARROWTRANSLATIONHANDLE_H
#define ARROWTRANSLATIONHANDLE_H

#include "transformhandle.h"

#include <QObject>

#include <Qt3DExtras/QText2DEntity>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QConeMesh>

class ArrowTranslationHandle : public TransformHandle {

    Q_OBJECT

public:
    ArrowTranslationHandle(Qt3DCore::QNode *parent,
                           AxisConstraint constraint,
                           const QVector3D &position,
                           const QColor &color);

public slots:
    void setColor(const QColor &color);
    void setEnabled(bool enabled) override;

protected:
    void handleAppearanceChange() override;

private:
    Qt3DCore::QEntity *m_cylinderEntity;
    Qt3DCore::QTransform *m_cylinderTransform;
    Qt3DExtras::QCylinderMesh *m_cylinderMesh;

    Qt3DCore::QEntity *m_coneEntity;
    Qt3DCore::QTransform *m_coneTransform;
    Qt3DExtras::QConeMesh *m_coneMesh;

    QObject *m_cameraConnectionContext = nullptr;
};

#endif // ARROWTRANSLATIONHANDLE_H
