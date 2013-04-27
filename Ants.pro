TEMPLATE = app

DEPENDPATH = . gui logic manager

INCLUDEPATH += . \
               ./gui \
               ./logic \
               ./manager

QMAKE_CXXFLAGS += -std=c++11 -g

TARGET   = Ants-demo
CONFIG  += console
CONFIG  -= app_bundle

HEADERS += main_window.hpp \
           gui/AntWidget.hpp \
           gui/IAntGUI.hpp \
           gui/IAntGuiImpl.hpp \
           logic/IAntLogic.hpp \
           logic/IAntLogicImpl.hpp \
           manager/AntManager.hpp


SOURCES += main.cpp \
           main_window.cpp \
           gui/AntWidget.cpp \
           gui/IAntGUI.cpp \
           gui/IAntGuiImpl.cpp \
           logic/IAntLogic.cpp \
           logic/IAntLogicImpl.cpp \
           manager/AntManager.cpp

FORMS   += \
    MainForm.ui
