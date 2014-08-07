
.. _calciumapi:


==========================================================
CALCIUM C and Fortran coupling library
==========================================================
This section is the CALCIUM coupling library reference for C and Fortran.
For python, see module :mod:`calcium`.

Presentation is done in the following order:

- include files : constants
- functions to connect and disconnect
- functions to export data
- functions to import data
- functions to erase data
- examples
- error codes

Include files
====================
All constants used with the coupling library are defined in an include file.

**C:**

.. code-block:: c

   #include "calcium.h"

**Fortran:**

.. code-block:: fortran

   INCLUDE 'calcium.hf'

These constants are:

- Error codes (see :ref:`errcodes`)
- Dependency modes (CP_TEMPS, CP_ITERATION or CP_SEQUENTIEL)
- Disconnection modes (CP_CONT or CP_ARRET)


.. default-domain:: c

Functions to connect and disconnect
===============================================

**C:**

.. function:: int info = cp_cd(long *compo, char *instance_name)

   Initialize the connection with YACS.

**Fortran:**

.. function:: call cpcd(compo, instance_name, info)

   :param compo: component address
   :type compo: long
   :param instance_name: instance name given by YACS
   :type instance_name: 64 characters string, output
   :param info: error code (possible codes: CPMACHINE)
   :type info: int, return


**C:**

.. function:: int info = cp_fin(long *compo, int directive)

   Close the connection with YACS.

**Fortran:**

.. function:: call cpfin(compo, directive, info)

   :param compo: component address
   :type compo: long
   :param directive: indicate how variables will be handled after disconnection. If directive = CP_CONT,
     variables produced by this component are defined constant beyond the last time or iteration number. If
     directive = CP_ARRET, variables are not defined beyond the last step.
   :type directive: int
   :param info: error code (possible codes: CPDNDI)
   :type info: int, return


Functions to export data
===============================
Writing requests is the way for a component to export data through one of its
output ports. nm_var is the port name.

Each request exports n values from the var_xxx array. Exported values are defined 
at time t if mode is CP_TEMPS or at iteration number i if mode is CP_ITERATION.

It is mandatory to export data at increasing times or iteration numbers.
The export is done in an asynchronous way. Control is given back to the caller as
soon as data have been sent.

There is generally one request for each data type to export. Each request can be used with one and only port type.
For long type, two requests are available (cp_elg and cp_eln). One (cp_elg) is working with CALCIUM_integer port type
and the second (cp_eln) is working with CALCIUM_long port type.

The available port types are :

- CALCIUM_integer
- CALCIUM_long
- CALCIUM_float
- CALCIUM_double
- CALCIUM_complex
- CALCIUM_string
- CALCIUM_logical
 
**C:**

.. function:: int info = cp_ere(long *compo, int dep, float t, int i, char *nm_var, int n, float *var_real)

        for single precision floating point values (C float type and CALCIUM_float port type)
.. function:: int info = cp_edb(long *compo, int dep, double td, int i, char *nm_var, int n, double *var_double)

        for double precision floating point values (C double type and CALCIUM_double port type)
.. function:: int info = cp_ecp(long *compo, int dep, float t, int i, char *nm_var, int n, float *var_complex)

        for complex values (C float type and CALCIUM_complex port type)
.. function:: int info = cp_een(long *compo, int dep, float t, int i, char *nm_var, int n, int *var_integer)

        for integer values (C int type and CALCIUM_integer port type)
.. function:: int info = cp_elg(long *compo, int dep, float t, int i, char *nm_var, int n, long *var_long)

        for integer values (C long type and CALCIUM_integer port type)
.. function:: int info = cp_eln(long *compo, int dep, float t, int i, char *nm_var, int n, long *var_long)

        for integer values (C long type and CALCIUM_long port type)
.. function:: int info = cp_elo(long *compo, int dep, float t, int i, char *nm_var, int n, int *var_boolean)

        for boolean values (C int type and CALCIUM_logical port type)
.. function:: int info = cp_ech(long *compo, int dep, float t, int i, char *nm_var, int n, char **var_string, int strSize)

        for string values (C char* type and CALCIUM_string port type)

**Fortran:**

