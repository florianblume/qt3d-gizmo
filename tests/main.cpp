#include "qt3dgizmo.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>

#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QRenderStateSet>
#include <Qt3DRender/QLayerFilter>
#include <Qt3DRender/QLayer>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QNoDraw>
#include <Qt3DRender/QDepthTest>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QFirstPersonCameraController>

int main(int argc, char *argv[]) {
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    format.setVersion(3, 0);
    QSurfaceFormat::setDefaultFormat(format);
    QApplication a(argc, argv);

    Qt3DExtras::Qt3DWindow *graphicsWindow = new Qt3DExtras::Qt3DWindow();

    Qt3DExtras::QTorusMesh *mesh = new Qt3DExtras::QTorusMesh();
    mesh->setRings(100);
    mesh->setSlices(100);
    mesh->setMinorRadius(0.5);
    mesh->setRadius(1);
    Qt3DExtras::QCylinderMesh *mesh2 = new Qt3DExtras::QCylinderMesh();
    mesh2->setRadius(20);
    mesh2->setLength(20);
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setAmbient("red");
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
    Qt3DRender::QLayer *objectsLayer = new Qt3DRender::QLayer;
    objectsLayer->setRecursive(true);
    Qt3DCore::QEntity *entity = new Qt3DCore::QEntity();
    //entity->addComponent(mesh2);
    //entity->addComponent(mesh);
    entity->addComponent(material);
    entity->addComponent(transform);
    entity->addComponent(objectsLayer);

    Qt3DExtras::QFirstPersonCameraController *cameraController = new Qt3DExtras::QFirstPersonCameraController(entity);
    cameraController->setCamera(graphicsWindow->camera());

    graphicsWindow->camera()->setNearPlane(0.01f);

    Qt3DGizmo *gizmo = new Qt3DGizmo(entity);
    gizmo->setDelegateTransform(transform);
    gizmo->setWindowSize(graphicsWindow->size());
    gizmo->setCamera(graphicsWindow->camera());
    Qt3DRender::QLayer *gizmoLayer = new Qt3DRender::QLayer;
    gizmoLayer->setRecursive(true);
    gizmo->addComponent(gizmoLayer);
    gizmo->addComponent(objectsLayer);

    Qt3DRender::QRenderStateSet *renderStateSet = new Qt3DRender::QRenderStateSet;
    Qt3DRender::QDepthTest *depthTest = new Qt3DRender::QDepthTest;
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    renderStateSet->addRenderState(depthTest);

    //Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(renderStateSet);
    //clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    //Qt3DRender::QNoDraw *noDraw = new Qt3DRender::QNoDraw(clearBuffers);

    //Qt3DRender::QLayerFilter *layerFilter1 = new Qt3DRender::QLayerFilter(renderStateSet);
    //layerFilter1->addLayer(objectsLayer);
    Qt3DExtras::QForwardRenderer *forwardRenderer = new Qt3DExtras::QForwardRenderer(renderStateSet);
    forwardRenderer->setBuffersToClear(Qt3DRender::QClearBuffers::AllBuffers);
    forwardRenderer->setCamera(graphicsWindow->camera());
    forwardRenderer->setClearColor(Qt::white);

    /*
    Qt3DRender::QLayerFilter *layerFilter2 = new Qt3DRender::QLayerFilter(renderStateSet);
    layerFilter2->addLayer(gizmoLayer);
    Qt3DExtras::QForwardRenderer *forwardRenderer2 = new Qt3DExtras::QForwardRenderer(layerFilter2);
    forwardRenderer2->setBuffersToClear(Qt3DRender::QClearBuffers::DepthBuffer);
    forwardRenderer2->setCamera(graphicsWindow->camera());
    forwardRenderer2->setClearColor(Qt::white);
    */

    graphicsWindow->setActiveFrameGraph(renderStateSet);
    graphicsWindow->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);

    QTimer animationTimer;
    animationTimer.setInterval(10);
    QObject::connect(&animationTimer, &QTimer::timeout, [graphicsWindow, transform](){

    });
    animationTimer.start();

    graphicsWindow->setRootEntity(entity);
    graphicsWindow->camera()->setPosition(QVector3D(-2, 3, 7));
    graphicsWindow->camera()->setViewCenter({0, 0, 0});

    graphicsWindow->show();

    return a.exec();
}
