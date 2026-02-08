QT += widgets multimedia
TARGET = qt_pomodoro
TEMPLATE = app
SOURCES += main.cpp \
           mainwindow.cpp \
           reminder_dialog.cpp \
           floating_timer.cpp
HEADERS += mainwindow.h \
           reminder_dialog.h \
           floating_timer.h
FORMS += mainwindow.ui
RESOURCES += resources.qrc