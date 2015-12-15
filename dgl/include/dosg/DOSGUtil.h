#ifndef _DOSGUTIL_H_
#define _DOSGUTIL_H_

#include<string>
#include<vector>
#include<map>

#include <dosg.h>
#include <osg/Switch>
#include <osg/Transform>
#include <osg/Geode>
#include <osgText/Text>

namespace DOSGUtil 
{
    /*!
     * @brief Compute the bounding box of a node
     *
     * Not all node classes are specifically supported- currently only Switch,
     * Geode, Transform- the rest are treated as generic Nodes.
     *
     * @param node node to use for bounding box computation
     *
     * @return the bounding box of the node
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    DOSGAPI osg::BoundingBox computeBoundingBox(osg::Node &node) ;

    /*!
     * @brief Compute an osg::Matrix from a dtkMatrix
     *
     * @param d the input matrix
     *
     * @return an equivalent osg::Matrix
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    DOSGAPI osg::Matrix getMatrix(dtkMatrix &d) ;

    /*!
     * @brief Compute a dtkCoord from an osg::Matrix
     *
     * @param o the input matrix
     *
     * @return the position and orientation in the osg::Matrix 
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    DOSGAPI dtkCoord getCoord(osg::Matrix &o) ;

    /*!
     * @brief Compute a dtkMatrix from an osg::Matrix
     *
     * @param o the input matrix
     *
     * @return the equivalent dtkMatrix
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    DOSGAPI dtkMatrix getMatrix(osg::Matrix &o) ;

    /*!
     * @brief Compute the osg::Matrix in a osg::Transform node
     *
     * @param t the node
     *
     * @return the osg::Matrix in the node
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    DOSGAPI osg::Matrix getMatrix(osg::Transform *t) ;

    /*!
     * @brief Print the contents of an osg::Matrix
     *
     * @param m the matrix to print
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    DOSGAPI void printMatrix(osg::Matrix m) ;

    /*!
     * @brief Convert an Euler angle to a osg::Quat
     *
     * @param h heading of the Euler angle
     *
     * @param p pitch of the Euler angle
     *
     * @param r roll of the Euler angle
     *
     * @return the quaternion computed from the Euler angle
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    DOSGAPI osg::Quat euler_to_quat(float h, float p, float r) ;

    /*!
     * @brief Convert a world coordinate to a dgl coordiate
     *
     * @param w a coordinate in world coordinates
     *
     * @return the coordinate converted to a dgl coordinate
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    DOSGAPI dtkCoord world_to_dgl(const dtkCoord &w) ;

    /*!
     * @brief Convert a dgl coordinate to a world coordiate
     *
     * @param d a coordinate in dgl coordinates
     *
     * @return the coordinate converted to a world coordinate
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    DOSGAPI dtkCoord dgl_to_world(const dtkCoord &d) ;

    /*!
     * @class ComputeBBVisitor DOSGUtil.h dosg.h
     *
     * @brief Node Visitor to compute a bounding box
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    class DOSGAPI ComputeBBVisitor : public osg::NodeVisitor
    {
    public :
	/*!
	 * Create a node visitor, seeded with a matrix value
	 *
	 * @param mat the seed matrix
	 */
        ComputeBBVisitor(const osg::Matrix &mat) : 
	    osg::NodeVisitor( TRAVERSE_ALL_CHILDREN )
        {
	    m_curMatrix = mat;
        }
      
	/*!
	 *
	 * @brief Execute if a osg::Switch node
	 *
	 * @param node the node to visit
	 */
	virtual void apply( osg::Switch &node )
	{
	    for (unsigned int i=0; i<node.getNumChildren(); i++)
	    {
		if (node.getValue(i))
		{
		    traverse(*(node.getChild(i)));
		}
	    }
	}
	
	/*!
	 *
	 * @brief Execute if a osg::Node node
	 *
	 * @param node the node to visit
	 */
	virtual void apply( osg::Node &node )
	{
	    traverse(node);
	}
	
