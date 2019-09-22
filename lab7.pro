#-------------------------------------------------
#
# Project created by QtCreator 2013-11-04T22:45:25
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = lab7
TEMPLATE = app

SOURCES += main.cpp\
    window.cpp \
    widget.cpp

HEADERS  += \
    window.h \
    widget.h

LIBS+= -lglut -lGLU

FORMS    += widget.ui \
    window.ui

OTHER_FILES +=

RESOURCES += \
    shaders.qrc \
    textures.qrc \
    raws.qrc
