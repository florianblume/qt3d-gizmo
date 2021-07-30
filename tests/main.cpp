#include "qt3dgizmo.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>

#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>

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
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setAmbient("red");
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
    Qt3DCore::QEntity *entity = new Qt3DCore::QEntity();
    entity->addComponent(mesh);
    entity->addComponent(material);
    entity->addComponent(transform);
    Qt3DExtras::QOrbitCameraController *cameraController = new Qt3DExtras::QOrbitCameraController(entity);
    cameraController->setCamera(graphicsWindow->camera());

    Qt3DGizmo *gizmo = new Qt3DGizmo();
    gizmo->setDelegateTransform(transform);
    gizmo->setWindowSize(graphicsWindow->size());
    gizmo->setViewMatrix(graphicsWindow->camera()->viewMatrix());
    gizmo->setProjectionMatrix(graphicsWindow->camera()->projectionMatrix());

    Qt3DExtras::QForwardRenderer *forwardRenderer = new Qt3DExtras::QForwardRenderer;
    forwardRenderer->setCamera(graphicsWindow->camera());
    forwardRenderer->setClearColor(Qt::gray);
    graphicsWindow->setActiveFrameGraph(forwardRenderer);

    QTimer animationTimer;
    animationTimer.setInterval(10);
    QObject::connect(&animationTimer, &QTimer::timeout, [transform](){
        transform->setRotationX(transform->rotationX() + 1);
    });
    animationTimer.start();

    graphicsWindow->setRootEntity(entity);
    graphicsWindow->camera()->setPosition(QVector3D(0, 0, 10));

    graphicsWindow->show();

    return a.exec();
}
