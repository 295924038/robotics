#-------------------------------------------------
#
# Project created by QtCreator 2019-08-06T20:44:34
#
#-------------------------------------------------

QT       += core gui network serialport serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XindeBao
TEMPLATE = app
CONFIG += c++1y

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

PROROOT = /home/td/robotics/weldroidProject
PROJ_HOME=/home/td/robotics
OPENCV_HOME = /home/td/opencv320
BOOST_HOME = /home/td/boost1640
DAHENG_ROOT = /home/td/Galaxy_U2_V2.0.1_Linux_x86_64_CN
GENICAM_ROOT_V2_3 = /home/td/Galaxy_U2_V2.0.1_Linux_x86_64_CN/sdk/genicam


MOC_DIR = $$PWD/moc
UI_DIR = $$PWD/ui
OBJECTS_DIR = $$PWD/obj

src_dir = $${PROROOT}/src/UI/Xindebao/etc
dst_dir = $$OUT_PWD/etc

!exists($$dst_dir) {
  system(cp -R $$src_dir $$dst_dir)
}

QMAKE_CXXFLAGS = -std=c++1z

INCLUDEPATH += $${OPENCV_HOME}/include \
               $${PROROOT}/include \
               $${BOOST_HOME}/include \
               $${DAHENG_ROOT}/sdk/include \
               $${GENICAM_ROOT_V2_3}/library/CPP/include \


LIBS += -L$${OPENCV_HOME}/lib \
        -L$${PROROOT}/lib \
        -L$${BOOST_HOME}/lib/debug \
        -L$${GENICAM_ROOT_V2_3}/bin/Linux64_x64 \
        -L$${GENICAM_ROOT_V2_3}/bin/Linux64_x64/GenApi/Generic \
        -ldhgentl -lgxiapi -ldximageproc    \
        -lGCBase_gcc40_v2_3 -lGenApi_gcc40_v2_3 -llog4cpp_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3 \
        -limage -l3dvision -lbasic -lgeometry -lbase \
        -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_tracking \
        -lboost_filesystem -lboost_system -lboost_date_time \
        -lnlopt \

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        preferences.cpp \
    pubfunc/imagelabel.cpp \
    pubfunc/mylineedit.cpp \
    camera/adjustCamera.cpp \
    camera/recogNeedleCamera.cpp \
    camera/recogWeldCamera.cpp \
    interface.cpp \
    loginon.cpp \
    input.cpp \
    motor/motorx.cpp \
    motor/motorz.cpp

HEADERS += \
        mainwindow.h \
        preferences.h \
    pubfunc/imagelabel.h \
    pubfunc/mylineedit.h \
    camera/adjustCamera.h \
    camera/recogNeedleCamera.h \
    camera/recogWeldCamera.h \
    interface.h \
    loginon.h \
    input.h \
    motor/motorx.h \
    motor/motorz.h

FORMS += \
        mainwindow.ui \
    preferences.ui \
    interface.ui \
    loginon.ui \
    input.ui
