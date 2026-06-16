HEADERS	+= *.h
SOURCES	+= *.cpp

TEMPLATE   = lib
CONFIG     += plugin 
CONFIG     -= debug
CONFIG     += release
CONFIG     += build_all
#CONFIG     += silent
equals(QT_MAJOR_VERSION, 5): QT += opengl
equals(QT_MAJOR_VERSION, 6): QT += openglwidgets

INCLUDEPATH += include
INCLUDEPATH += ../../viewer/glwidget/include
INCLUDEPATH += ../../viewer/interfaces
INCLUDEPATH += ../../viewer/core/include
INCLUDEPATH += ../glslang
INCLUDEPATH += ../glslang/glslang/Include
INCLUDEPATH += ../glslang/glslang/Public
INCLUDEPATH += ../glslang/SPIRV
INCLUDEPATH += ../glslang/glslang/MachineIndependent
INCLUDEPATH += ../shaderLoader

LIBS += -L../glslang/build/glslang
LIBS += -L../glslang/build/SPIRV
LIBS += -L../glslang/build/glslang/OSDependent/Unix
LIBS += -lglslang \
        -lMachineIndependent \
        -lOSDependent \
        -lSPIRV \
        -lglslang-default-resource-limits

DESTDIR = ../bin

MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

unix:target.path = ../bin/plugins
win32:target.path = ../bin/plugins

# GLEW
win32:INCLUDEPATH += E:/lib/glew-1.10.0/include/
win32:LIBS += -LE:/lib/glew-1.10.0/lib/Release/Win32
win32:LIBS += -lglew32
INSTALLS   += target

win32:LIBS += -L../../viewer/bin/ -lcore
win32:  LIBS += ../../viewer/bin/pluginglwidget.lib -L../bin/ -lpluginglwidget

macx: LIBS += -L../../viewer/bin/ -lcore
macx: LIBS += -L../bin/ -lglwidget






