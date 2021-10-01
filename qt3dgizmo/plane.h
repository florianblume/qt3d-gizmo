#ifndef PLANE_H
#define PLANE_H

#include <QVector3D>

class Plane {

public:
    Plane() {
    }

    Plane(const QVector3D &position, const QVector3D &normal) {
        this->position = position;
        this->normal = normal;
    }

    // TODO ATTENTION! This is OpenGL normal with y being up and down
    QVector3D normal = QVector3D(0.f, 1.f, 0.f);
    QVector3D position;
};

#endif // PLANE_H
