.. _calciummod:

==============================================
:mod:`calcium` -- python interface
==============================================

.. module:: calcium
   :platform: Unix
   :synopsis: calcium interface (connect to YACS and exchange data between coupled components)

calcium python module provides all is needed to connect to YACS and exchange data between coupled components.

Presentation is done in the following order:

- constants
- array types
- functions to connect and disconnect
- functions to export data
- functions to import data
- functions to erase data
- example

Constants
================
The constants defined in this module are:

.. data:: CP_TEMPS

   This constant is used in cp_e* and cp_l* calls to indicate that data are associated to a time step.

.. data:: CP_ITERATION

   This constant is used in cp_e* and cp_l* calls to indicate that data are associated to an iteration number.

.. data:: CP_SEQUENTIEL

   This constant is used in cp_l* calls to indicate that the call requests the next data that has been produced.

.. data:: CP_CONT

   This constant can be used in cp_fin to indicate that variables produced by the component are defined 
   beyond the last time or iteration number and their value is equal to the last value produced.

.. data:: CP_ARRET

   This constant can be used in cp_fin to indicate that variables produced by the component are not defined 
   beyond the last time or iteration number.

and all the error codes as listed in :ref:`errcodes`.

Array types
=============
numpy arrays can be used to export or import data but if the numpy module is not installed
calcium provides simple array types to replace them.

.. class:: calcium.intArray(nelem)

       The constructor takes a single argument which is the number of elements of the array

.. class:: calcium.longArray(nelem)

       The constructor takes a single argument which is the number of elements of the array

.. class:: calcium.floatArray(nelem)

       The constructor takes a single argument which is the number of elements of the array

.. class:: calcium.doubleArray(nelem)

       The constructor takes a single argument which is the number of elements of the array

.. class:: calcium.stringArray(nelem, eltsize)

       The constructor takes two arguments: the number of elements of the array and the size
       of the elements (strings)

Functions to connect and disconnect
===============================================

.. function:: calcium.cp_cd(compo) -> info, name

   Initialize the connection with YACS.

   :param compo: component reference
   :type compo: SALOME component object

   :param info: error code
   :type info: int
   :param name: instance name given by YACS
   :type name: string

.. function:: calcium.cp_fin(compo, directive) -> info

   Close the connection with YACS.

   :param compo: component reference
   :type compo: SALOME component object
   :param directive: indicate how variables will be handled after disconnection. If directive = CP_CONT,
     variables produced by this component are defined constant beyond the last time or iteration number. If
     directive = CP_ARRET, variables are not defined beyond the last step.
   :type directive: int

   :param info: error code
   :type info: int

Functions to export data
============================
.. function:: calcium.cp_ere(compo, dep, t, i, nm_var, n,var_real) -> info
.. function:: calcium.cp_edb(compo, dep, t, i, nm_var, n,var_double) -> info
.. function:: calcium.cp_ecp(compo, dep, t, i, nm_var, n,var_complex) -> info
.. function:: calcium.cp_een(compo, dep, t, i, nm_var, n,var_integer) -> info
.. function:: calcium.cp_elg(compo, dep, t, i, nm_var, n,var_long) -> info
.. function:: calcium.cp_eln(compo, dep, t, i, nm_var, n,var_long) -> info
.. function:: calcium.cp_elo(compo, dep, t, i, nm_var, n,var_boolean) -> info
.. function:: calcium.cp_ech(compo, dep, t, i, nm_var, n,var_string) -> info

   :param compo: component reference
   :type compo: SALOME component object
   :param dep: dependency mode (calcium.CP_TEMPS, calcium.CP_ITERATION)
   :type dep: int
   :param t: export time if mode=calcium.CP_TEMPS
   :type t: float
   :param i: export iteration number if mode=calcium.CP_ITERATION
   :type i: int
   :param nm_var: port name
   :type nm_var: string
   :param n: number of values to export
   :type n: int
   :param var_real: array containing float values to export
   :type var_real: float numpy array or :class:`calcium.floatArray`
   :param var_double: array containing double values to export
   :type var_double: double numpy array or :class:`calcium.doubleArray`
   :param var_complex: array containing complex values to export
   :type var_complex: complex numpy array or :class:`calcium.floatArray` (double size)
   :param var_integer: array containing integer values to export
   :type var_integer: integer numpy array or :class:`calcium.intArray`
   :param var_long: array containing long values to export
   :type var_long: long numpy array or :class:`calcium.longArray`
   :param var_boolean: array containing boolean values to export
   :type var_boolean: integer numpy array or :class:`calcium.intArray`
   :param var_string: array containing string values to export
   :type var_string: string numpy array or :class:`calcium.stringArray`

   :param info: error code
   :type info: int

With numpy, the data types to use when creating the array, are the following:

============ ====================
Request       numpy data type
============ ====================
cp_lre           'f'
cp_ldb           'd'
cp_lcp           'F'
cp_len           'i'
cp_llg           'l'
cp_lln           'l'
cp_llo           'i'
cp_lch           'Sxx' 
============ ====================

.. note::

   For cp_lch xx is the size of the element string


