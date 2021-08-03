#include "qt3dgizmo.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>

#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
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
    Qt3DCore::QEntity *root = new Qt3DCore::QEntity();

    Qt3DCore::QEntity *objectEntity = new Qt3DCore::QEntity(root);
    Qt3DExtras::QTorusMesh *mesh = new Qt3DExtras::QTorusMesh();
    mesh->setRings(100);
    mesh->setSlices(100);
    mesh->setMinorRadius(0.5);
    mesh->setRadius(1);
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setAmbient("gray");
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
    Qt3DRender::QLayer *objectsLayer = new Qt3DRender::QLayer;
    objectsLayer->setRecursive(true);
    objectEntity->addComponent(mesh);
    objectEntity->addComponent(material);
    objectEntity->addComponent(transform);
    objectEntity->addComponent(objectsLayer);

    Qt3DExtras::QFirstPersonCameraController *cameraController = new Qt3DExtras::QFirstPersonCameraController(root);
    //cameraController->setCamera(graphicsWindow->camera());

    graphicsWindow->camera()->setNearPlane(0.01f);

    Qt3DGizmo *gizmo = new Qt3DGizmo(root);
    gizmo->setDelegateTransform(transform);
    gizmo->setWindowSize(graphicsWindow->size());
    gizmo->setCamera(graphicsWindow->camera());
    Qt3DRender::QLayer *gizmoLayer = new Qt3DRender::QLayer;
    gizmoLayer->setRecursive(true);
    gizmo->addComponent(gizmoLayer);

    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector;
    renderSurfaceSelector->setSurface(graphicsWindow);
    Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport(renderSurfaceSelector);
    Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    cameraSelector->setCamera(graphicsWindow->camera());
    Qt3DRender::QRenderStateSet *renderStateSet = new Qt3DRender::QRenderStateSet(cameraSelector);
    Qt3DRender::QDepthTest *depthTest = new Qt3DRender::QDepthTest;
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);
    renderStateSet->addRenderState(depthTest);

    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(renderStateSet);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::AllBuffers);
    clearBuffers->setClearColor("white");
    Qt3DRender::QNoDraw *noDraw = new Qt3DRender::QNoDraw(clearBuffers);

    Qt3DRender::QLayerFilter *layerFilter1 = new Qt3DRender::QLayerFilter(renderStateSet);
    layerFilter1->addLayer(objectsLayer);

    Qt3DRender::QClearBuffers *clearBuffers2 = new Qt3DRender::QClearBuffers(renderStateSet);
    clearBuffers2->setBuffers(Qt3DRender::QClearBuffers::DepthBuffer);
    Qt3DRender::QNoDraw *noDraw2 = new Qt3DRender::QNoDraw(clearBuffers2);

    Qt3DRender::QLayerFilter *layerFilter2 = new Qt3DRender::QLayerFilter(renderStateSet);
    layerFilter2->addLayer(gizmoLayer);

    graphicsWindow->setActiveFrameGraph(renderSurfaceSelector);
    graphicsWindow->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);

    QTimer animationTimer;
    animationTimer.setInterval(10);
    QObject::connect(&animationTimer, &QTimer::timeout, [graphicsWindow, transform](){
        if (transform->translation().x() < 10) {
            //transform->setTranslation(transform->translation() + QVector3D(0.001, 0, 0));
        }

    });
    animationTimer.start();

    QObject::connect(graphicsWindow, &Qt3DExtras::Qt3DWindow::widthChanged,
                     gizmo, &Qt3DGizmo::setWindowWidth);
    QObject::connect(graphicsWindow, &Qt3DExtras::Qt3DWindow::heightChanged,
                     gizmo, &Qt3DGizmo::setWindowHeight);

    graphicsWindow->setRootEntity(root);
    graphicsWindow->camera()->setPosition(QVector3D(2, 3, 7));
    graphicsWindow->camera()->setViewCenter({0, 0, 0});

    graphicsWindow->show();

    return a.exec();
}
