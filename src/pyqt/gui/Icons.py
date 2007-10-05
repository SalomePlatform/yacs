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

