// ! praptor - 미완성입니다. 작동하지 않습니다.

#include "i3GuiPCH.h"
#include "i3GuiDropTarget.h"


I3_CLASS_INSTANCE(i3GuiDropTarget);

void i3GuiDropTarget::SetManager(i3GuiDragDropManager* pDragDropManager)
{
	m_pDragDropManager = pDragDropManager;
}

void i3GuiDropTarget::SetTargetArea(INT32 x, INT32 y, UINT32 w, UINT32 h)
{
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;
}

INT32 i3GuiDropTarget::GetState(void) const
{
	return m_nState;
}

void i3GuiDropTarget::DragNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y)
{
	if( _IsInTargetArea(x, y) == true)
	{
		m_nCommand = DROP_COMMAND_DRAG_ON;
		m_pDragSource = pDragSource;

		OnComponentUpdate();
	}
}

void i3GuiDropTarget::DropNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y)
{
	if( _IsInTargetArea(x, y) == true)
	{
		m_nCommand = DROP_COMMAND_DROP_ON;
		m_pDragSource = pDragSource;

		OnComponentUpdate();
	}
}

void i3GuiDropTarget::Block(void)
{
	m_nCommand = DROP_COMMAND_BLOCK;

	OnComponentUpdate();
}

void i3GuiDropTarget::EndDrop(void)
{
	m_nCommand = DROP_COMMAND_END_DROP;

	OnComponentUpdate();
}

void i3GuiDropTarget::OnComponentUpdate(void)
{
	switch( m_nState)
	{
	case DROP_STATE_IDLE:
		switch( m_nCommand)
		{
		case DROP_COMMAND_DRAG_ON:
			m_nState = DROP_STATE_SELECT;
			break;
		case DROP_COMMAND_BLOCK:
			m_nState = DROP_STATE_BLOCK;
			break;		
		}
		break;
	case DROP_STATE_SELECT:
		switch( m_nCommand)
		{
		case DROP_COMMAND_DROP_ON:
			m_nState = DROP_STATE_DROP;
			break;
		default:
			m_nState = DROP_STATE_IDLE;
			m_pDragSource = nullptr;
			break;
		}
		break;
	case DROP_STATE_DROP:
		switch( m_nCommand)
		{
		case DROP_COMMAND_END_DROP:
			m_nState = DROP_STATE_IDLE;
			m_pDragSource = nullptr;
			break;
		}
		break;
	case DROP_STATE_BLOCK:
		switch( m_nCommand)
		{
		case DROP_COMMAND_UNBLOCK:
			m_nState = DROP_STATE_IDLE;
			break;
		}
		break;
	}

	m_nCommand = DROP_COMMAND_WITHOUT;	
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가합니다. 2017.02.14. soon9
#endif

namespace pack
{
	struct DropTargetPD
	{
		OBJREF _orDragDropManager = 0;
		OBJREF _orDragSource = 0;
		INT32 _nState = 0;
		INT32 _nCommand = 0;
		INT32 _x = 0;
		INT32 _y = 0;
		INT32 _w = 0;
		INT32 _h = 0;
	};
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

UINT32 i3GuiDropTarget::OnSave(i3ResourceFile* pResFile)
{
	UINT32 Rc = i3GuiComponentBase::OnSave(pResFile);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not save file.");
		return STREAM_ERR;
	}

	pack::DropTargetPD pd;
	pd._orDragDropManager = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *) m_pDragDropManager);
	pd._orDragSource = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *) m_pDragSource);
	pd._nState = m_nState;
	pd._nCommand = m_nCommand;
	pd._x = m_x;
	pd._y = m_y;
	pd._w = m_w;
	pd._h = m_h;

	UINT32 RcTarget = pResFile->Write( &pd, sizeof(pd));
	if( RcTarget == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not save file.");
		return STREAM_ERR;
	}

	return Rc + RcTarget;	
}

UINT32 i3GuiDropTarget::OnLoad(i3ResourceFile* pResFile)
{
	UINT32 Rc = i3GuiComponentBase::OnLoad(pResFile);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not read data.");
		return STREAM_ERR;
	}

	pack::DropTargetPD pd;
	UINT32 RcTarget = pResFile->Read( &pd, sizeof(pd));
	if( RcTarget == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not read data.");
		return STREAM_ERR;
	}

	if( pd._orDragDropManager != 0)
	{
        m_pDragDropManager = (i3GuiDragDropManager*)pResFile->FindObjectByID( pd._orDragDropManager);
		I3ASSERT(m_pDragDropManager);
	}

	if( pd._orDragSource != 0)
	{
        m_pDragSource = (i3GuiDragSource*)pResFile->FindObjectByID( pd._orDragSource);
		I3ASSERT(m_pDragSource);
	}

	m_nState = pd._nState;
	m_nCommand = pd._nCommand;
	m_x = pd._x;
	m_y = pd._y;
	m_w = pd._w;
	m_h = pd._h;

	return Rc + RcTarget;
}

bool i3GuiDropTarget::_IsInTargetArea(INT32 x, INT32 y)
{
	if( m_w == 0 || m_h == 0)
	{
		return false;
	}

	if( x < m_x || x > m_x + (INT32)m_w - 1)
	{
		return false;
	}

	if( y < m_y || y > m_y + (INT32)m_h - 1)
	{
		return false;
	}

	return true;
}
