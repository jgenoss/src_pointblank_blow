// ! praptor - 미완성입니다. 작동하지 않습니다.

#include "i3GuiPCH.h"
#include "i3GuiDragSource.h"
#include "i3GuiDragDropManager.h"


I3_CLASS_INSTANCE(i3GuiDragSource);


void i3GuiDragSource::SetManager(i3GuiDragDropManager* pDragDropManager)
{
	m_pDragDropManager = pDragDropManager;
}

INT32 i3GuiDragSource::GetState(void) const
{
	return m_nState;
}

void i3GuiDragSource::Push(INT32 x, INT32 y)
{
	m_nCommand = DRAG_COMMAND_PUSH;
	m_x = x;
	m_y = y;
}

void i3GuiDragSource::Move(INT32 x, INT32 y)
{
	m_nCommand = DRAG_COMMAND_MOVE;
	m_x = x;
	m_y = y;
}

void i3GuiDragSource::Pop(INT32 x, INT32 y)
{
	m_nCommand = DRAG_COMMAND_POP;
	m_x = x;
	m_y = y;
}

void i3GuiDragSource::TargetIn(void)
{
	m_nCommand = DRAG_COMMAND_TARGET_IN;
}

void i3GuiDragSource::OnComponentUpdate(void)
{
	switch( m_nState)
	{
	case DRAG_STATE_IDLE:
		switch( m_nCommand)
		{
		case DRAG_COMMAND_PUSH:
			m_nState = DRAG_STATE_PUSHED;
			m_pDragDropManager->DragNotify( this, m_x, m_y);
			break;			
		}
		break;
	case DRAG_STATE_DRAG:
		switch( m_nCommand)
		{
		case DRAG_COMMAND_MOVE:
			m_pDragDropManager->DragNotify( this, m_x, m_y);
			break;
		}
		// fall through
	case DRAG_STATE_PUSHED:
		if( m_nState == DRAG_STATE_PUSHED)
		{			
			m_nState = DRAG_STATE_DRAG;
		}

		switch( m_nCommand)
		{
		case DRAG_COMMAND_POP:
			m_nState = DRAG_STATE_DROP;
			m_pDragDropManager->DropNotify( this, m_x, m_y);
			break;
		}
		break;
	case DRAG_STATE_DROP:
		switch( m_nCommand)
		{
		case DRAG_COMMAND_TARGET_IN:
			m_nState = DRAG_STATE_IDLE;
			break;
		default:
			m_nState = DRAG_STATE_CANCEL;
			break;
		}
		break;
	case DRAG_STATE_CANCEL:
		switch( m_nCommand)
		{
		case DRAG_COMMAND_END_CANCEL:
			m_nState = DRAG_STATE_IDLE;
			break;
		}
		break;
	}

	m_nCommand = DRAG_COMMAND_WITHOUT;
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가합니다. 2017.02.14. soon9
#endif

namespace pack
{
	struct DragSourcePD
	{
		OBJREF _orDragDropManager = 0;
		INT32 _nState = 0;
		INT32 _nCommand = 0;
		INT32 _Pad = 0;
	};
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

UINT32 i3GuiDragSource::OnSave(i3ResourceFile* pResFile)
{
	UINT32 Rc = i3GuiComponentBase::OnSave(pResFile);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not save file.");
		return STREAM_ERR;
	}

	pack::DragSourcePD pd;
	pd._orDragDropManager = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *) m_pDragDropManager);
	pd._nState = m_nState;
	pd._nCommand = m_nCommand;

	UINT32 RcSource = pResFile->Write( &pd, sizeof(pd));
	if( RcSource == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not save file.");
		return STREAM_ERR;
	}

	return Rc + RcSource;
}

UINT32 i3GuiDragSource::OnLoad(i3ResourceFile* pResFile)
{
	UINT32 Rc = i3GuiComponentBase::OnLoad(pResFile);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not load data.");
		return STREAM_ERR;
	}

	pack::DragSourcePD pd;
	UINT32 RcSource = pResFile->Read( &pd, sizeof(pd));
	if( RcSource == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not load data.");
		return STREAM_ERR;
	}

	if( pd._orDragDropManager != 0)
	{
		m_pDragDropManager = (i3GuiDragDropManager*)pResFile->FindObjectByID( pd._orDragDropManager);
		I3ASSERT(m_pDragDropManager);
	}

	m_nState = pd._nState;
	m_nCommand = pd._nCommand;

	return Rc + RcSource;
}
