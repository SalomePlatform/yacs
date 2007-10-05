#
# Common begin part
#

include $(top_srcdir)/adm/unix/make_begin.am

#
# Developper part
#

lib_LTLIBRARIES            = libCpp_Template_CXX.la
libCpp_Template_CXX_la_SOURCES  = Cpp_Template_.cxx
libCpp_Template_CXX_la_LIBADD   = $(MED_LIBS)

# exported headers
library_includedir=$(includedir)
library_include_HEADERS = Cpp_Template_.hxx
			
INCLUDES  = $(MED_INCLUDES)


bin_PROGRAMS = Cpp_Template__test
Cpp_Template__test_SOURCES = main.cxx
Cpp_Template__test_LDADD   = -L. -lCpp_Template_CXX 


#
# Common end part
#

include $(top_srcdir)/adm/unix/make_end.am


