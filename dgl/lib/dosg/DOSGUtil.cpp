// for utility functions that use OSG
// John Kelso, kelso@nist.gov, 4/06

#include <string>

#include <dosg.h>
#include <osg/Transform>
#include <osg/Version>

namespace DOSGUtil {

    osg::Matrix getMatrix(osg::Transform *t) 
    {
	osg::NodeVisitor nv = osg::NodeVisitor() ; 
	osg::Matrix mat ;
	t->computeLocalToWorldMatrix(mat,&nv);
	return mat ;
    }

    osg::BoundingBox computeBoundingBox( osg::Node &node ) 
    {
	osg::Matrix mat ;
#if 0
	osg::Transform *t ;
	if ((t=node.asTransform())) 
	{
	    osg::NodeVisitor nv = osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) ;
	    t->computeLocalToWorldMatrix(mat,&nv);
	}
      
	else
	    mat.makeIdentity() ;
#else
	mat.makeIdentity() ;
#endif
	DOSGUtil::ComputeBBVisitor foo(mat) ;
	node.accept(foo) ;
	return foo.getBound() ;
    }
 
    osg::Matrix getMatrix(dtkMatrix &d)
    {
	dtkVec3 t ;
	d.translate(&t) ;
	float x, y, z, w ;
	d.quat(&x, &y, &z, &w) ;
	osg::Matrix o ;
#if 1
	//printf("scale first\n") ;
	o = osg::Matrix::scale(osg::Vec3(d.scale(),d.scale(),d.scale())) *
	    osg::Matrix::rotate(osg::Quat(x, y, z, w)) *
	    osg::Matrix::translate(osg::Vec3(t.x, t.y, t.z)) ;
#endif
#if 0
	//printf("scale last\n") ;
	o = osg::Matrix::translate(osg::Vec3(t.x, t.y, t.z)) *
	    osg::Matrix::rotate(osg::Quat(x, y, z, w)) *	
	    osg::Matrix::scale(osg::Vec3(d.scale(),d.scale(),d.scale())) ;
#endif
#if 0
	//printf("original buggy way\n") ;
	o.makeScale(d.scale(),d.scale(),d.scale()) ;
	o.setRotate(osg::Quat(x, y, z, w)) ;
	o.setTrans(t.x, t.y, t.z) ;
#endif
	return o ;
    }

    dtkMatrix getMatrix(osg::Matrix &o)
    {
	dtkMatrix m ;
	if (o(0,3) != 0.f || o(1,3) != 0.f || o(2,3) != 0.f || o(3,3) != 1.f) 
	{
	    dtkMsg.add(DTKMSG_ERROR, 1,
		       "DOSGUtil::getCoord(): OSG matrix can not be converted to DTK matrix- not affine!\n") ;
	    dtkMsg.add(DTKMSG_ERROR, 1,
		       "     o(0,3) = %f, o(1,3) = %f, o(2,3) = %f, o(3,3) = %f\n",
		       o(0,3), o(1,3), o(2,3), o(3,3)) ;
	}
	else
	{
	    osg::Vec3 s, tv ;
	    osg::Quat q, so ;
#if OSG_VERSION_MAJOR < 2
	    tv = o.getTrans() ;
	    s = o.getScale() ;
	    q = o.getRotate() ;
#else
	    o.decompose(tv, q, s, so) ;
#endif
#define SCALETOLERANCE (.0001)
	    if (fabs(s.x()-s.y())>SCALETOLERANCE ||
		fabs(s.x()-s.z())>SCALETOLERANCE ||
		fabs(s.y()-s.y())>SCALETOLERANCE)
		dtkMsg.add(DTKMSG_WARNING, 1,
			   "non-uniform scaling, %f %f %f, using X scale\n",s.x(),s.y(),s.z()) ;
	    m.scale(s.x()) ;
	    m.quat(q.x(), q.y(), q.z(), q.w()) ;
	    m.translate(tv.x(), tv.y(), tv.z()) ;
	}
	return m ;
    }

    dtkCoord getCoord(osg::Matrix &o)
    {
	dtkMatrix m = getMatrix(o) ;
	dtkCoord d ;
	m.coord(&d) ;
	return d ;
    }

    void printMatrix(osg::Matrix m)
    {
	for (int r=0; r<4; r++) {
	    for (int c=0; c<4; c++) {
		printf("%+8.8f  ",m(r,c)) ;
	    }
	    printf("\n") ;
	}
    }

    osg::Quat euler_to_quat(float h, float p, float r) 
    {
	dtkMatrix mat ;
	mat.rotateHPR(h, p, r) ;
	float x, y, z, w ;
	mat.quat(&x, &y, &z, &w) ;
	return osg::Quat(x, y, z, w) ;
    }

    dtkCoord world_to_dgl(const dtkCoord &w)
    {
	// get the transformation from world to dgl
	osg::Matrix n = DOSGUtil::getMatrix(DOSG::getNav()) ;
	osg::Matrix e = DOSGUtil::getMatrix(DOSG::getEther()) ;
	e = e*n ;
	dtkMatrix t = DOSGUtil::getMatrix(e) ;

	// convert world to a matrix and multiply by transformation
	dtkMatrix wm(w) ;
	wm.mult(&t) ;

	// get coord out of matrix
	dtkCoord d ;
	wm.coord(&d) ;
    
	return d ;

    }

    dtkCoord dgl_to_world(const dtkCoord &d)
    {

	// get the transformation from world to dgl
	osg::Matrix n = DOSGUtil::getMatrix(DOSG::getNav()) ;
	osg::Matrix e = DOSGUtil::getMatrix(DOSG::getEther()) ;
	e = e*n ;
	dtkMatrix t = DOSGUtil::getMatrix(e) ;

	// now flip it!
	t.invert() ;

	// convert dgl to a matrix and multiply by transformation
	dtkMatrix dm(d) ;
	dm.mult(&t) ;

	// get coord out of matrix
	dtkCoord w ;
	dm.coord(&w) ;
    
	return w ;

    }

    map<string, osg::Node*> DOSGUtil::nodeList::m_nodeMap ;
}
