TEMPLATE = app

QT       += core gui widgets 3dcore 3drender 3dextras opengl

CONFIG += c++11

DEPENDPATH += $$PWD $$PWD/../include
INCLUDEPATH += $$PWD/../include
LIBS += -L$$PWD/../lib/ -lqt3dgizmo

SOURCES += \
    main.cpp
