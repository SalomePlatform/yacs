# Copyright (C) 2006-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""Ce module fournit des icones (QPixmap) pour afficher dans l'arbre
   Ces icones sont obtenues a partir d'un nom et conservees dans un cache.
   La source des icones est soit une string xpm soit un fichier gif ou autre
"""

import os
from qt import QPixmap
from imagesxpm import dico_xpm

dico_images={}

def get_image(name):
  if dico_images.has_key(name):
    return dico_images[name]
  else :
    if dico_xpm.has_key(name):
      image=QPixmap(dico_xpm[name])
    else:
      fic_image = os.path.join(os.path.dirname(__file__),"icons",name)
      if not os.path.isfile(fic_image):
        file, ext = os.path.splitext(fic_image)
        fic_image = file + '.gif'
      image = QPixmap(fic_image)
    dico_images[name]=image
    return image

def update_cache():
   global dico_images
   dico_images={}