.. function:: CALL CPERE(LONGP compo, INTEGER dep, REAL*4 t,  INTEGER i, nm_var, INTEGER n, REAL*4 var_real,      INTEGER info)
.. function:: CALL CPEDB(LONGP compo, INTEGER dep, REAL*8 td, INTEGER i, nm_var, INTEGER n, REAL*8 var_double,    INTEGER info)
.. function:: CALL CPECP(LONGP compo, INTEGER dep, REAL*4 t,  INTEGER i, nm_var, INTEGER n, REAL*4 var_complex,   INTEGER info)
.. function:: CALL CPEEN(LONGP compo, INTEGER dep, REAL*4 t,  INTEGER i, nm_var, INTEGER n, INTEGER var_integer,  INTEGER info)
.. function:: CALL CPELG(LONGP compo, INTEGER dep, REAL*4 t,  INTEGER i, nm_var, INTEGER n, INTEGER*8 var_long,   INTEGER info)

               can only be used on 64 bits architecture.
.. function:: CALL CPELN(LONGP compo, INTEGER dep, REAL*4 t,  INTEGER i, nm_var, INTEGER n, INTEGER*8 var_long,   INTEGER info)

               can only be used on 64 bits architecture.
.. function:: CALL CPEIN(LONGP compo, INTEGER dep, REAL*4 t,  INTEGER i, nm_var, INTEGER n, INTEGER*4 var_int,    INTEGER info)
.. function:: CALL CPELO(LONGP compo, INTEGER dep, REAL*4 t,  INTEGER i, nm_var, INTEGER n, INTEGER*4 var_boolean,INTEGER info)
.. function:: CALL CPECH(LONGP compo, INTEGER dep, REAL*4 t,  INTEGER i, nm_var, INTEGER n, var_string,           INTEGER info)

   :param compo: component address
   :type compo: long
   :param dep: dependency type - CP_TEMPS (time dependency) or CP_ITERATION (iteration dependency)
   :type dep: int
   :param t: time value if dep=CP_TEMPS
   :type t: float
   :param td: time value if dep=CP_TEMPS
   :type td: double
   :param i: iteration number if dep=CP_ITERATION
   :type i: int
   :param nm_var: port name
   :type nm_var: string (64 characters)
   :param n: number of values to export (from var_xxx array)
   :type n: int
   :param var_real: array containing the values to export
   :type var_real: float array
   :param var_complex: array containing the values to export (array size is twice the number of complex numbers)
   :type var_complex: float array
   :param var_integer: array containing the values to export
   :type var_integer: int array
   :param var_long: array containing the values to export
   :type var_long: long array
   :param var_boolean: array containing the values to export
   :type var_boolean: int array
   :param var_string: array containing the values to export
   :type var_string: array of strings
   :param var_double: array containing the values to export
   :type var_double: double array
   :param strSize: size of strings in var_string
   :type strSize: int
   :param info: error code (possible codes: CPIT, CPITVR, CPNMVR, CPNTNULL, CPIOVR, CPTPVR or CPCTVR)
   :type info: int, return

.. note::
   LONGP is a Fortran type that is same size as the C long type, so, most of a time, INTEGER\*4 for 32 bits architecture 
   and INTEGER\*8 for 64 bits architecture.

.. warning::
   CPELG (or cp_elg) can produce wrong results (conversion problem) on 64 bits architecture if the KERNEL module is built
   with the default option (--with-cal_int=int)

See :ref:`fortran64bits` for more details.


Functions to import data
=============================
Reading requests is the way for a component to import data through one of its
input ports. nm_var is the port name. Import is only possible if the input port
is connected to an output port.

Reading requests can be of two kinds:
  - standard request
  - sequential request

A standard request imports data at a given time or iteration number. In case of time dependency, the effective time
is calculated by YACS from the interval time (ti, tf) and the interpolation scheme that is given in the coupling file.

A sequential request imports data in a sequential way. Each request returns
the next data with its associated time or iteration number.

There is generally one request for each data type to import. Each request can be used with one and only port type.
For long type, two requests are available (cp_llg and cp_lln). One (cp_llg) is working with CALCIUM_integer port type
and the second (cp_lln) is working with CALCIUM_long port type.

**C:**

