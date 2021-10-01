#ifndef MODESWITCHERHANDLE_H
#define MODESWITCHERHANDLE_H

#include "handle.h"

#include <QObject>
#include <Qt3DCore/QEntity>
#include <Qt3DExtras/QSphereMesh>

class ModeSwitcherHandle : public Handle {

public:
    ModeSwitcherHandle(Qt3DCore::QNode *parent, const QVector3D &position, const QColor &color);

protected:
    void handleAppearanceChange() override;

private:
    Qt3DCore::QEntity *m_sphereEntity;
    Qt3DExtras::QSphereMesh *m_sphereMesh;
    QColor m_defaultColor = QColor(50, 50, 50, 50);
};

#endif // MODESWITCHERHANDLE_H
