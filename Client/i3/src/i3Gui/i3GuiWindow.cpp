// ============================================================================
//
// File: i3GuiWindow.cpp
//
// Desc: Graphic User Interface Components
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================
#include "i3GuiPCH.h"
#include "i3GuiControl.h"
#include "i3GuiWindow.h"

// ============================================================================
//
// i3GuiWindow
//
// ============================================================================

I3_CLASS_INSTANCE( i3GuiWindow);

void i3GuiWindow::OnUpdate(REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate(rDeltaSeconds);
}

void i3GuiWindow::SetEnable(bool bTrue)
{
	if(bTrue)
	{
		// Window가 활성화되었을때 호출합니다.
		OnActive();
	}
	else
	{
		// Window가 비활성화 되었을때 호출합니다.
		OnDisable();
	}

	i3GameNode::SetEnable(bTrue); 
}

void i3GuiWindow::UserInput(REAL32 fX, REAL32 fY, UINT32 state)
{
	i3GuiObjBase::UserInput(fX, fY, state);
}

void i3GuiWindow::OnCreate(void)
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);	
}
void i3GuiWindow::OnActive(void)
{
}
void i3GuiWindow::OnDisable(void)
{

}
void i3GuiWindow::OnChangeFocus(void)
{

}

void i3GuiWindow::OnPushControl(void)
{

}

void i3GuiWindow::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiObjBase::CopyTo( pDest, method);

	if( !i3::kind_of<i3GuiWindow*>(pDest))
		return ;

	i3GuiWindow * pWindow = (i3GuiWindow*)pDest;

	pWindow->setLayerCount( m_nLayerCount );

	pWindow->setState( m_state );
}

void	i3GuiWindow::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3GuiObjBase::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4	GUIWINDOW
	{
		INT32	m_bEnable = 0;
		INT32	m_nLayerCount = 0;
		UINT32	m_state = 0;
		INT8	m_Reserved[20] = { 0 };
	};
}

#if defined ( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32	i3GuiWindow::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3GuiObjBase::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::GUIWINDOW	data;
	data.m_nLayerCount	= m_nLayerCount;
	data.m_state		= m_state;

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3GuiWindow::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3GuiWindow::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIWINDOW data;

	Result = i3GuiObjBase::OnLoad( pResFile );
	if( Result == STREAM_ERR )
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data ));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiWindow::OnLoad()", "Could now read data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_nLayerCount	= data.m_nLayerCount;
	m_state			= data.m_state;

	// 처음에 로드한 Window의 경우에는 Focus된 Window가 없는 상태입니다.
	// 그러므로 Focus상태의 Window의 경우에는, 그 상태를 비활성화 합니다.
	if(m_state & I3GUI_WINDOW_STATE_FOCUS)
	{
		removeState(I3GUI_WINDOW_STATE_FOCUS);
	}

	// Window가 생성되어졌을때 호출합니다.
	OnCreate();

	return Result;
}

bool i3GuiWindow::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GuiObjBase::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->addAttr( "LayerCount", m_nLayerCount);
	pXML->addAttr( "State", m_state);

	return true;
}

bool i3GuiWindow::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3GuiObjBase::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->getAttr( "LayerCount", &m_nLayerCount);
	pXML->getAttr( "State", &m_state);

	// 처음에 로드한 Window의 경우에는 Focus된 Window가 없는 상태입니다.
	// 그러므로 Focus상태의 Window의 경우에는, 그 상태를 비활성화 합니다.
	// 따로 하는 일이 없이 마스크만 제거하므로 if구문 뺏습니다.
	removeState(I3GUI_WINDOW_STATE_FOCUS);

	return true;
}