.. function:: int info = cp_lre(long *compo, int dep, float *ti, float *tf, int *i, char *nm_var, int len, int *n, float *var_real)
.. function:: int info = cp_ldb(long *compo, int dep, double *tid, double *tfd, int *i, char *nm_var, int len, int *n, double *var_double)
.. function:: int info = cp_lcp(long *compo, int dep, float *ti, float *tf, int *i, char *nm_var, int len, int *n, float *var_complex)
.. function:: int info = cp_len(long *compo, int dep, float *ti, float *tf, int *i, char *nm_var, int len, int *n, int *var_integer)
.. function:: int info = cp_llg(long *compo, int dep, float *ti, float *tf, int *i, char *nm_var, int len, int *n, long *var_long)
.. function:: int info = cp_lln(long *compo, int dep, float *ti, float *tf, int *i, char *nm_var, int len, int *n, long *var_long)
.. function:: int info = cp_llo(long *compo, int dep, float *ti, float *tf, int *i, char *nm_var, int len, int *n, int *var_boolean)
.. function:: int info = cp_lch(long *compo, int dep, float *ti, float *tf, int *i, char *nm_var, int len, int *n, char **var_string, int strSize)

**Fortran:**

.. function:: CALL CPLRE(LONGP compo, INTEGER dep, REAL*4 ti,  tf, INTEGER i, nm_var,INTEGER len, INTEGER n,REAL*4 var_real,      INTEGER info)
.. function:: CALL CPLDB(LONGP compo, INTEGER dep, REAL*8 tid, tf, INTEGER i, nm_var,INTEGER len, INTEGER n,REAL*8 var_double,    INTEGER info)
.. function:: CALL CPLCP(LONGP compo, INTEGER dep, REAL*4 ti,  tf, INTEGER i, nm_var,INTEGER len, INTEGER n,REAL*4 var_complex,   INTEGER info)
.. function:: CALL CPLEN(LONGP compo, INTEGER dep, REAL*4 ti,  tf, INTEGER i, nm_var,INTEGER len, INTEGER n,INTEGER var_integer,  INTEGER info)
.. function:: CALL CPLLG(LONGP compo, INTEGER dep, REAL*4 ti,  tf, INTEGER i, nm_var,INTEGER len, INTEGER n,INTEGER*8 var_long,   INTEGER info)

               can only be used on 64 bits architecture.
.. function:: CALL CPLLN(LONGP compo, INTEGER dep, REAL*4 ti,  tf, INTEGER i, nm_var,INTEGER len, INTEGER n,INTEGER*8 var_long,   INTEGER info)

               can only be used on 64 bits architecture.
.. function:: CALL CPLIN(LONGP compo, INTEGER dep, REAL*4 ti,  tf, INTEGER i, nm_var,INTEGER len, INTEGER n,INTEGER*4 var_int,    INTEGER info)
.. function:: CALL CPLLO(LONGP compo, INTEGER dep, REAL*4 ti,  tf, INTEGER i, nm_var,INTEGER len, INTEGER n,INTEGER*4 var_boolean,INTEGER info)
.. function:: CALL CPLCH(LONGP compo, INTEGER dep, REAL*4 ti,  tf, INTEGER i, nm_var,INTEGER len, INTEGER n,var_string,           INTEGER info)

   :param compo: component address
   :type compo: long
   :param dep: dependency type - CP_TEMPS (time dependency) or CP_ITERATION (iteration dependency)
   :type dep: int
   :param ti: interval start time if dep=CP_TEMPS or associated time if dep=CP_SEQUENTIEL
   :type ti: float, inout
   :param tf: interval end time if dep=CP_TEMPS
   :type tf: float
   :param tid: interval start time if dep=CP_TEMPS or associated time if dep=CP_SEQUENTIEL
   :type tid: double, inout
   :param tfd: interval end time if dep=CP_TEMPS
   :type tfd: double
   :param i: iteration number if dep=CP_ITERATION or dep=CP_SEQUENTIEL
   :type i: int, inout
   :param nm_var: port name
   :type nm_var: string (64 characters)
   :param len: size of var_xxx array
   :type len: int
   :param n: effective number of values imported (into var_xxx array)
   :type n: int, out
   :param var_real: array containing the values imported
   :type var_real: float array, out
   :param var_complex: array containing the values imported (array size is twice the number of complex numbers)
   :type var_complex: float array, out
   :param var_integer: array containing the values imported
   :type var_integer: int array, out
   :param var_long: array containing the values imported
   :type var_long: long array, out
   :param var_boolean: array containing the values imported
   :type var_boolean: int array, out
   :param var_string: array containing the values imported
   :type var_string: array of strings (char*), out
   :param var_double: array containing the values imported
   :type var_double: double array, out
   :param strSize: size of strings in var_string
   :type strSize: int
   :param info: error code (possible codes: CPIT, CPITVR, CPNMVR, CPNTNULL, CPIOVR, CPTPVR, CPLIEN, CPATTENTE, CPLGVR or CPSTOP)
   :type info: int, return

