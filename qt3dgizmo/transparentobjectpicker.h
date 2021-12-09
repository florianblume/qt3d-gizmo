#ifndef TRANSPARENTOBJECTPICKER_H
#define TRANSPARENTOBJECTPICKER_H

#include "ray.h"

#include <QObject>
#include <QList>
#include <QMatrix4x4>
#include <QMutex>
#include <QMutexLocker>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QComponent>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QAttribute>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QBuffer>
#include <QMutex>
#include <QMutexLocker>

class TransparentObjectPicker : public Qt3DRender::QMaterial {

    Q_OBJECT

public:
    explicit TransparentObjectPicker(Qt3DCore::QNode *parent = nullptr);

public slots:
    void setGeometry(Qt3DRender::QGeometry *geometry);
    void setMousePosition(int x, int y);

signals:
    void intersected(const QVector3D &worldIntersection,
                     const QVector3D &localIntersection,
                     const QPoint &mousePosition);

private slots:
    void onGeometryDataChanged();
    void onIntersectionBufferDataChanged(const QByteArray &bytes);

private:
    QVector3D m_worldIntersection;
    QVector3D m_localIntersection;
    QPoint m_mousePosition;

    Qt3DRender::QGeometry* m_geometry;

    QMutex m_mutex;

    Qt3DRender::QParameter *m_mouseXParameter;
    Qt3DRender::QParameter *m_mouseYParameter;
    Qt3DRender::QParameter *m_intersectionBufferParameter;
    Qt3DRender::QBuffer *m_intersectionBuffer;
    // We need to store all vertices and indices in a parameter
    // so that the shader doesn't loop over individual vertices
    Qt3DRender::QParameter *m_vertexBufferParameter;
    Qt3DRender::QBuffer *m_vertexBuffer;
    Qt3DRender::QParameter *m_vertexByteStrideParameter;
    Qt3DRender::QParameter *m_vertexOffsetParameter;
    Qt3DRender::QParameter *m_indexBufferParameter;
    Qt3DRender::QBuffer *m_indexBuffer;
    Qt3DRender::QParameter *m_requestComputeParameter;
    Qt3DRender::QShaderProgram *m_shaderProgram;
    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QFilterKey *m_filterKey;
};

#endif // TRANSPARENTOBJECTPICKER_H
