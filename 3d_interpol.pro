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
    src/kernel/linear_system_configurer.cpp \
    src/kernel/workers/solver.cpp

HEADERS += \
    src/lib/containers/limited_deque.h \
    src/lib/containers/simple_vector.h \
    src/lib/sparse_matrix/msr_dqgmres_initializer.h \
    src/lib/sparse_matrix/msr_matrix.h \
    src/lib/sparse_matrix/msr_thread_dqgmres_solver.h \
    src/lib/sparse_matrix/msr_thread_handler.h \
    src/lib/threads/thread_handler.h \
    src/lib/threads/thread_vector_utils.h \
    src/kernel/linear_system_configurer.h \
    src/kernel/workers/solver.h

INCLUDEPATH += src/lib
