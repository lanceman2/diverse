#ifndef _DOSG_SHM_COORD_NODE_H_
#define _DOSG_SHM_COORD_NODE_H_

#include <string.h>

#include <osg/PositionAttitudeTransform>
#include <osg/CopyOp>

#include <dosg.h>

/*!
 * @class dosgShmCoordNode dosgShmCoordNode.h dosg.h
 *
 * @brief A node based on osg::PositionAttitudeTransform whose matrix is
 * updated every frame based on data in dtk shared memory
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
////////////////////////////////////////////////////////////////////////
class DOSGAPI dosgShmCoordNode : public osg::PositionAttitudeTransform
{

 public:

    /*!
     * @brief Create a dosgShmCoordNode
     *
     * @param coordName the name of a dtk shared memory file containing six
     * floats, corresponding to the XYZHPR coordinate.
     *
     * @param activeName the name of a one byte dtk shared memory file
     * containing a byte which is used to signal if coordName should be
     * read for updates.  If set to NULL, coordName is always read.
     *
     * @param flag is ANDed with the value in activeName, and if the result
     * is non-zero, coordName is read.
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    dosgShmCoordNode(const char *coordName=NULL, const char *activeName=NULL, char flag=0xFF);

    dosgShmCoordNode(const dosgShmCoordNode& stn, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    META_Node(osg, dosgShmCoordNode);
    
    virtual ~dosgShmCoordNode();

    /*!  
     * @brief returns true if the node is "valid", i.e., able to connect
     * to the requested dtk shared memory files.
     *
     *  @return true if the node is "valid"
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    bool isValid() { return _valid ; } ;

    /*!  
     * @brief returns true if the node is "invalid", i.e., unable to connect
     * to the requested dtk shared memory files.
     *
     * @return true if the node is "invalid"
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    bool isInvalid() { return !_valid ; } ;

    /*!  
     * @brief sets the name of the dtk shared memory file to use for XYZHPR data.
     *
     * @param newName the name of the dtk shared memory file to use for XYZHPR data
     *
     * @return true if successful
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    bool setCoordName(const char *newName) ;

    /*!  
     * @brief returns the name of the dtk shared memory file used for XYZHPR data.
     *
     * @return the name of the dtk shared memory file used for XYZHPR data
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    const char *getCoordName() { return _coordName ; } ;

    /*!  
     * @brief returns a pointer to the dtk shared memory file used for XYZHPR data.
     *
     * @return a pointer to the dtk shared memory file used for XYZHPR data.
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    dtkSharedMem* getCoordShm() { return _coordShm ; } ;

    /*!  
     * @brief sets the name of the dtk shared memory file that determines
     * if the shared memory file containing XYZHPR data is read.
     *
     * @param newName the name of the dtk shared memory file
     *
     * @return true if successful
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    bool setActiveName(const char *newName) ;

    /*!  
     * @brief returns the name of the dtk shared memory file that determines
     * if the shared memory file containing XYZHPR data is read.
     *
     * @return the name of the dtk shared memory file that determines
     * if the shared memory file containing XYZHPR data is read.
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    const char *getActiveName() { return _activeName ; } ;

    /*!  
     * @brief returns a pointer to the dtk shared memory file that determines
     * if the shared memory file containing XYZHPR data is read.
     *
     * @return a pointer to the dtk shared memory file that determines
     * if the shared memory file containing XYZHPR data is read.
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    dtkSharedMem* getActiveShm() { return _activeShm ; } ;

    /*!
     * @brief sets the flag that is ANDed with the dtk shared memory file that determines
     * if the shared memory file containing XYZHPR data is read.  If the result is non-zero,
     * the dtk shared memory file used for XYZHPR data is read.
     *
     * @param c the flag that is ANDed with the dtk shared memory file that determines
     * if the shared memory file containing XYZHPR data is read.
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    void flag(char c) { _flag = c ; } ;

    /*!
     * @brief returns the flag that is ANDed with the dtk shared memory file that determines
     * if the shared memory file containing XYZHPR data is read.  If the result is non-zero,
     * the dtk shared memory file used for XYZHPR data is read.
     *
     * @returns the flag that is ANDed with the dtk shared memory file that determines
     * if the shared memory file containing XYZHPR data is read.
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    char flag() { return _flag ; } ;

    /*!
     * @brief indicates if the node should read shared memory and update
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    void on(bool b) { _on = b ; } ;
     
    /*!
     * @brief returns the value of on(bool b)
     *
     * @returns the value of on(bool b)
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    bool on() { return _on ; } ;
     
     

 private:
    dtkSharedMem* _coordShm ;
    char*         _coordName ;

    dtkSharedMem* _activeShm ;
    char*         _activeName ;

    char          _flag ;

    bool          _valid ;

    bool          _on ; 

};

#endif
