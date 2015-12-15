#ifndef _DOSG_CLIP_CULL_NODE_H_
#define _DOSG_CLIP_CULL_NODE_H_

#include <stack>
#include <osg/Group>
#include <osg/ClipPlane>
#include <osg/BoundingSphere>
#include <osg/CopyOp>


#include <dosg.h>

#define DOSG_CLIPCULLNODE_NUM_PLANES (6)

class dosgClipCullNode ;

/*!
 * @class dosgClipCullUpdateCallback dosgClipCullNode.h dosg.h
 *
 * @brief update callback which calculates the scenegraph transformation to the
 * clip plane.  Only needed for culling.
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class DOSGAPI dosgClipCullUpdateCallback : public osg::NodeCallback
{
 public:
    dosgClipCullUpdateCallback() {_frame=-1; _oldFrame=-1; }

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor ) ;

 private:
    int _frame ;
    int _oldFrame ;
    
};


/*!
 * @class dosgClipCullNode dosgClipCullNode.h dosg.h
 *
 * @brief  a node which clips and culls all geometry in the +Y plane.
 *
 * @note This class currently crashes when running with culling on and
 * multi-threaded with multiple displays, so culling is off for now.  When
 * it gets fixed, culling will be turned on by default.  In the meantime, it
 * works pretty much like an osg::ClipNode.
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class DOSGAPI dosgClipCullNode : public osg::Group
{

 public:

    enum cullTypeValue{OFF, HIGH, LOW} ;

    dosgClipCullNode();

    dosgClipCullNode(const dosgClipCullNode& ccn, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY);

    META_Node(osg, dosgClipCullNode);
    
    virtual ~dosgClipCullNode();
    virtual osg::BoundingSphere computeBound() const { const osg::BoundingSphere bs(osg::Vec3(0.0f,0.0f,0.0f), FLT_MAX) ; return bs ; } ;

    osg::Matrix matrix() { return _matrix ; } ;    
    void matrix(osg::Matrix m) { _matrix = m ; } ;

    // returns -1 if no plane was created
    int num() { return _num ; } ;    

    void enable() ;
    void disable() ;
    bool enabled() { return _enabled ; } ; 

    static void clip(bool b) { _clip = b ; } ;
    static bool clip() { return _clip ; } ;    
    static void cullType(cullTypeValue c) { _cullType = c ; if (c==OFF) resetNodemasks() ; } ; 
    static cullTypeValue cullType() { return _cullType ; } ;    
    // returns how many dosgClipCullNodes have been created
    static unsigned int howManyNodes() { return _howManyNodes ; } ;
    // returns pointer to any node
    static dosgClipCullNode *ptr(unsigned int i) { return _ptr[i] ; } ;
    static void resetNodemasks() ;
    static void addCulledNode(osg::Node &node, osg::Node::NodeMask mask) ;

 private:
    struct nodeAndMask
    {
	osg::Node *node ;
	osg::Node::NodeMask mask ;
	
    } ;

    int _num ;
    osg::ClipNode *_clipNode ;
    osg::Matrix _matrix ;
    bool _enabled ;

    static bool _clip ;
    static cullTypeValue _cullType ;
    static dosgClipCullNode *_ptr[DOSG_CLIPCULLNODE_NUM_PLANES] ;
    static unsigned int _howManyNodes ; ;
    static std::vector<nodeAndMask> _culledNodesAndMasks ;
};

/*!
 * @class dosgClipCullVisitor dosgClipCullNode.h dosg.h
 *
 * @brief a visitor which "culls" by setting the node mask based on whether
 * all of the the bounding sphere is on the wrong side ofthe plane
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class DOSGAPI dosgClipCullPlaneVisitor : public osg::NodeVisitor
{
 public:
    dosgClipCullPlaneVisitor(): 
	osg::NodeVisitor(TRAVERSE_ALL_CHILDREN) {};
	virtual void apply(osg::Transform& tx) ;
	virtual void apply(osg::Node& node) ;
	virtual void apply(osg::Geode& node) ;
	
 private:
	std::stack<osg::Matrix> _matrixStack ;
	virtual void _traverseNodeOrGeode(osg::Node& node) ;
};

#endif
