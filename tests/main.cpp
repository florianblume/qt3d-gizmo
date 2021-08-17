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
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QFirstPersonCameraController>

Qt3DCore::QEntity *createScene(Qt3DExtras::Qt3DWindow *graphicsWindow) {
    Qt3DCore::QEntity *root = new Qt3DCore::QEntity();

    Qt3DGizmo *gizmo = new Qt3DGizmo(root);
    gizmo->setWindowSize(graphicsWindow->size());
    gizmo->setCamera(graphicsWindow->camera());
    Qt3DRender::QLayer *gizmoLayer = new Qt3DRender::QLayer;
    gizmoLayer->setRecursive(true);
    gizmo->addComponent(gizmoLayer);

    Qt3DRender::QLayer *objectsLayer = new Qt3DRender::QLayer;
    objectsLayer->setRecursive(true);

    Qt3DCore::QEntity *planeEntity = new Qt3DCore::QEntity(root);
    Qt3DExtras::QPlaneMesh *planeMesh = new Qt3DExtras::QPlaneMesh();
    planeMesh->setWidth(5);
    planeMesh->setHeight(5);
    Qt3DExtras::QPhongMaterial *planeMaterial = new Qt3DExtras::QPhongMaterial();
    planeMaterial->setAmbient("lightgray");
    Qt3DCore::QTransform *planeTransform = new Qt3DCore::QTransform();
    objectsLayer->setRecursive(true);
    planeEntity->addComponent(planeMesh);
    planeEntity->addComponent(planeMaterial);
    planeEntity->addComponent(planeTransform);
    planeEntity->addComponent(objectsLayer);

    Qt3DCore::QEntity *torusEntity = new Qt3DCore::QEntity(root);
    Qt3DExtras::QTorusMesh *torusMesh = new Qt3DExtras::QTorusMesh();
    torusMesh->setRings(100);
    torusMesh->setSlices(100);
    torusMesh->setMinorRadius(0.3);
    torusMesh->setRadius(0.5);
    Qt3DExtras::QPhongMaterial *torusMaterial = new Qt3DExtras::QPhongMaterial();
    torusMaterial->setAmbient(QColor(150, 100, 210));
    Qt3DCore::QTransform *torusTransform = new Qt3DCore::QTransform();
    torusTransform->setTranslation(QVector3D(0, 1, 0));
    Qt3DRender::QObjectPicker *torusPicker = new Qt3DRender::QObjectPicker;
    QObject::connect(torusPicker, &Qt3DRender::QObjectPicker::clicked,
                     [gizmo, torusTransform](){
        gizmo->setDelegateTransform(torusTransform);
    });
    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusMaterial);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(objectsLayer);
    torusEntity->addComponent(torusPicker);

    Qt3DCore::QEntity *cuboidEntity1 = new Qt3DCore::QEntity(root);
    Qt3DExtras::QCuboidMesh *cuboidMesh1 = new Qt3DExtras::QCuboidMesh();
    Qt3DExtras::QPhongMaterial *cuboidMaterial1 = new Qt3DExtras::QPhongMaterial();
    cuboidMaterial1->setAmbient(QColor(100, 200, 110));
    Qt3DCore::QTransform *cuboidTransform = new Qt3DCore::QTransform();
    cuboidTransform->setTranslation(QVector3D(0.5, 0.5, 1.5));
    cuboidTransform->setScale(0.5);
    Qt3DRender::QObjectPicker *cuboidPicker1 = new Qt3DRender::QObjectPicker;
    QObject::connect(cuboidPicker1, &Qt3DRender::QObjectPicker::clicked,
                     [gizmo, cuboidTransform](){
        gizmo->setDelegateTransform(cuboidTransform);
    });
    cuboidEntity1->addComponent(cuboidMesh1);
    cuboidEntity1->addComponent(cuboidMaterial1);
    cuboidEntity1->addComponent(cuboidTransform);
    cuboidEntity1->addComponent(objectsLayer);
    cuboidEntity1->addComponent(cuboidPicker1);

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
    graphicsWindow->renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::Always);
    graphicsWindow->camera()->setNearPlane(0.01f);

    QObject::connect(graphicsWindow, &Qt3DExtras::Qt3DWindow::widthChanged,
                     gizmo, &Qt3DGizmo::setWindowWidth);
    QObject::connect(graphicsWindow, &Qt3DExtras::Qt3DWindow::heightChanged,
                     gizmo, &Qt3DGizmo::setWindowHeight);

    return root;
}

int main(int argc, char *argv[]) {
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    format.setVersion(3, 0);
    QSurfaceFormat::setDefaultFormat(format);
    QApplication a(argc, argv);

    Qt3DExtras::Qt3DWindow *graphicsWindow = new Qt3DExtras::Qt3DWindow();
    graphicsWindow->camera()->setPosition(QVector3D(6, 5, 7));
    graphicsWindow->camera()->setViewCenter({0, 1, 0});
    Qt3DCore::QEntity *root = createScene(graphicsWindow);
    graphicsWindow->setRootEntity(root);
    graphicsWindow->show();

    return a.exec();
}
