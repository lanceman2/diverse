#include <dtk.h>
#include <dgl.h>

void DGLScreen::frame()
{
	for (unsigned int i=0; i<m_viewport.size(); i++)
	{
		m_viewport[i]->frame();
	}
}
void DGLScreen::clear()
{
	for (unsigned int i=0; i<m_viewport.size(); i++)
	{
		m_viewport[i]->clear();
	}
};