.. note::
   LONGP is a Fortran type that is same size as the C long type, so, most of a time, INTEGER\*4 for 32 bits architecture 
   and INTEGER\*8 for 64 bits architecture.

.. warning::
   CPLLG (or cp_llg) can produce wrong results (conversion problem) on 64 bits architecture if the KERNEL module is built
   with the default option (--with-cal_int=int)

See :ref:`fortran64bits` for more details.

Functions to erase data
==========================
The functions cp_fini and cp_fint are used to request that all values of the specified 
variable defined for iteration number or time before a given one be erased.

The functions cp_effi and cp_efft are used to request that all values of the specified 
variable defined for iteration number or time after a given one be erased.

**C:**

.. function:: int info = cp_fini(long *compo, char *nm_var, int i)

   Erase all values of port nm_var before iteration i

**Fortran:**

.. function:: call cpfini(compo, nm_var, i, info)

   :param compo: component address
   :type compo: long
   :param nm_var: port name
   :type nm_var: string
   :param i: iteration number
   :type i: int
   :param info: error code 
   :type info: int, return

**C:**

.. function:: int info = cp_fint(long *compo, char *nm_var, float t)

   Erase all values of port nm_var before time t

**Fortran:**

.. function:: call cpfint(compo, nm_var, t, info)

   :param compo: component address
   :type compo: long
   :param nm_var: port name
   :type nm_var: string
   :param t: time
   :type t: float
   :param info: error code 
   :type info: int, return

**C:**

.. function:: int info = cp_effi(long *compo, char *nm_var, int i)

   Erase all values of port nm_var after iteration i

**Fortran:**

.. function:: call cpfini(compo, nm_var, i, info)

   :param compo: component address
   :type compo: long
   :param nm_var: port name
   :type nm_var: string
   :param i: iteration number
   :type i: int
   :param info: error code
   :type info: int, return

**C:**

.. function:: int info = cp_efft(long *compo, char *nm_var, float t)

   Erase all values of port nm_var after time t

**Fortran:**

.. function:: call cpfint(compo, nm_var, t, info)

   :param compo: component address
   :type compo: long
   :param nm_var: port name
   :type nm_var: string
   :param t: time
   :type t: float
   :param info: error code
   :type info: int, return

Examples
===========
Fortran example
-------------------
If you want to export a single precision real array and import an integer array in iteration mode,
you could write a subroutine as follows. Connection and disconnection must be done only once.

.. code-block:: fortran

          subroutine coupling(compo)
          include 'calcium.hf'
          integer*8 compo
          real*4 t,af(10)
          integer i, info, n, ai(10), nval
          character*64 name
   C connect to YACS
          call cpcd(compo,name,info)
          n=10
          af(1)=2.5
   C export 10 real values at iteration 1 on port outa
          call cpere(compo,CP_ITERATION,t,i,'outa',n,af,info)
   C import 10 integer values at iteration 1 on port ina
          call cplen(compo,CP_ITERATION,ti,tf,i,'ina',n,nval,ai,info)
	  write(6,*)ai(1)
   C disconnect 
          call cpfin(compo,CP_CONT,info)
          end
 
C example
-------------------
If you want to export a double precision real array and import an integer array in time mode,
you could write a function as follows. Connection and disconnection must be done only once.

