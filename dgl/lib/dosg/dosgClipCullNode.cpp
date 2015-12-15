#include <osg/Group>
#include <osg/ClipNode>
#include <osg/ClipPlane>

#include<dosg.h>

#define DEBUG 0

////////////////////////////////////////////////////////////////////////
// update callback- calculate scenegraph transformation to the clip plane
// only needed for culling
void dosgClipCullUpdateCallback::operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor )
{
    if (dosgClipCullNode::cullType() != dosgClipCullNode::OFF)
    {
	dosgClipCullNode *ccNode = static_cast<dosgClipCullNode *>(node) ;
	if ((dosgClipCullNode::cullType() != dosgClipCullNode::OFF) && ccNode->enabled())
	{
	    // only need to compute matrix once per frame
	    _frame = DOSG::getFrameNumber() ;
	    if (_frame != _oldFrame) 
	    {
		if (DEBUG) 
		{
		    printf("\ndosgClipCullUpdateCallback: frame = %d\n",_frame) ;
		    printf("                            ccNode = %p\n",ccNode) ;
		    printf("                            num = %d\n",ccNode->num()) ;
		}
		_oldFrame = _frame ;
		
		std::vector<osg::Matrix> matList = node->getWorldMatrices() ;
		osg::Matrix mat ;
		mat.makeIdentity() ;
		for (int i = 0; i<matList.size(); i++)	\
		{
		    mat.preMult(matList[i]) ;
		}
		ccNode->matrix(mat) ;
		
		if (DEBUG) 
		{
		    dtkCoord c = DOSGUtil::getCoord(mat) ;
		    printf("  matrix = %f %f %f, %f %f %f\n", c.x, c.y, c.z, c.h, c.p, c.r) ; 
		}
	    }
	    else if (DEBUG) 
	    {
		printf("  dosgClipCullUpdateCallback: duplicate frame = %d\n", _frame) ;
	    }
	}
    }
    traverse(node, nodeVisitor) ;
}

////////////////////////////////////////////////////////////////////////
// visitor which calculates the transformation to this node
void dosgClipCullPlaneVisitor::apply(osg::Transform& tx)
{
    if (DEBUG) printf("visiting tx node %p, \"%s\"\n", &tx, tx.getName().c_str()) ;
	    
    osg::BoundingSphere bs = tx.getBound() ;
    if (DEBUG) printf("  tx's radius = %f, center = %f %f %f\n",
		      bs.radius(), bs.center().x(), bs.center().y(), bs.center().z()) ;
    
    osg::Matrix mat = DOSGUtil::getMatrix(&tx) ;
    if( _matrixStack.size() == 0 )
	_matrixStack.push(mat);
    else
	_matrixStack.push(mat*_matrixStack.top());
    if (DEBUG) {
	osg::Matrix m = _matrixStack.top() ;
	dtkCoord c = DOSGUtil::getCoord(m) ;
	printf("  pushed tx matrix = %f %f %f, %f %f %f\n", c.x, c.y, c.z, c.h, c.p, c.r) ; 
    }

    traverse(tx);
    _matrixStack.pop();
}

////////////////////////////////////////////////////////////////////////
// visitor which culls only if culling is done on the node level
void dosgClipCullPlaneVisitor::apply(osg::Node& node)
{
    if (dosgClipCullNode::cullType() == dosgClipCullNode::HIGH)
    {
	_traverseNodeOrGeode(node) ;
    }
    else
    {
	traverse(node);
    }
}

////////////////////////////////////////////////////////////////////////
// visitor which culls if culling is active
void dosgClipCullPlaneVisitor::apply(osg::Geode& geode)
{
    if (dosgClipCullNode::cullType() != dosgClipCullNode::OFF)
    {
	_traverseNodeOrGeode(geode) ;
    }
    else
    {
	traverse(geode);
    }
}

