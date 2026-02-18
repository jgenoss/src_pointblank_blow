#ifndef __I3GUI_PROGRESSBAR_H__
#define __I3GUI_PROGRESSBAR_H__

#include "i3GuiControl.h"

/*
enum I3GUI_PROGRESSBAR_STATE
{
	I3GUI_PROGRESSBAR_NORMAL = 0,
	I3GUI_PROGRESSBAR_START,
	I3GUI_PROGRESSBAR_TIMEOUT,
	I3GUI_PROGRESSBAR_DISABLED,

	MAX_I3GUI_PROGRESSBAR_STATE
};
*/

// ============================================================================
//
// i3GuiProgressBar
//
// ============================================================================
class I3_EXPORT_GUI i3GuiProgressBar : public i3GuiControl
{
	I3_CLASS_DEFINE( i3GuiProgressBar );

protected:
	BOOL					m_bUpdated;
	REAL32					m_fTime;
	REAL32					m_fSpeed;
	INT32					m_nStaticWidth;

	REAL32					m_fTimeGap;
	REAL32					m_fGabageTime;
	BOOL					m_bPause;

public:
	i3GuiProgressBar();
	virtual ~i3GuiProgressBar();  

	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape);

	void			Start(REAL32 fTime);
	void			ResetProgressBar();
	void			SetProgressBarPause( BOOL bTrue);
	BOOL			getProgressBarPause( )					{ return m_bPause;		}

	REAL32			GetProgressBarValue();
	INT32			GetProgressBarCurWidth();

	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

#endif //__I3GUI_PROGRESSBAR_H__
