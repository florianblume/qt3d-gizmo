#ifndef RAY_H
#define RAY_H

#include "plane.h"
#include "constants.h"

#include <QVector3D>
#include <QMatrix4x4>

class Ray {

public:
    static Ray generate3DRayFromScreenToInfinity(int x, int y,
                                                 const QSize &windowSize,
                                                 const QMatrix4x4 &viewMatrix,
                                                 const QMatrix4x4 &projectionMatrix) {
        float winZ0 = 0.0f;
        float winZ1 = 1.0f;
        int yCorrected = windowSize.height() - y;

        QVector3D origin = QVector3D(x, yCorrected, winZ0);
        QVector3D destination = QVector3D(x, yCorrected, winZ1);

        // TODO is using window size correct here?
        // TODO use viewport dimensions here
        QVector3D unprojectedOrigin = origin.unproject(viewMatrix,
                                                       projectionMatrix,
                                                       QRect(0, 0, windowSize.width(), windowSize.height()));
        QVector3D unprojectedDestination = destination.unproject(viewMatrix,
                                                                 projectionMatrix,
                                                                 QRect(0, 0, windowSize.width(), windowSize.height()));

        return Ray(unprojectedOrigin, unprojectedDestination);
    }

    inline Ray() {
    }

    inline Ray(const QVector3D &start, const QVector3D &end)
        : start(start), end(end) {
    }

    inline QPair<int, QVector3D> intersectsPlane(const Plane &plane) const {
        int result = 0;

        QVector3D intersectPoint;

        QVector3D u = start - end;
        QVector3D w = start - plane.position;

        float     D = QVector3D::dotProduct(plane.normal, u);
        float     N = -QVector3D::dotProduct(plane.normal, w);

        if (qAbs(D) < EPSILON)		   // segment is parallel to plane
        {
            if (N == 0)                // segment lies in plane
                result = 2;
            else
                result = 0;            // no intersection
        }

        float s = N / D;
        if (s < 0 || s > 1)
            result = 0;                 // no intersection

        intersectPoint = start + u * s; // compute segment intersect point

        result = 1;
        return qMakePair(result, intersectPoint);
    }

    inline bool intersectsBoundingBox(const QVector3D &minExtent, const QVector3D &maxExtent) const {
        // Adapted from https://gamedev.stackexchange.com/a/18459

        // unit direction vector of ray
        QVector3D unitDirection = QVector3D(1.0f, 1.0f, 1.0f) / (end - start);
        QVector3D first = (minExtent - start) * unitDirection;
        QVector3D second = (maxExtent - start) * unitDirection;
        float t1 = first.x();
        float t2 = second.x();
        float t3 = first.y();
        float t4 = second.y();
        float t5 = first.z();
        float t6 = second.z();

        float tmin = qMax(qMax(qMin(t1, t2), qMin(t3, t4)), qMin(t5, t6));
        float tmax = qMin(qMin(qMax(t1, t2), qMax(t3, t4)), qMax(t5, t6));

        // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
        if (tmax < 0) {
            return false;
        }

        // if tmin > tmax, ray doesn't intersect AABB
        if (tmin > tmax) {
            return false;
        }

        return true;
    }

    inline QPair<bool, QVector3D>  intersectsTriangle(const QVector3D &a,
                                                      const QVector3D &b,
                                                      const QVector3D &c) const {
        // Qt3D's internal triangle intersection procedure
        const QVector3D ab = b - a;
        const QVector3D ac = c - a;
        const QVector3D qp = (start - end);

        const QVector3D n = QVector3D::crossProduct(ab, ac);
        const float d = QVector3D::dotProduct(qp, n);

        if (d <= 0.0f)
            return qMakePair(false, QVector3D());

        const QVector3D ap = start - a;
        float t = QVector3D::dotProduct(ap, n);

        if (t < 0.0f || t > d)
            return qMakePair(false, QVector3D());

        const QVector3D e = QVector3D::crossProduct(qp, ap);
        QVector3D uvw;
        uvw.setY(QVector3D::dotProduct(ac, e));

        if (uvw.y() < 0.0f || uvw.y() > d)
            return qMakePair(false, QVector3D());

        uvw.setZ(-QVector3D::dotProduct(ab, e));

        if (uvw.z() < 0.0f || uvw.y() + uvw.z() > d)
            return qMakePair(false, QVector3D());

        const float ood = 1.0f / d;
        t *= ood;
        uvw.setY(uvw.y() * ood);
        uvw.setZ(uvw.z() * ood);
        uvw.setX(1.0f - uvw.y() - uvw.z());

        QVector3D intersection = start + t * (end - start);

        // Return global intersection
        return qMakePair(true, intersection);
    }

    QVector3D start;
    QVector3D end;
};

#endif // RAY_H