.. code-block:: c


   void coupling(void* compo)
   {
     int info, i, n;
     char[64] name;
     double af[10], td,tf;
     int ai[10];
     // connect to YACS
     info = cp_cd(compo,name);
     af[0]=10.99;
     n=10;
     td=0.;
     // export 10 double values at time 0. on port outa
     info = cp_edb(compo,CP_TEMPS,td,i,"outa",n,af);
     tf=1.;
     // import 10 integer values at interval time (0.,1.) on port ina 
     // (by default, it is imported at start time 0.) 
     info = cp_len(compo,CP_TEMPS,&td,&tf,&i,"ina",n,&nval,ai);
     // disconnect 
     info = cp_fin(compo,CP_CONT);
   }
 


.. _errcodes:

Error codes
==============

========= ============ =================================
Code      Value        Explanation
========= ============ =================================
CPOK        0             No error
CPERIU      1             Emitter unknown
CPNMVR      2             Variable name unknown
CPIOVR      3             Different input/output codes in code and supervisor
CPTP        4             Variable type unknown
CPTPVR      5             Different variable types in code and supervisor
CPIT        6             Dependency mode unknown
CPITVR      7             Different dependency modes in code and supervisor
CPRENA      8             Unauthorized request
CPDNTP      9             Unauthorized disconnection request type
CPDNDI     10             Unauthorized disconnection directive
CPNMCD     11             Code name unknown
CPNMIN     12             Instance name unknown
CPATTENTE  13             Waiting request
CPBLOC     14             Blocking
CPNTNUL    15             Zero value number
CPLGVR     16             Insufficient variable length
CPSTOP     17             Instance is going to stop
CPATAL     18             Unexpected instance stop
CPNOCP     19             Manual execution
CPCTVR     20             Output variable not connected
CPPASNULL  21             Number of steps to execute is nul
CPMACHINE  22             Computer not declared
CPGRNU     23             Environment variable COUPLAGE_GROUPE is not set
CPGRIN     24             Instance group given by COUPLAGE_GROUPE is wrong
CPERRFICH  26             Format error in input file
CPNORERR   27             Request ignored because of switching to NORMAL mode
CPRUNERR   28             Supervisor is in normal execution mode
CPOPT      29             Unknown option
CPVALOPT   30             Option value is wrong
CPECREFF   31             Impossible to write because of an erasing request
CPLIEN     32             Reading of a variable wrongly connected
CPDECL     35             Error in declaration
CPINEXEC   36             Error in instance launching
CPCOM      37             Communication error
CPMODE     39             Execution mode not defined
CPINSTDEC  40             Disconnected instance
========= ============ =================================

.. _fortran64bits:

Some considerations about architecture (32, 64 bits) and programming language (C, Fortran)
=============================================================================================
Depending on the architecture and the language, types have varying sizes.
For example, below we compare the size of C and Fortran types for two Linux distributions. One is Debian etch 32 bits
and the other is Debian lenny 64 bits.

**Type size (in bytes) in C:**

======================= ==================== ===================
Architecture, compiler     32 bits, gcc 3.3   64 bits, gcc 4.3
======================= ==================== ===================
short                         2                        2
int                           4                        4
long                          4                        8
long long                     8                        8
float                         4                        4
double                        8                        8
long double                  12                       16
======================= ==================== ===================

**Type size (in bytes) in Fortran:**

======================= ================== ======================= =======================================================
Architecture, compiler   32 bits, g77 3.3   64 bits, gfortran 4.3   64bits, gfortran -fdefault-integer-8 -fdefault-real-8
======================= ================== ======================= =======================================================
integer                      4                    4                    8
integer*8                    8                    8                    8
real                         4                    4                    8
double precision             8                    8                    8
real*8                       8                    8                    8
======================= ================== ======================= =======================================================

With another architecture or compiler, sizes can be different.

Most of a time, Fortran INTEGER is mapped on C int type. So it is 4 bytes wide and equivalent to INTEGER\*4.
This is the case for 32 bits architecture and 64 bits architecture with standard fortran options.

It is possible, with special options, to map Fortran INTEGER on C long type (-i8 with intel compiler or 
-fdefault-integer-8 with gnu fortran, for example). In this case, using the standard CALCIUM API can be cumbersome. 

It is possible to build the SALOME KERNEL module with a special option (--with-cal_int=long) to match this kind of mapping.

By using the cp_een call (or CPEEN fortran call), it is possible to write a code that is independent from the mapping
and that can always use fortran INTEGER type.

