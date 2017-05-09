QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += opengl
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

LIBS += -pthread
LIBS += -lGL
LIBS += -glew

SOURCES += \
#    src/main.cpp \
    src/lib/containers/simple_vector.cpp \
    src/lib/sparse_matrix/msr_dqgmres_initializer.cpp \
    src/lib/sparse_matrix/msr_matrix.cpp \
    src/lib/sparse_matrix/msr_thread_dqgmres_solver.cpp \
    src/lib/sparse_matrix/msr_thread_handler.cpp \
    src/lib/threads/thread_handler.cpp \
    src/lib/threads/thread_vector_utils.cpp \
    src/kernel/workers/solver.cpp \
    src/kernel/least_squares_interpol.cpp \
    src/lib/ttime/ttime.cpp \
    src/gui/main_window.cpp \
    src/gui/gl_plot_widget.cpp \
    src/main_gui.cpp \
    src/test_functions/test_functions.cpp

HEADERS += \
    src/lib/containers/limited_deque.h \
    src/lib/containers/simple_vector.h \
    src/lib/sparse_matrix/msr_dqgmres_initializer.h \
    src/lib/sparse_matrix/msr_matrix.h \
    src/lib/sparse_matrix/msr_thread_dqgmres_solver.h \
    src/lib/sparse_matrix/msr_thread_handler.h \
    src/lib/threads/thread_handler.h \
    src/lib/threads/thread_vector_utils.h \
    src/kernel/workers/solver.h \
    src/kernel/least_squares_interpol.h \
    src/lib/ttime/ttime.h \
    src/gui/main_window.h \
    src/gui/gl_plot_widget.h \
    src/test_functions/test_functions.h

INCLUDEPATH += src/
INCLUDEPATH += src/lib
