#ifndef _dgl_pipe_h_
#define _dgl_pipe_h_

#include <dtk.h>
#include <vector>
using namespace std;

class DGLWindow;
class DGLDisplay;

/*!
 * @class DGLPipe DGLPipe.h dgl.h
 *
 * @brief DGL Pipe class.  
 *
 * A "pipe" is a graphics output device.  There is one object of this class
 * for each pipe.  A pipe contains windows, as described by the DGLWindow
 * class, one DGLWindow object per window.
 *
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author John Kelso, kelso@nist.gov
*/

class DGLAPI DGLPipe : public dtkBase
{
 public:
  
  /*!
   * Create a pipe, with default display number 0, screen number 0, hostname = ""
   *
   */
  DGLPipe(void)
    {m_display = DGL::getApp()->display(); m_displayNumber = 0 ; m_displayNumberSet = false; m_screenNumber = 0 ; m_screenNumberSet = false; m_hostName = NULL ; m_hostNameSet = false; m_pci_bus_id = -1;m_pci_device_id = -1;validate();};

  // tidy up!
  virtual ~DGLPipe()
    {m_window.erase(m_window.begin(), m_window.end());};

  /*!
   * @brief Set the display number of this pipe
   *
   */
  virtual void setDisplayNum(const unsigned int& num)
    {m_displayNumberSet = true ; m_displayNumber = num;};

  /*!
   * @brief Returns true if the display number was explicitly set
   *
   */
  virtual bool setDisplayNum(void)
    {return m_displayNumberSet;};

  /*!
   * @brief Return the display number of this pipe
   *
   * @return the display number of this pipe
   */
  virtual unsigned int getDisplayNum()
    {return m_displayNumber;};

  /*!
   * @brief Set the screen number in the pipe
   *
   */
  virtual void setScreenNum(const int& screenNum)
    { m_screenNumberSet = true ; m_screenNumber = screenNum;};
  
  /*!
   * @brief Returns true if the screen number was explicitly set
   *
   */
  virtual bool setScreenNum(void)
    {return m_screenNumberSet;};

  /*!
   * @brief Return the screen number in the pipe
   *
   * @return the number of the screen in the pipe
   */
  virtual unsigned int getScreenNum()
    {return m_screenNumber;};

  /*!
   * @brief Set the hostname of the pipe (localhost or yourRemoteMachineHere)
   *
   */
  virtual void setHostName(char* host)
    {m_hostNameSet = true ; m_hostName = host;};

  /*!
   * @brief Returns true if the hostname was explicitly set
   *
   */
  virtual bool setHostName(void)
    {return m_hostNameSet;};

  /*!
   * @brief Return the hostname of the pipe (localhost or yourRemoteMachineHere)
   *
   * @return the hostname of the display in the pipe
   */
  virtual char* getHostName()
    {return m_hostName;};

   /*!
   * @brief Return the number of DGLWindow objects in this pipe
   *
   * @return the number of DGLWindows in this pipe
   */ 
  virtual unsigned int getNumWindows()
    { return m_window.size();};

  /*!
   * @brief Return a pointer the specified DGLWindow
   *
   * @param num an index into the std::vector of DGLWindow objects
   *
   * @return a pointer the specified DGLWindow
   */
  virtual DGLWindow* getWindow(unsigned int num) 
    { if (num < m_window.size()) return m_window[num]; else return NULL;};
  
  /*!
   * @brief Add a DGLWindow to the std::vector of DGLWindow objects
   *
   * @param window a pointer to the DGLWindow object to be added to the
   * std::vector of DGLWindow objects
   */
  virtual void addWindow(DGLWindow* window)
    {m_window.push_back(window);};

  virtual void setPCIBusID( int pci_bus_id )
	{
		m_pci_bus_id = pci_bus_id;
	}

	virtual int getPCIBusID()
	{
		return m_pci_bus_id;
	}

  virtual void setPCIDeviceID( int pci_device_id )
	{
		m_pci_device_id = pci_device_id;
	}

	virtual int getPCIDeviceID()
	{
		return m_pci_device_id;
	}

 private:
  unsigned int       m_screenNumber;
  bool               m_screenNumberSet;
  unsigned int	     m_displayNumber;
  bool               m_displayNumberSet;
  char*		     m_hostName;
  bool               m_hostNameSet;
  DGLDisplay*        m_display;
  vector<DGLWindow*> m_window;
	int                m_pci_bus_id;
	int                m_pci_device_id;
};
#endif