Functions to import data
===========================
.. function:: calcium.cp_lre(compo, dep, ti, tf, i, nm_var, len, var_real) -> info, t, ii, n
.. function:: calcium.cp_ldb(compo, dep, ti, tf, i, nm_var, len, var_double) -> info, t, ii, n
.. function:: calcium.cp_lcp(compo, dep, ti, tf, i, nm_var, len, var_complex) -> info, t, ii, n
.. function:: calcium.cp_len(compo, dep, ti, tf, i, nm_var, len, var_integer) -> info, t, ii, n
.. function:: calcium.cp_llg(compo, dep, ti, tf, i, nm_var, len, var_long) -> info, t, ii, n
.. function:: calcium.cp_lln(compo, dep, ti, tf, i, nm_var, len, var_long) -> info, t, ii, n
.. function:: calcium.cp_llo(compo, dep, ti, tf, i, nm_var, len, var_boolean) -> info, t, ii, n
.. function:: calcium.cp_lch(compo, dep, ti, tf, i, nm_var, len, var_string) -> info, t, ii, n

   :param compo: component reference
   :type compo: SALOME component object
   :param dep: dependency mode (calcium.CP_TEMPS, calcium.CP_ITERATION or calcium.CP_SEQUENTIEL)
   :type dep: int
   :param ti: interval start time
   :type ti: float
   :param tf: interval end time
   :type tf: float
   :param i: iteration number
   :type i: int
   :param nm_var: port name
   :type nm_var: string
   :param len: number of values to import
   :type len: int
   :param var_real: array to store imported float values (must be large enough to contain imported values)
   :type var_real: float numpy array or :class:`calcium.floatArray`
   :param var_double: array to store imported double values
   :type var_double: double numpy array or :class:`calcium.doubleArray`
   :param var_complex: array to store imported complex values
   :type var_complex: complex numpy array or :class:`calcium.floatArray` (double size)
   :param var_integer: array to store imported integer values
   :type var_integer: integer numpy array or :class:`calcium.intArray`
   :param var_long: array to store imported long values
   :type var_long: long numpy array or :class:`calcium.longArray`
   :param var_boolean: array to store imported boolean values
   :type var_boolean: integer numpy array or :class:`calcium.intArray`
   :param var_string: array to store imported string values
   :type var_string: string numpy array or :class:`calcium.stringArray`

   :param info: error code
   :type info: int
   :param t: effective time if mode=calcium.CP_TEMPS or associated time if mode=calcium.CP_SEQUENTIEL
   :type t: float
   :param ii: associated iteration number if mode=calcium.CP_SEQUENTIEL
   :type ii: int
   :param n: effective number of imported values (<= len)
   :type n: int

Functions to erase data
==========================
The functions cp_fini and cp_fint are used to request that all values of the specified
variable defined for iteration number or time before a given one be erased.

The functions cp_effi and cp_efft are used to request that all values of the specified
variable defined for iteration number or time after a given one be erased.

.. function:: calcium.cp_fini(compo, nm_var, i) -> info

   Erase all values of port nm_var before iteration i

   :param compo: component reference
   :type compo: SALOME component object
   :param nm_var: port name
   :type nm_var: string
   :param i: iteration number
   :type i: int
   :param info: error code
   :type info: int, return

.. function:: calcium.cp_fint(compo, nm_var, t) -> info

   Erase all values of port nm_var before time t

   :param compo: component reference
   :type compo: SALOME component object
   :param nm_var: port name
   :type nm_var: string
   :param t: time
   :type t: float
   :param info: error code
   :type info: int, return

.. function:: calcium.cp_effi(compo, nm_var, i) -> info

   Erase all values of port nm_var after iteration i

   :param compo: component reference
   :type compo: SALOME component object
   :param nm_var: port name
   :type nm_var: string
   :param i: iteration number
   :type i: int
   :param info: error code
   :type info: int, return

.. function:: calcium.cp_efft(compo, nm_var, t) -> info

   Erase all values of port nm_var after time t

   :param compo: component reference
   :type compo: SALOME component object
   :param nm_var: port name
   :type nm_var: string
   :param t: time
   :type t: float
   :param info: error code
   :type info: int, return

Example
========
If you want to export a double precision real array and import an integer array in time mode,
you could write a function as follows. Connection and disconnection must be done only once.

.. code-block:: numpy

   import calcium
   import numpy

   def coupling(compo):
     # connect to YACS
     info, name = calcium.cp_cd(compo)
     # export 10 double values at time 0. on port outa 
     af=numpy.zeros(10,'d')
     af[0]=4.3
     info = calcium.cp_edb(compo,calcium.CP_TEMPS,0.,0,"outa",10,af);
     tf=1.;
     # import 10 integer values at interval time (0.,1.) on port ina 
     # (by default it is imported at the start time 0.)
     ai=numpy.zeros(10,'i')
     info,t,i,n = calcium.cp_len(compo,calcium.CP_TEMPS,0., 1.,0,"ina",10,ai);
     # disconnect 
     info = calcium.cp_fin(compo,calcium.CP_CONT);



