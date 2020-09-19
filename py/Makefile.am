AM_CXXFLAGS = $(WARNING_FLAGS) $(PKG_FLAGS)

SOURCES = \
    ssr.cpp \
    pyrendererwrapper.h \
    ../src/legacy_orientation.cpp \
    ../src/legacy_orientation.h

pyexec_LTLIBRARIES = ssr@PY_LIB_SUFFIX@.la
ssr@PY_LIB_SUFFIX@_la_CPPFLAGS = $(PYTHON_CFLAGS) -I$(srcdir)/../apf -I$(srcdir)/../pybind11/include
ssr@PY_LIB_SUFFIX@_la_SOURCES = $(SOURCES)
ssr@PY_LIB_SUFFIX@_la_LDFLAGS = -avoid-version -module -shared -export-dynamic
# libssr@PY_LIB_EXT@_la_LDFLAGS = -version-info 0:0:0