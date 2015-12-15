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
#include <stdio.h>
#include <dtk.h>

#define PROGRAM_NAME  "dtk-shutdownServer"

int Usage(void)
{
  dtkColor c(stdout);
  printf("  Usage: %s%s%s [%s--help%s|%s-h%s]\n"
	 "\n"
	 "  %s%s%s shutsdown the DTK server that is connects to.\n"
	 "The environment variable %sDTK_CONNECTION%s may be set to the\n"
	 "servers address and port and override the default of %s%s%s.\n"
	 "If you wish to shutdown a DTK server that does not have a INET\n"
	 "TCP interface, you must find the servers PID and send it a\n"
	 "SIGTERM signal.\n"
	 "\n",
	 c.yel,PROGRAM_NAME,c.end,
	 c.grn, c.end, c.grn, c.end, 
	 c.yel,PROGRAM_NAME,c.end,
	 c.tur, c.end,
	 c.grn, DTK_DEFAULT_SERVER_CONNECTION, c.end);
  return 1;
}


int main(int argc, char **argv)
{
  if(argc > 1)
    return Usage();

  dtkMsg.setPreMessage("%s%s%s",dtkMsg.color.vil,
		       PROGRAM_NAME, dtkMsg.color.end);
  dtkClient client;
  if(client.isInvalid())
    {
      dtkMsg.append("   Do you have a %sdtk-server%s running?\n",
		 dtkMsg.color.vil, dtkMsg.color.end);
      return 1;
    }

  return - client.shutdownServer();
}