////////////////////////////////////////////////////////////////////////
// called by node visitors- checks if a bounding sphere is on the culled
// side of any of the clip/cull planes, and if so, sets the node mask so it
// isn't traversed
void dosgClipCullPlaneVisitor::_traverseNodeOrGeode(osg::Node& node)
{
    osg::BoundingSphere bs = node.getBound() ;
    if (bs.radius()>0)
    {
	
	if (DEBUG)
	{
	    printf("\nvisiting & checking node %p, \"%s\"\n", 
		   &node, node.getName().c_str()) ;
	    
	    printf("  node's radius = %f, center = %f %f %f\n",
		   bs.radius(), bs.center().x(), bs.center().y(), bs.center().z()) ;
	}
	
	// loop through all the planes
	for (int i=0; i<DOSG_CLIPCULLNODE_NUM_PLANES; i++)
	{
	    dosgClipCullNode *ccn = dosgClipCullNode::ptr(i) ;
	    if (ccn!= NULL && ccn->enabled() && 
		(dosgClipCullNode::cullType() != dosgClipCullNode::OFF))
	    {
		if (DEBUG) printf("  checking clip plane %d\n", i) ;
		
		// get matrix for this clipping plane
		osg::Matrix Cmat = ccn->matrix() ;
		if (DEBUG) {
		    dtkCoord c = DOSGUtil::getCoord(Cmat) ;
		    printf("  matrix for clipping plane = %f %f %f, %f %f %f\n", 
			   c.x, c.y, c.z, c.h, c.p, c.r) ; 
		}

		// get matrix for this node
		osg::Matrix Nmat = _matrixStack.top() ;
		Nmat.invert(Nmat) ;
		if (DEBUG) {
		    dtkCoord c = DOSGUtil::getCoord(Nmat) ;
		    printf("  matrix for node = %f %f %f, %f %f %f\n", 
			   c.x, c.y, c.z, c.h, c.p, c.r) ; 
		}

		// magically get matrix which transforms clipping plane to
		// node's coordinate system
		osg::Matrix mat = Cmat * Nmat ;

		// transform clip plane by matrix
		osg::Plane plane(0.f, -1.f, 0.f, 0.f) ;
		plane.transform(mat) ;
		
		if (DEBUG) {
		    dtkCoord c = DOSGUtil::getCoord(mat) ;
		    printf("  magic matrix = %f %f %f, %f %f %f\n", 
			   c.x, c.y, c.z, c.h, c.p, c.r) ; 
		    osg::Vec3 n = plane.getNormal() ;
		    printf("  normal = %f %f %f\n", n.x(), n.y(), n.z()) ;
		}


		float distanceFromPlane =  plane.distance(bs.center()) ;
		if (DEBUG) printf("  distance = %8.8f\n",distanceFromPlane) ; 
		
		// add in a slop factor to be sure we don't cull too aggressively
		if(distanceFromPlane < -(bs.radius()*1.05f)) 
		{
		    if (DEBUG) printf("  turning off node mask\n") ;
		    dosgClipCullNode::addCulledNode(node, node.getNodeMask()) ;
		    node.setNodeMask(0); 
		    return ;
		}
	    }
	}
    }
    traverse(node);
}


////////////////////////////////////////////////////////////////////////
// a node which clips all geometry in the +Y plane

dosgClipCullNode *dosgClipCullNode::_ptr[DOSG_CLIPCULLNODE_NUM_PLANES] = 
    {NULL, NULL, NULL, NULL, NULL, NULL} ;
unsigned int dosgClipCullNode::_howManyNodes = 0 ;
std::vector<dosgClipCullNode::nodeAndMask> dosgClipCullNode::_culledNodesAndMasks ;
bool dosgClipCullNode::_clip = true ;
dosgClipCullNode::cullTypeValue dosgClipCullNode::_cullType = dosgClipCullNode::LOW ;

dosgClipCullNode::dosgClipCullNode() : 
    Group(),
    _num(-1),
    _enabled(false)
{
    _howManyNodes++ ;
    for (int i=0; i<DOSG_CLIPCULLNODE_NUM_PLANES; i++)
    {
	if (!_ptr[i])
	{
	    _num = i ;
	    if (DEBUG) printf("dosgClipCullNode: num = %d\n",_num) ;
	    _ptr[i] = this ;
	    break ;
	}
    }
    if (_num<0)
    {
	printf("dosgClipCullNode: ERROR- no clip plane available\n") ;
	return ;
    }
    if (DEBUG) printf("using clip plane %d\n",_num) ;
    
    if (_clip)
    {
	_clipNode = new osg::ClipNode ;
	//printf("dosgClipCullNode::dosgClipCullNode(): new clip node = %p\n",_clipNode) ;
	addChild(_clipNode) ;
	
	osg::ClipPlane *cp =  new osg::ClipPlane(_num , 0.f, -1.f, 0.f, 0.f) ;
	_clipNode->addClipPlane(cp) ;

	//printf("dosgClipCullNode::dosgClipCullNode(): new clip plane = %p\n",cp) ;
	if (_enabled) enable() ;
	
    }

    setUpdateCallback(new dosgClipCullUpdateCallback) ;
}

