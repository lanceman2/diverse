#include <dtk.h>
#include <dgl.h>

dglAugment::dglAugment(char* Name, DGL_AUGMENT_TYPE type): dtkAugment(Name)
{
	DGL::init();
	m_active = true;
	if (type == BASE)
		DGL::addAugment(this);
	else if (type == OVERLAY)
		DGL::addOverlay(this);
	else if (type == PRENAV)
		DGL::addPreNavAugment(this);
}
dglAugment::~dglAugment()
{

}
int dglAugment::preConfig()
{
	return DTK_REMOVE_CALLBACK;
}
int dglAugment::postConfig()
{
	return DTK_REMOVE_CALLBACK;
}
int dglAugment::preFrame()
{
	return DTK_REMOVE_CALLBACK;
}
int dglAugment::postFrame()
{
	return DTK_REMOVE_CALLBACK;
}
int dglAugment::draw()
{
	return DTK_REMOVE_CALLBACK;
}
int dglAugment::initGL()
{
	return DTK_REMOVE_CALLBACK;
}
int dglAugment::setupGL()
{
	return DTK_REMOVE_CALLBACK;
}
int dglAugment::activate()
{
	m_active = true;
	return 0;
}
int dglAugment::deactivate()
{
	m_active = false;
	return 0;
}
bool dglAugment::isActive()
{
	return m_active;
}
