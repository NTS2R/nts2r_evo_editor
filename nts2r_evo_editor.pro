#-------------------------------------------------
#
# Project created by QtCreator 2019-05-13T22:36:26
#
#-------------------------------------------------

QT       += core gui axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nts2r_evo_editor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
        commander.cpp \
        formation.cpp \
        main.cpp \
        mainwindow.cpp \
        militarycommander.cpp

HEADERS += \
        commander.h \
        formation.h \
        mainwindow.h \
        militarycommander.h

FORMS += \
        formation.ui \
        mainwindow.ui \
        militarycommander.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QXLSX_PARENTPATH=./QXlsx/QXlsx       # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include(./QXlsx/QXlsx/QXlsx.pri)

