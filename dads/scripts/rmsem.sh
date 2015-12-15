#!/bin/sh

# DIVERSE Toolkit (DTK) -- version 2.1.0
# released: June 26, 2002   12:02:34  EDT
# DTK HOMPAGE is at: http://www.diverse.vt.edu/DTK/
# 
# The DIVERSE Toolkit
# Copyright (C) 2000, 2001, 2002  Lance Arsenault
# 
# This software, the DIVERSE Toolkit library, is free software; you can
# redistribute it and/or modify it under the terms of the GNU Lesser
# General Public License (LGPL) as published by the Free Software
# Foundation; either version 2.1 of the License, or (at your option) any
# later version.
# 
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this software, in the top level source directory in
# a file named "COPYING.LGPL"; if not, see it at:
# http://www.gnu.org/copyleft/lesser.html or write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA.


user=`whoami`
os=`uname`
if [ $os = "Linux" ]
then
  if [ $user = "root" ]
  then
    i="`ipcs -s | awk '/0x/ {print $2}'`"
  else
    i="`ipcs -s | grep $user | awk '/0x/ {print $2}'`"
  fi
  for j in $i
  do
    echo "ipcrm sem $j"
    ipcrm sem $j
  done
else # IRIX
  if [ $user = "root" ]
  then
    i="`ipcs -s | awk '/0x/ {print $3}'`"
  else
    i="`ipcs -s | grep $user | awk '/0x/ {print $3}'`"
  fi
  for j in $i
  do
    echo "ipcrm -S $j"
    ipcrm -S $j
  done
fi
