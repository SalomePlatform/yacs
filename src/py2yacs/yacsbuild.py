#!/usr/bin/env python3
# Copyright (C) 2006-2025  CEA, EDF
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

if __name__ == '__main__':
  import argparse
  parser = argparse.ArgumentParser(
    description="Build a YACS schema out of a decorated python script.")
  parser.add_argument("path", help='Path to the script.')
  parser.add_argument("mainbloc",
        help='Name of the function containing the main bloc of the schema.')
  parser.add_argument("yacsfile", help='Path to the output yacs file.')
  parser.add_argument("-c", "--containers",
                      help="File of containers.",
                      default=None)
  args = parser.parse_args()
  import yacsdecorator
  yacsdecorator.activateYacsMode()
  if not args.containers is None :
    yacsdecorator.loadContainers(args.containers)
  import yacstools
  fn = yacstools.getFunction(args.path, args.mainbloc)
  fn()
  yacsdecorator.export(args.yacsfile)
