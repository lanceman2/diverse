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

/********************************************************************/
/**************** EDIT TO CONFIGURE *********************************/
/********************************************************************/

// This just works on GNU/Linux operating systems.

// This service will auto detect the first correct USB device.


// List of possible USB device IDs

static const unsigned short device_id[][4] =
  {
    /* For a given input device ID there are 4 numbers
     *  { bus,  vender,  product,  version }
     */

    // "Logitech Inc. WingMan Extreme Digital 3D"
    { DTKUSB_ANYID, 0x46d, 0xc207, DTKUSB_ANYID },

    // add another here
    // { DTKUSB_ANYID, 0x45e, 0x9, DTKUSB_ANYID },

    { 0, 0, 0, 0 }
  };


// Calibration stuff, see wingManJoystick.cpp for meaning.

#define JOY_X_REST_VAL       545
#define JOY_Y_REST_VAL       552
#define JOY_Z_REST_VAL       141

#define JOY_X_SCALE          300
#define JOY_Y_SCALE          300
#define JOY_Z_SCALE          70

#define JOY_DEAD_BAND_WIDTH  10

#define THROTTLE_MIN_VAL     48
#define THROTTLE_MAX_VAL     193


#ifndef DTK_ARCH_LINUX
# error "This is only ported to GNU/Linux"
#endif

/********************************************************************/
/**************** END --  EDIT TO CONFIGURE *************************/
/********************************************************************/
