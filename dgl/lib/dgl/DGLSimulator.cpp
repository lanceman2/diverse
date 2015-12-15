// cave simulator base class
// John Kelso, kelso@nist.gov, 5/06

#include <dtk.h>
#include <dgl.h>

DGLSimulator::DGLSimulator(dtkManager *m) :
 dtkAugment("DGLSimulator")
{
  //Class for sending the message 
#ifdef DGL_ARCH_WIN32
  msgSender = new DGLPostOffice("dgl\DGLSimulator");
#else
  msgSender = new DGLPostOffice("dgl/DGLSimulator");
#endif
  msg = NULL ;
  validate() ;

}

int DGLSimulator::postFrame(void) 
{
  while ((msg = msgSender->getMessage()))
    {
      static const char *subject ;
      subject = msg->getSubject().c_str() ;
      if (!strcasecmp(subject,"head"))
	{
	  headMsg() ;
	}
      else if (!strcasecmp(subject,"wand"))
	{
	  wandMsg() ;
	}
      else if (!strcasecmp(subject,"jump"))
	{
	  jumpMsg() ;
	}
      else if (!strcasecmp(subject,"jumpto"))
	{
	  jumpToMsg() ;
	}
      else
	{
	  dtkMsg.add(DTKMSG_ERROR,"DGLSimulator::postFrame: unknown message subject \"%s\".\n", subject);
	}
      msg->clear() ;
    }
  return CONTINUE; 
}

void DGLSimulator::headMsg(void) 
{
  if (msg->getInt())
    {
      head(true) ;
    }
  else
    {
      head(false) ;
    }
}

void DGLSimulator::wandMsg(void) 
{
  if (msg->getInt())
    {
      wand(true) ;
    }
  else
    {
      wand(false) ;
    }
}

void DGLSimulator::jumpMsg(void) 
{
  if (msg->getInt())
    {
      jump(true) ;
    }
  else
    {
      jump(false) ;
    }
}

void DGLSimulator::jumpToMsg(void) 
{
  static float x, y, z, h, p, r ;
  x = msg->getFloat() ;
  y = msg->getFloat() ;
  z = msg->getFloat() ;
  h = msg->getFloat() ;
  p = msg->getFloat() ;
  r = msg->getFloat() ;
  jumpTo(dtkCoord(x, y, z, h, p, r)) ;
}

void DGLSimulator::head(bool v) 
{
  printf("DGLSimulator::head(%d)\n",v) ;
}

void DGLSimulator::wand(bool v) 
{
  printf("DGLSimulator::wand(%d)\n",v) ;
}

void DGLSimulator::jump(bool v) 
{
  printf("DGLSimulator::jump(%d)\n",v) ;
}

void DGLSimulator::jumpTo(dtkCoord coord) 
{
  printf("DGLSimulator::jumpTo") ;
  coord.print() ;
}




