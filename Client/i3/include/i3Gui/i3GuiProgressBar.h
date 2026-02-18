#ifndef __I3GUI_PROGRESSBAR_H__
#define __I3GUI_PROGRESSBAR_H__

#include "i3GuiControl.h"

enum I3GUI_PROGRESSBAR_STATE
{
	I3GUI_PROGRESSBAR_NONE	= -1,
	I3GUI_PROGRESSBAR_READY = 0,
	I3GUI_PROGRESSBAR_STARTED,
	I3GUI_PROGRESSBAR_TIMEOUT,

	MAX_I3GUI_PROGRESSBAR_STATE
};

enum I3GUI_PROGRESSBAR_TYPE
{
	I3GUI_PROGRESSBAR_LEFT_TO_RIGHT	= 0,
	I3GUI_PROGRESSBAR_RIGHT_TO_LEFT
};

// ============================================================================
//
// i3GuiProgressBar
//
// ============================================================================
class I3_EXPORT_GUI i3GuiProgressBar : public i3GuiControl
{
	I3_EXPORT_CLASS_DEFINE( i3GuiProgressBar, i3GuiControl );

protected:
	I3GUI_PROGRESSBAR_STATE	m_PrgState = I3GUI_PROGRESSBAR_READY;
	I3GUI_PROGRESSBAR_STATE	m_OldPrgState = I3GUI_PROGRESSBAR_NONE;
	REAL32					m_fTime = 0.0f;
	REAL32					m_fSpeed = 0.0f;
	INT32					m_nStaticWidth = 0;

	REAL32					m_fTimeGap = 0.0f;
	bool					m_bPause = false;
	I3GUI_PROGRESSBAR_TYPE	m_eProgressBarType = I3GUI_PROGRESSBAR_LEFT_TO_RIGHT;

public:
	i3GuiProgressBar();

	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape) override;

	void			Start(REAL32 fTime, I3GUI_PROGRESSBAR_TYPE eType = I3GUI_PROGRESSBAR_RIGHT_TO_LEFT);
	void			ResetProgressBar();
	void			SetProgressBarPause( bool bTrue);
	bool			getProgressBarPause( )					{ return m_bPause;		}

	REAL32			GetProgressBarValue();
	INT32			GetProgressBarCurWidth();

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;
};

#endif //__I3GUI_PROGRESSBAR_H__
