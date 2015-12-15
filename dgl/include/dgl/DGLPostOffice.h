#ifndef dglpostoffice_h
#define dglpostoffice_h

class DGLMessage;
class dtkSharedMem;
class dtkClient;
#include <vector>
#include <string>
using namespace std;

/*!
 * @class DGLPostOffice DGLPostOffice.h dgl.h
 *
 * @brief A class for sending DGLMessages 
 *
 * Useful in general for Diverse applications 
 *
 * Warning, do not use tabs in your messages, spaces are OK though.
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author Mark Soltys
 * @author John Kelso, kelso@nist.gov
 */
class DGLAPI DGLPostOffice
{
 public:
  /*!
   * Create a DGLPostOffice 
   *
   * @param segmentName name of the dtk shared memory segment containing
   * DGLMessages to read/write. segmentName becomes the default segment,
   * used when no segment name is specified.
   */
  DGLPostOffice(const string& segmentName);

  // usual destructor
  ~DGLPostOffice();

  /*!
   * @brief Send a message using the default segment
   *
   * @param Message the DGLMessage object to send
   *
   * @return zero on success, non-zero if an error occurred
   */
  int sendMessage(DGLMessage* Message);

  /*!
   * @brief Get a message using the default segment
   *
   * @return a pointer to a DGLMessage object, or NULL if there were no
   * messages on the queue
   */
  DGLMessage* getMessage();

  /*!
   * @brief Connect to a remote computer using the default segment
   *
   * @param ComputerName hostname or IP address of a remote computer
   *
   * @return zero on success, non-zero if an error occurred
   */
  int connectTo(const string& ComputerName);

  //If you want to use the class with multiple shared memory segments
  //Use these functions
  
  /*!
   * @brief Add another dtk shared memory segment to the std::vector of
   * possible dtk shared memory segments to use for this DGLPostOffice object
   *
   * @param Name name of the dtk shared memory segment containing
   * DGLMessages to read/write.
   *
   * @return zero on success, non-zero if an error occurred
   */
  int addSegment(const string& Name);
  
  /*!
   * @brief Select which dtk shared memory segment will be used for subsequent
   * message passing using the default segment.
   *
   * The dtk shared memory segment must have already been added with
   * DGLPostOffice::addSegment
   *
   * @param Name name of the dtk shared memory segment containing
   * DGLMessages to read/write using the default segment.
   *
   * @return zero on success, non-zero if an error occurred
   */
  int setDefaultSegment(const string& Name);
  
  /*!
   * @brief Send a message using a specific segment
   *
   * @param Message the DGLMessage object to send
   *
   * @param SegmentName name of the dtk shared memory segment to use for
   * this message
   *
   * @return zero on success, non-zero if an error occurred
   */
  int sendMessage(DGLMessage* Message, const string& SegmentName);
  
  /*!
   * @brief Connect to a remote computer using a specific segment
   *
   * @param SegmentName name of the dtk shared memory segment
   *
   * @param ComputerName hostname or IP address of a remote computer
   *
   * @return zero on success, non-zero if an error occurred
   */
  int connectTo(const string& SegmentName, const string& ComputerName);
  
  /*!
   * @brief Get a message using a specific segment
   *
   * @param Name name of the dtk shared memory segment
   *
   * @return a pointer to a DGLMessage object, or NULL if there were no
   * messages on the queue
   */
  DGLMessage*  getMessageFrom(const string& Name);
 private:
  int         m_size;
  dtkClient*  m_client;
  string m_defaultName;
  dtkSharedMem* m_defaultSegment;       //The default segment
  vector<dtkSharedMem*> m_segments;     //The shared memory segments
  vector<string>        m_segmentNames; //The segment names
  //These are mainly so that we don't have variables going in and out of
  //scope repeatedly
  char* m_buff;                    //Temporary read buffer
  string m_temp;                   //Temporary string for use
};
#endif