	/*!
	 *
	 * @brief Execute if a osg::Geode node
	 *
	 * @param geode the node to visit
	 */
	virtual void apply( osg::Geode &geode )
	{
	    for(unsigned int i = 0; i < geode.getNumDrawables(); i++) 
	    {
		osgText::Text *t = dynamic_cast<osgText::Text*>(geode.getDrawable(i)) ; 
		if (!t || !t->getAutoRotateToScreen())
		{
		    osg::BoundingBox bb = geode.getDrawable(i)->getBound();
		    m_bb.expandBy(bb.corner(0)*m_curMatrix);
		    m_bb.expandBy(bb.corner(1)*m_curMatrix);
		    m_bb.expandBy(bb.corner(2)*m_curMatrix);
		    m_bb.expandBy(bb.corner(3)*m_curMatrix);
		    m_bb.expandBy(bb.corner(4)*m_curMatrix);
		    m_bb.expandBy(bb.corner(5)*m_curMatrix);
		    m_bb.expandBy(bb.corner(6)*m_curMatrix);
		    m_bb.expandBy(bb.corner(7)*m_curMatrix);
		    traverse(geode);
		}
		else
		{
		    dtkMsg.add(DTKMSG_WARNING, 1, "ComputeBBVisitor:: skipping billboard text \"%s\"\n",
			       ((t->getText()).createUTF8EncodedString()).c_str() );
		}
	    }
	}
	
	/*!
	 *
	 * @brief Execute if a osg::Transform node
	 *
	 * @param node the node to visit
	 */
	virtual void apply(osg::Transform& node)
	{
	    osg::Matrix matrix;
	    node.computeLocalToWorldMatrix(matrix,this);
	    
	    osg::Matrix prevMatrix = m_curMatrix;
	    m_curMatrix.preMult(matrix);
	    
	    traverse(node);
	    
	    m_curMatrix = prevMatrix;
	}
        
	/*!
	 * @brief Return the bounding box
	 *
	 * @return the bounding box
	 */
	osg::BoundingBox getBound()
	{
	    return m_bb;
	}
	
