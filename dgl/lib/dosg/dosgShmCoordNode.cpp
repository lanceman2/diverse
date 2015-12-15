#include<dosg.h>

#define DEBUG 0

////////////////////////////////////////////////////////////////////////
// update callback- updates the matrix based on shm
class DOSGAPI dosgShmCoordUpdateCallback : public osg::NodeCallback
{
 public:
    dosgShmCoordUpdateCallback(dosgShmCoordNode *node) 
    {
	if (node->on())
	{
	    if (node->isValid() && node->getCoordShm())
	    {
		node->getCoordShm()->read(_coord) ;
	    }
	    else
	    {
		memset(_coord, 0, sizeof(_coord)) ;   
	    }
	    _updateMatrix(node, true) ;
	} 
    }

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nodeVisitor )
    {	
	dosgShmCoordNode *stNode = static_cast<dosgShmCoordNode *>(node) ;
	if (stNode->isValid() && stNode->on() && stNode->getCoordShm())
	{
	    if (!stNode->getActiveShm())
	    {
		stNode->getCoordShm()->read(_coord) ;
		_updateMatrix(stNode) ;
	    }
	    else
	    {
		stNode->getActiveShm()->read(&_active) ;
		if (_active & stNode->flag())
		{
		    stNode->getCoordShm()->read(_coord) ;
		    _updateMatrix(stNode) ;
		}
	    }
	}
	
	traverse(node, nodeVisitor) ;
    }

 private:
    // update the matrix in the node based on the XYZHPR in _coord 
    void _updateMatrix(dosgShmCoordNode *node, bool always=false) 
    {
	if (always || memcmp(_oldCoord, _coord, sizeof(_coord)))
	{
	    node->setPosition(osg::Vec3(_coord[0], _coord[1], _coord[2])) ;
	    node->setAttitude(DOSGUtil::euler_to_quat(_coord[3], _coord[4], _coord[5])) ;
	}
	memcpy(_oldCoord, _coord, sizeof(_coord)) ;
    }
    float _coord[6] ;
    float _oldCoord[6] ;
    char _active ;
};


////////////////////////////////////////////////////////////////////////
dosgShmCoordNode::dosgShmCoordNode(const char *coordName, const char *activeName, char flag) : 
    PositionAttitudeTransform(),
    _coordShm(NULL),
    _coordName(NULL),
    _activeShm(NULL),
    _activeName(NULL),
    _valid(false),
    _flag(flag),
    _on(true)
    
{

    setCoordName(coordName) ;

    setActiveName(activeName) ;

    setUpdateCallback(new dosgShmCoordUpdateCallback(this)) ;

}

////////////////////////////////////////////////////////////////////////
dosgShmCoordNode::dosgShmCoordNode(const dosgShmCoordNode& stn, 
					   const osg::CopyOp& copyop) :
    PositionAttitudeTransform(stn, copyop),
    _coordShm(stn._coordShm),
    _coordName(stn._coordName),
    _activeShm(stn._activeShm),
    _activeName(stn._activeName),
    _valid(stn._valid),
    _flag(stn._flag),
    _on(stn._on)

{
    // anything else?
}

////////////////////////////////////////////////////////////////////////
dosgShmCoordNode::~dosgShmCoordNode() 
{
}

////////////////////////////////////////////////////////////////////////
// specify a shared memory file to use by name, replacing the old file
// passing a NULL causes the node to never be updated
bool dosgShmCoordNode::setCoordName(const char *newName)
{
    // if passed a NULL, delete old shared memory
    if (!newName)
    {
	if (_coordShm) 
	{
	    delete (_coordShm) ;
	    _coordShm = NULL ;
	}
	if (_coordName)
	{
	    free(_coordName) ;
	    _coordName = NULL ;
	}
	// the memory is not valid, but the call was not an error
	_valid = true ;
	return true ;
    }

    // do nothing if same name as we already have
    if (_coordName && !strcmp(newName, _coordName)) 
    {
	return _valid ;
    }

    // try and get shared memory by name
    dtkSharedMem *newMem =  new dtkSharedMem(sizeof(float)*6, newName) ;

    if (newMem->isValid())
    {
	// free the old shm
	if (_coordShm) 
	{
	    delete (_coordShm) ;
	}
	_coordShm = newMem ;	
	// free the old name
	if (_coordName)
	{
	    free(_coordName) ;
	}
	_coordName = strdup(newName) ;
	_valid = true ;
    }
    else
    {
	_valid = false ;
    }
    return _valid ;
}

////////////////////////////////////////////////////////////////////////
// specify a shared memory file to use by name, replacing the old file
// passing a NULL causes the node to always be updated
bool dosgShmCoordNode::setActiveName(const char *newName)
{
    // if passed a NULL, delete old shared memory
    if (!newName)
    {
	if (_activeShm) 
	{
	    delete (_activeShm) ;
	    _activeShm = NULL ;
	}
	if (_activeName)
	{
	    free(_activeName) ;
	    _activeName = NULL ;
	}
	// the memory is not valid, but the call was not an error
	return true ;
    }

    // do nothing if same name as we already have
    if (_activeName && !strcmp(newName, _activeName)) 
    {
	return _valid ;
    }

    // try and get shared memory by name
    dtkSharedMem *newMem =  new dtkSharedMem(sizeof(char), newName) ;

    if (newMem->isValid())
    {
	// free the old shm
	if (_activeShm) 
	{
	    delete (_activeShm) ;
	}
	_activeShm = newMem ;	
	// free the old name
	if (_activeName)
	{
	    free(_activeName) ;
	}
	_activeName = strdup(newName) ;
	_valid = true ;
    }
    else
    {
	_valid = false ;
    }
    return _valid ;
}



