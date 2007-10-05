# Check if component environment is either defined or not
#
# Author : Jean-Yves PRADILLON (OPEN CASCADE, 2005)
#

AC_DEFUN([CHECK_COMPONENT_ENV],[

AC_CHECKING(for Component Environment)

Comp_Env_ok=no

if test -d "$HXX2SALOME_GENERIC_CLASS_NAMECPP_ROOT_DIR" ; then
   Comp_Env_ok=yes
   AC_MSG_RESULT(Using Component Root Dir ${HXX2SALOME_GENERIC_CLASS_NAMECPP_ROOT_DIR})
else
   AC_MSG_WARN(Cannot find Component Root Dir "${HXX2SALOME_GENERIC_CLASS_NAMECPP_ROOT_DIR}")
   if test "x$HXX2SALOME_GENERIC_CLASS_NAMECPP_ROOT_DIR" = "x" ; then
      AC_MSG_WARN(Did you source the environment file?)
   fi
fi
  
AC_MSG_RESULT(for Component Environment: $Comp_Env_ok)
 
])dnl
 
