QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console
CONFIG -= windows

DESTDIR = $$PWD/../bin



# OpenDDS configuration
OPENDDS_ROOT = C:/Users/pnmt1054/Adithya_working_directory/QT_PROJECTS/INDIGIS_DEPS/OpenDDS-3.33.0
IDLNAME = Messager
IDLFILESOURCE = ../idl


exists(OPENDDS_ROOT) {
    message("Using OpenDDS from: $$OPENDDS_ROOT")
} else {
    error("OPENDDS_ROOT directory does not exist: $$OPENDDS_ROOT")
}
isEmpty(OPENDDS_ROOT) {
    error("DDS_ROOT environment variable is not set")
}
exists(OPENDDS_ROOT){
  error("DDS_ROOT Directory is not exits")
}
ACE_ROOT = $$OPENDDS_ROOT/ACE_wrappers
TAO_ROOT = $$ACE_ROOT/TAO
MPC_ROOT = $$ACE_ROOT/MPC
RAPIDJSON_ROOT = $$OPENDDS_ROOT/tools/rapidjson



INCLUDEPATH += \
    $$OPENDDS_ROOT \
    $$ACE_ROOT \
    $$TAO_ROOT \
    $$TAO_ROOT/orbsvcs \

LIBS += -L$$OPENDDS_ROOT/lib \
        -L$$ACE_ROOT/lib \
        -L$$TAO_ROOT/lib

LIBS += -lOpenDDS_Dcpsd -lOpenDDS_InfoRepoLibd
LIBS += -L$$OPENDDS_ROOT/lib \
        -lOpenDDS_Dcpsd \
        -lOpenDDS_InfoRepoLibd \
        -lOpenDDS_InfoRepoDiscoveryd \
        -lOpenDDS_Multicastd \
        -lOpenDDS_Rtpsd \
        -lOpenDDS_Tcpd \
        -lOpenDDS_Udpd

LIBS += -lOpenDDS_Rtps_Udpd


CONFIG(debug, debug|release) {
    LIBS += \
        -lTAOd \
        -lTAO_PortableServerd \
        -lTAO_AnyTypeCoded \
        -lTAO_CodecFactoryd \
        -lTAO_Valuetyped \
        -lTAO_DynamicInterfaced \
        -lTAO_Svc_Utilsd \
        -lACEd
} else {
    LIBS += \
        -lTAO \
        -lTAO_PortableServer \
        -lTAO_AnyTypeCode \
        -lTAO_CodecFactory \
        -lTAO_Valuetype \
        -lTAO_DynamicInterface \
        -lTAO_Svc_Utils
        -lACE
}


HEADERS += \
    $$IDLFILESOURCE/$${IDLNAME}C.h \
    $$IDLFILESOURCE/$${IDLNAME}S.h \
    $$IDLFILESOURCE/$${IDLNAME}TypeSupportImpl.h \
    $$IDLFILESOURCE/$${IDLNAME}TypeSupportC.h \
    $$IDLFILESOURCE/$${IDLNAME}TypeSupportS.h \
    ../OpenDDSHelper.h \
    ../MessageReaderListener.h
SOURCES += \
    $$IDLFILESOURCE/$${IDLNAME}C.cpp \
    $$IDLFILESOURCE/$${IDLNAME}S.cpp \
    $$IDLFILESOURCE/$${IDLNAME}TypeSupportImpl.cpp \
    $$IDLFILESOURCE/$${IDLNAME}TypeSupportC.cpp \
    $$IDLFILESOURCE/$${IDLNAME}TypeSupportS.cpp \
    ../OpenDDSHelper.cpp \
    ../MessageReaderListener.cpp

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