    private :
	osg::BoundingBox m_bb;
	osg::Matrix m_curMatrix;
    };
  
    ////////////////////////////////////////////////////////////////////////
    /*!
     * @class nodeList DOSGUtil.h dosg.h
     *
     * @brief magaes a map of label/node pointer pairs
     *
     * A global list of names and node pointers.  Useful for quickly finding
     * nodes by name.  The standard scenegraph nodes are automatically put
     * in this map, as are the files loaded bu dosg-fly
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    class DOSGAPI nodeList 
    {
    public:
	/*!
	 * @brief Return a copy of the "node map" of the scene graph
	 *
	 * @return the "node map" of the scene graph
	 */
	static map<string, osg::Node*> getMap()
	{ 
	    return m_nodeMap; 
	}
      
	/*!
	 * @brief Return a pointer to the "node map" of the scene graph
	 *
	 * @return a pointer to the "node map" of the scene graph
	 */
	static map<string, osg::Node*> *getMapPtr()
	{ 
	    return &m_nodeMap; 
	}
      
	/*!
	 *
	 * @brief Add a label/node pointer pair to the map
	 *
	 * @param label new label
	 *
	 * @param node pointer to be paired with label
	 *
	 @ @return true on success
	 */
	static bool add(string label, osg::Node *node)
	{
	    return m_nodeMap.insert(std::make_pair(label, node)).second ;
	}

	/*!
	 * @brief Return the node pointer from DOSGUtil::getNodeMap() which
	 * has the specified name, or NULL if none found
	 *
	 * @param name to match
	 *
	 * @return the node pointer from which has the specified name, or
	 * NULL if none found
	 */
	static osg::Node* get(string name) 
	{ 
	    std::map<std::string, osg::Node*>::iterator pos ;
	    pos = m_nodeMap.find(name) ;
	    if (pos == m_nodeMap.end())
	    {
		return NULL ;
	    }
	    else
	    {
		return pos->second ;
	    }
	}

	/*!
	 * @brief Return a vector of node pointers for all nodes from
	 * DOSGUtil::getNodeMap()
	 *
	 * @return a vector of node pointers for all nodes in scenegraph
	 */
	static vector<osg::Node*> getVector(void) 
	{ 
	    vector<osg::Node*> vec ;
	    std::map<std::string, osg::Node*>::iterator pos ;
	    for (pos=m_nodeMap.begin() ; pos != m_nodeMap.end(); ++pos)
	    {
		vec.push_back(pos->second) ;
	    }
	    return vec ; 
	}
      
    private:
	static map<string, osg::Node*>              m_nodeMap ;
    } ;


    ////////////////////////////////////////////////////////////////////////

    /*!
     * @class getNamedNodes DOSGUtil.h dosg.h
     *
     * @brief returns a vector or map of all nodes in the scenegraph
     *
     * @author John Kelso, kelso@nist.gov
     * @author Andrew A. Ray, anray2@vt.edu
     */
    class DOSGAPI getNamedNodes : public osg::NodeVisitor
    {
    public:
      
	/*!
	 * Create a node visitor.  Need to apply() manually
	 */
	getNamedNodes(void): 
	    osg::NodeVisitor(TRAVERSE_ALL_CHILDREN) {setNodeMaskOverride(~0) ; m_matchName = false ; m_stopAtFirst = false ; }
      
	/*!
	 * Create and apply a node visitor starting at a specified node
	 *
	 * @param node the node to start the traversal
	 */
	getNamedNodes(osg::Node *node): 
	    osg::NodeVisitor(TRAVERSE_ALL_CHILDREN) {setNodeMaskOverride(~0) ; m_matchName = false ; m_stopAtFirst = false ; apply(*node);}
	    
	/*!
	 * Create a node visitor starting at a specified node, matching only
	 * nodes with a specified name
	 *
	 * @param node the node to start the traversal
	 *
	 * @param name to match
	 *
	 * @param  stopAtFirst stop at first match if true
	 */
	getNamedNodes(osg::Node *node, std::string name, bool stopAtFirst = false): 
	    osg::NodeVisitor(TRAVERSE_ALL_CHILDREN) {setNodeMaskOverride(~0) ; m_matchName = true ; m_stopAtFirst = stopAtFirst; m_name = name; apply(*node);}
	    
	/*!
	 * Create and apply a node visitor matching only nodes with a
	 * specified name. Need to apply() manually
	 *
	 * @param name to match
	 *
	 * @param  stopAtFirst stop at first match if true
	 */
	getNamedNodes(std::string name, bool stopAtFirst = false): 
	    osg::NodeVisitor(TRAVERSE_ALL_CHILDREN) {setNodeMaskOverride(~0) ; m_matchName = true ; m_stopAtFirst = stopAtFirst ; m_name = name; }
	    
	    
	virtual void apply(osg::Node &node)
	{ 
	    std::string name = node.getName();
	    if( m_matchName == false || m_name == name )
	    {
		m_map.insert(std::make_pair(name, &node)) ;
		m_vector.push_back(&node) ;
	    }
	    
	    if (m_stopAtFirst && m_vector.size()>0)
		return ;
	    
	    // Must traverse the node, or traversal will stop here
	    traverse(node);
	}
	
	std::map<std::string, osg::Node*> getMap(void)
	{
	    return m_map ;
	}
	
	std::vector<osg::Node*> getVector(void)
	{
	    return m_vector ;
	}
	
	osg::Node* getFirst(void)
	{
	    if (m_vector.size()>0)
		return m_vector[0] ;
	    else
		return NULL ;
	}
	
    private:
	std::map<std::string, osg::Node*> m_map ;
	std::vector<osg::Node*> m_vector ;
	std::string m_name ;
	bool m_matchName ;
	bool m_stopAtFirst ;
    };
    
    
}
#endif    



