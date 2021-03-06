/*
 * The DIVERSE Toolkit
 * Copyright (C) 2000 - 2003  Virginia Tech
 * 
 * This software, the DIVERSE Toolkit library, is free software; you can
 * redistribute it and/or modify it under the terms of the GNU Lesser
 * General Public License (LGPL) as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option) any
 * later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software, in the top level source directory in
 * a file named "COPYING.LGPL"; if not, see it at:
 * http://www.gnu.org/copyleft/lesser.html or write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 */

//#define XEVENT_MASK    (ExposureMask|ButtonPressMask|StructureNotifyMask)
#define XEVENT_MASK    (StructureNotifyMask)

// default dtkAugment name
#define DTKGLXSIMPLE_TITLE "glxSimpleDisplay"

class dtkGLXSimpleDisplay: public dtkBasicDisplay // which is a dtkAugment
{
 public:

  dtkGLXSimpleDisplay(dtkManager *manager, const char *name,
		      int x=0, int y=0, int w=400, int h=400,
		      const char *title = DTKGLXSIMPLE_TITLE);
  dtkGLXSimpleDisplay(dtkManager *manager, const char *name,
		      int x, int y,
		      const char *title=DTKGLXSIMPLE_TITLE);
  dtkGLXSimpleDisplay(dtkManager *manager, const char *name,
		      const char *title);
  virtual ~dtkGLXSimpleDisplay(void);

  int frame(void);
  int sync(void);

 private:

  Display *dpy;
  XVisualInfo *vi;
  Bool doubleBuffer;
  GLXContext cx;
  Colormap cmap;
  Window win;
  //Bool needRedraw;
  //Bool recalcModelView;
  int isMapped;

  unsigned int refresh_count;

  void clean_up(void);
  void init(const char *name,
	    int x, int y, int w, int h,
	    const char *title);

  void reshapeWin(int w, int h);

  dtkXWindowList *windowList;
};

