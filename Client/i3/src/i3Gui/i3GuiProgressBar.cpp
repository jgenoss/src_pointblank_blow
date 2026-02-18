#include "i3GuiPCH.h"
#include "i3GuiProgressBar.h"

// ============================================================================
//
// i3GuiProgressBar
//
// ============================================================================
I3_CLASS_INSTANCE( i3GuiProgressBar);

i3GuiProgressBar::i3GuiProgressBar()
{
	setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

void i3GuiProgressBar::SetControlShapeTemplate(i3GuiTemplateElement* pShape)
{
	i3GuiControl::SetControlShapeTemplate( pShape);

	//	
	m_nStaticWidth = (INT32) m_pData->m_pShape->getImage(0)->m_nWidth;
}

void i3GuiProgressBar::Start(REAL32 fTime, I3GUI_PROGRESSBAR_TYPE eType)
{
	m_PrgState = I3GUI_PROGRESSBAR_STARTED;

	m_fTime = fTime;

	m_fSpeed = i3Math::div( ((REAL32)m_pData->m_pShape->getImage(0)->m_nWidth), m_fTime);	

	m_eProgressBarType = eType;
}

void i3GuiProgressBar::ResetProgressBar()
{
	m_fTime = 0.0f;
	m_fSpeed = 0.0f;
	m_fTimeGap = 0.0f;
	m_pData->m_pShape->getImage(0)->m_nWidth = (REAL32) m_nStaticWidth;
	m_PrgState = I3GUI_PROGRESSBAR_READY;
}

void i3GuiProgressBar::SetProgressBarPause( bool bTrue)
{
	m_bPause = bTrue;
}

REAL32 i3GuiProgressBar::GetProgressBarValue()
{
	REAL32 fVal = 0.0025f * m_nStaticWidth;

	return (REAL32)m_pData->m_pShape->getImage(0)->m_nWidth * fVal;
}

INT32 i3GuiProgressBar::GetProgressBarCurWidth()
{
	return (INT32) m_pData->m_pShape->getImage(0)->m_nWidth;
}

// °»½Å
void i3GuiProgressBar::OnUpdate( REAL32 rDeltaSeconds )
{
	i3GuiControl::OnUpdate( rDeltaSeconds ); 

	bool bFirst = (m_OldPrgState != m_PrgState);
	m_OldPrgState = m_PrgState;

	switch( m_PrgState)
	{
	case I3GUI_PROGRESSBAR_READY:
		if( bFirst)
			ResetEvent();
		break;

	case I3GUI_PROGRESSBAR_STARTED:
		{
			if( !m_bPause)
			{
				m_fTimeGap += rDeltaSeconds;
			}

			if( m_fTimeGap > m_fTime)
			{
				m_fTimeGap = m_fTime;
				m_PrgState = I3GUI_PROGRESSBAR_TIMEOUT;
			}
			if(m_eProgressBarType == I3GUI_PROGRESSBAR_RIGHT_TO_LEFT) {
				m_pData->m_pShape->getImage(0)->m_nWidth = m_nStaticWidth * (1.0f - (m_fTimeGap / m_fTime));
			}
			else if(m_eProgressBarType == I3GUI_PROGRESSBAR_LEFT_TO_RIGHT) {
				m_pData->m_pShape->getImage(0)->m_nWidth = m_nStaticWidth*(m_fTimeGap / m_fTime);
			}
			else {
				I3ASSERT_0;
			}
		}
		break;
	case I3GUI_PROGRESSBAR_TIMEOUT:
		break;
	}

	UpdateRenderObjects();
}

void i3GuiProgressBar::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GuiControl::CopyTo( pDest, method );

	if( !i3::kind_of<i3GuiProgressBar*>(pDest))
		return ;

	i3GuiProgressBar * pProgressBar = (i3GuiProgressBar*)pDest;

	pProgressBar->setControlState( m_ControlState );
}

void i3GuiProgressBar::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3GuiControl::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUIPROGRESSBAR
	{
		UINT32	m_State = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiProgressBar::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIPROGRESSBAR data;

	Result = i3GuiControl::OnSave( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_State = m_ControlState;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiProgressBar::OnSave()", "Could not Save data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiProgressBar::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUIPROGRESSBAR data;

	Result = i3GuiControl::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiProgressBar::OnLoad()","Could not load data");
		return STREAM_ERR;
	}
	Result += Rc;

	m_ControlState = (GUI_CONTROL_STATE)data.m_State;

	return Result;
}
