C  Copyright (C) 2006-2010  CEA/DEN, EDF R&D
C
C  This library is free software; you can redistribute it and/or
C  modify it under the terms of the GNU Lesser General Public
C  License as published by the Free Software Foundation; either
C  version 2.1 of the License.
C
C  This library is distributed in the hope that it will be useful,
C  but WITHOUT ANY WARRANTY; without even the implied warranty of
C  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
C  Lesser General Public License for more details.
C
C  You should have received a copy of the GNU Lesser General Public
C  License along with this library; if not, write to the Free Software
C  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
C
C  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
C

       subroutine addvec(C,A,B,n)
       integer i,n
       double precision A(n), B(n), C(n)
C
       do i=1, n
          C(i) = A(i) + B(i)
       enddo
C
       return
       end
