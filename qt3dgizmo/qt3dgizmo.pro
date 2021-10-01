TEMPLATE = lib

VERSION = 1.0

QT       += widgets 3dcore 3drender 3dextras 3dinput 3dlogic

CONFIG += c++11 shared_and_static build_all

# To copy the new header file to the include folder
QMAKE_POST_LINK += mkdir -p $PWD/../include/ | cp $$PWD/qt3dgizmo.h $$PWD/../include/qt3dgizmo.h

DEFINES += QT3DWIDGETLIB_LIBRARY

DESTDIR += $$PWD/../lib

SOURCES += \
    arrowtranslationhandle.cpp \
    flatmaterial.cpp \
    handle.cpp \
    planetranslationhandle.cpp \
    qt3dgizmo.cpp \
    raycomputematerial.cpp \
    rotationhandle.cpp \
    transparentobjectpicker.cpp

HEADERS += \
    arrowtranslationhandle.h \
    constants.h \
    flatmaterial.h \
    handle.h \
    plane.h \
    planetranslationhandle.h \
    qt3dgizmo.h \
    qt3dgizmo_p.h \
    ray.h \
    raycomputematerial.h \
    rotationhandle.h \
    transparentobjectpicker.h

RESOURCES += \
    shaders.qrc