////////////////////////////////////////////////////////////////////////
dosgClipCullNode::dosgClipCullNode(const dosgClipCullNode& ccn, const osg::CopyOp& copyop) :
    Group(ccn, copyop),
    _clipNode(ccn._clipNode), // do we need to allocate a new clipnode instead?
    _num(ccn._num),
    _enabled(ccn._enabled),
    _matrix(ccn._matrix)
{
    // I'm sure more code needs to go here! set _howMany*
}

////////////////////////////////////////////////////////////////////////
dosgClipCullNode::~dosgClipCullNode() 
{
    _howManyNodes-- ;
    disable() ;
    if (DEBUG) printf("dosgClipCullNode destructor called\n") ;
    _ptr[_num] = NULL ;
    // decrement the ref counter so it'll be deleted
    for (int i=0; i<_clipNode->referenceCount(); i++)
    {
	_clipNode->unref() ;
    }
}


////////////////////////////////////////////////////////////////////////
void dosgClipCullNode::enable() 
{ 
    if (_clip)
    {
	DOSG::getScene()->getOrCreateStateSet()->
	    setMode(GL_CLIP_PLANE0+_num, osg::StateAttribute::ON);
	// set plane coefficients
	if (_clipNode)
	{
	  //printf("dosgClipCullNode::enable(): clip node %p has %d clipping planes\n",_clipNode,_clipNode->getNumClipPlanes()) ;
	    osg::ClipPlane *cp = _clipNode->getClipPlane(0) ;
	    if (cp) 
	    {
	      //printf("enable: cp %p\n",cp) ;
		cp->setClipPlane(0.f, -1.f, 0.f, 0.f) ;
	    }
	}
    }
    
    _enabled = true ;
} 

////////////////////////////////////////////////////////////////////////
void dosgClipCullNode::disable() 
{ 
    if (_clip)
    {
	DOSG::getScene()->getOrCreateStateSet()->
	    setMode(GL_CLIP_PLANE0+_num, osg::StateAttribute::OFF);
	// set plane coefficients
	if (_clipNode)
	{
	  //printf("dosgClipCullNode::disable(): clip node %p has %d clipping planes\n",_clipNode,_clipNode->getNumClipPlanes()) ;
	    osg::ClipPlane *cp = _clipNode->getClipPlane(0) ;
	    if (cp) 
	    {
	      //printf("disable: cp %p\n",cp) ;
		cp->setClipPlane(0.f, 0.f, 0.f, 0.f) ;
	    }
	}
    }
    _enabled = false ;
} 

////////////////////////////////////////////////////////////////////////
// add a node to the list of nodes culled, so can be reset
void dosgClipCullNode::addCulledNode(osg::Node &node, osg::Node::NodeMask mask)
{
    if (DEBUG) printf("dosgClipCullNode::addCulledNode node = %p, mask = %d\n",&node, mask) ;
    dosgClipCullNode::nodeAndMask item ;
    item.node = &node ;
    item.mask = mask ;
    _culledNodesAndMasks.push_back(item) ;
}

////////////////////////////////////////////////////////////////////////
// reset all zero'd mode masks
void dosgClipCullNode::resetNodemasks()
{
    if (DEBUG) printf("dosgClipCullNode::resetNodemasks():: resetting %d node masks\n",
		      _culledNodesAndMasks.size()) ;
    dosgClipCullNode::nodeAndMask item ;
    for (int i=0; i<_culledNodesAndMasks.size(); i++)
    {
	item =  _culledNodesAndMasks[i] ;
	if (DEBUG) printf("dosgClipCullNode::resetNodemasks node = %p, mask = %d\n",
			  item.node , item.mask) ;
	item.node->setNodeMask(item.mask) ;
    }
    _culledNodesAndMasks.clear() ;
}
