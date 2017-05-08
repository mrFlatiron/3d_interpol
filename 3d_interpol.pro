TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread

SOURCES += src/main.cpp \
    src/lib/containers/simple_vector.cpp \
    src/lib/sparse_matrix/msr_dqgmres_initializer.cpp \
    src/lib/sparse_matrix/msr_matrix.cpp \
    src/lib/sparse_matrix/msr_thread_dqgmres_solver.cpp \
    src/lib/sparse_matrix/msr_thread_handler.cpp \
    src/lib/threads/thread_handler.cpp \
    src/lib/threads/thread_vector_utils.cpp \
    src/kernel/workers/solver.cpp \
    src/kernel/least_squares_interpol.cpp \
    src/lib/ttime/ttime.cpp

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
    src/lib/ttime/ttime.h

INCLUDEPATH += src/lib
