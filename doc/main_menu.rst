
.. _main_menu:

Main Menu
=========

The menu bar is located on the upper part of the desktop window.



.. image:: images/main_menu_0.png
  :align: center


.. centered::
  **Main Menu**



Let's consider the available menus in more detail.

.. _file:

File
----
File menu contains standard operations for creating, opening and saving documents (SALOME studies) as well as schema creation or import operations. The SALOME study is saved in a HDF file with default extension \*.hdf. YACS Schema must be saved apart in XML files.

If YACS module has been loaded File menu contains the following items:


.. image:: images/main_menu_2.png
  :align: center



+ YACS:


    + :ref:`create_new_schema`



    + :ref:`import_yacs_schema`




Edit
----
Edit menu supports standard edition operations like copy/paste and undo/redo functionality. Cut, copy, paste, undo/redo should be available in edition mode only. ( **Cut, copy and paste are only available within context popup menus, and undo/redo are not currently implemented, but will be available in the future version!** )



.. image:: images/main_menu_3.png
  :align: center



Other edition operations on different types of objects are available with help of corresponding context popup menus or Input Panel property pages. For more information about this topic see :ref:`edit_object` section.

The user can :ref:`delete_object` with help of corresponding context popup menu item in the edition Tree View or 2D view.


View
----
View menu allows maintaining visibility of the different parts of the desktop like toolbars, status bar, Object Browser, Tree View, Catalogs, Input Panel.



.. image:: images/main_menu_4.png
  :align: center


.. centered::
  **Standard Toolbar**


.. image:: images/main_menu_5.png
  :align: center

.. centered::
  **Windows**


.. _yacs_menu:

YACS menu
---------
YACS menu contains a set of commands for creating opening and saving of YACS schemas, importing old SUPERV schemas, execute schemas, import catalogs, and set some specific behavior of 2D views. YACS menu contains the following items:


.. image:: images/main_menu_6.png
  :align: center


+ :ref:`create_new_schema` : create a new schema from scratch


+ :ref:`import_yacs_schema` : load a YACS schema saved on an XML file 


+ :ref:`import_supervisor_graph` : load a SUPERV schema saved on an XML file


+ Save Schema : save in the current opened file


+ :ref:`export_schema` : save with file dialog box to choose a new filename


+ :ref:`execute_schema` : switch from edition to run mode


+ :ref:`save_restore_execution_state` (from a state of execution saved previously)


+ Load a Schema in run mode (without edition)


+ Import Catalog : to import specific data types or nodes


+ Automatic Links: toggle behaviour for automatic link reconstruction when 2D View layout changes


+ Simplify Links: toggle behaviour for link simplification (less segments)


+ Force Ortho Links: toggle behaviour on very simple links (2 segments becomes 3 orthogonals segments)


+ Separate Links: toggle behaviour to try to avoid or not superposition of links 


+ What's this mode : experimental, detailled help on a few items



Tools
-----
Tools menu provides two standard Salome useful tools such as XML catalog generator and registry display for supervision over component's processes.



.. image:: images/main_menu_10.png
  :align: center




Window
------
Window menu contains commands to arrange already opened windows in a different ways (split horizontally or vertically) or to create a new window with predefined type.



.. image:: images/main_menu_11.png
  :align: center




Help
----
Help menu allows reading some basic information about application with help of 'About' dialog box and provides help for YACS module as HTML documents.



.. image:: images/main_menu_12.png
  :align: center


