/*
 * The DIVERSE Toolkit
 * Copyright (C) 2000 - 2003  Virginia Tech
 * 
 * This is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License (GPL) as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software, in the top level source directory in a file
 * named "COPYING.GPL"; if not, see it at:
 * http://www.gnu.org/copyleft/gpl.html or write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 */
// this is the base class for class Foo in foo.cpp.

// Thank you Bill for this #$^%%$Y# mess.
#ifdef DTK_ARCH_WIN32_VCPP
# ifdef EXPORT_THIS
#  define DLLPORT __declspec(dllexport)
# else
#  define DLLPORT __declspec(dllimport)
# endif
#else
# define DLLPORT
#endif

class DLLPORT Base
{
public:
  Base(const char *s);
  virtual int base(void) = 0;
  virtual ~Base(void);
};

