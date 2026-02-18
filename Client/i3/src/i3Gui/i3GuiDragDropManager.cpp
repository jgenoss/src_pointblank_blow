// ! praptor - 미완성입니다. 작동하지 않습니다.

#include "i3GuiPCH.h"
#include "i3GuiDragDropManager.h"
#include "i3GuiDropTarget.h"


I3_CLASS_INSTANCE(i3GuiDragDropManager);


void i3GuiDragDropManager::RegisterDropTarget(i3GuiDropTarget* pDropTarget)
{	
	m_listDropTarget.push_back(pDropTarget);
}

void i3GuiDragDropManager::UnregisterDropTarget(i3GuiDropTarget* pDropTarget)
{
	i3::vu::remove(m_listDropTarget, pDropTarget);
}

void i3GuiDragDropManager::DragNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y)
{	
	for(size_t i = 0; i < m_listDropTarget.size(); i++)
	{
		m_listDropTarget[i]->DragNotify(pDragSource, x, y);
	}
}

void i3GuiDragDropManager::DropNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y)
{
	for(size_t i = 0; i < m_listDropTarget.size(); i++)
	{
		m_listDropTarget[i]->DropNotify(pDragSource, x, y);
	}
}

void i3GuiDragDropManager::OnComponentUpdate()
{
	// nullptr
}

UINT32 i3GuiDragDropManager::OnSave(i3ResourceFile* pResFile)
{
	UINT32 Rc = i3GuiComponentBase::OnSave(pResFile);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not save file.");
		return STREAM_ERR;
	}

	UINT32 RcList = SaveFromListToResourceFile(m_listDropTarget, pResFile);		// m_listDropTarget.SaveTo( pResFile);
	if( RcList == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not save list.");
		return STREAM_ERR;
	}
    
	return Rc + RcList;
}

UINT32 i3GuiDragDropManager::OnLoad(i3ResourceFile* pResFile)
{
	UINT32 Rc = i3GuiComponentBase::OnLoad(pResFile);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not load data.");
		return STREAM_ERR;
	}

	UINT32 RcList = LoadFromResourceFileToList(m_listDropTarget, pResFile); // m_listDropTarget.LoadFrom( pResFile);
	if( RcList == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not load list.");
		return STREAM_ERR;
	}

	return Rc + RcList;
}
