#
# Common begin part
#

include $(top_srcdir)/adm/unix/make_begin.am

#
# Developper part
#

lib_LTLIBRARIES = libCpp_Template_SWIG.la
nodist_libCpp_Template_SWIG_la_SOURCES = Cpp_Template__wrap.cxx 

BUILT_SOURCES = Cpp_Template__wrap.cxx

SWIG_PYTHON_INCLUDES = $(MED_INCLUDES) -I$(top_srcdir)/src/Cpp_Template_/Cpp_Template__CXX

INCLUDES = -I$(top_srcdir)/src/Cpp_Template_/Cpp_Template__CXX $(PYTHON_INCLUDES) $(MED_INCLUDES)

libCpp_Template_SWIG_la_LIBADD = -L../Cpp_Template__CXX -lCpp_Template_CXX $(MED_LIBS) -lMEDMEM_Swigcmodule


bin_SCRIPTS = Cpp_Template_SWIG.py

clean-local:
	$(RM) Cpp_Template_SWIG.py Cpp_Template__wrap.cxx

#
# Common end part
#

include $(top_srcdir)/adm/unix/make_end.am
