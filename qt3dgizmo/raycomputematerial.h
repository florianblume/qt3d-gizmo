#ifndef RAYCOMPUTEMATERIAL_H
#define RAYCOMPUTEMATERIAL_H

#include "ray.h"

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QBuffer>

class RayComputeMaterial : public Qt3DRender::QMaterial {

    Q_OBJECT

public:
    RayComputeMaterial();
    void setMouseCoordinates(int x, int y);

signals:
    void rayComputed(const Ray &ray);

private slots:
    void bufferDataChanged();

private:
    Qt3DRender::QParameter *m_mouseXParameter;
    Qt3DRender::QParameter *m_mouseYParameter;
    Qt3DRender::QParameter *m_rayBufferParameter;
    Qt3DRender::QParameter *m_requestComputeParameter;
    Qt3DRender::QShaderProgram *m_shaderProgram;
    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QTechnique *m_technique;
    Qt3DRender::QRenderPass *m_renderPass;
    Qt3DRender::QFilterKey *m_filterKey;
    Qt3DRender::QBuffer *m_rayBuffer;

    int m_computationCount = 0;
};

#endif // RAYCOMPUTEMATERIAL_H
