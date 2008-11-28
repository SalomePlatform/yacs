//  Copyright (C) 2006-2008  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SALOME_CONFIG_H
#define SALOME_CONFIG_H

#define DEBUG

#define QUOTE(x)  #x
#define CORBA_CLIENT_HEADER(x)  QUOTE(x.hh)
#define CORBA_SERVER_HEADER(x)  QUOTE(x.hh)

#ifndef PCLINUX
  #define PCLINUX
#endif


/* A path to a rcp-like command */
#define RCP "/usr/bin/rcp"

/* A path to a rm-like command */
#define RM "/bin/rm"

/* A path to a cp-like command */
#define CP "/bin/cp"

/* A path to a rsh-like command */
#define RSH "/usr/bin/rsh"

/* A path to a scp-like command */
#define SCP "/usr/bin/scp"

/* A path to a sh-like command */
#define SH "/bin/sh"

/* A path to a ssh-like command */
#define SSH "/usr/bin/ssh"

#endif
