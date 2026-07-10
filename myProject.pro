QT       += core gui widgets sql charts concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DatabaseManager.cpp \
    LoadDataWorker.cpp \
    LoginDialog.cpp \
    MainWindow.cpp \
    SessionManager.cpp \
    StatisticsService.cpp \
    StudentService.cpp \
    StudentTableModel.cpp \
    main.cpp
HEADERS += \
    DatabaseManager.h \
    LoadDataWorker.h \
    LoginDialog.h \
    MainWindow.h \
    SessionManager.h \
    StatisticsService.h \
    StudentEntity.h \
    StudentService.h \
    StudentTableModel.h

FORMS += \
    LoginDialog.ui \
    MainWindow.ui

TRANSLATIONS += \
    myProject_zh_CN.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Login
