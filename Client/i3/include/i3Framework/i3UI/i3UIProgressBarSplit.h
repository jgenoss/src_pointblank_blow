#pragma once

#include "i3UICaptionControl.h"

class I3_EXPORT_FRAMEWORK i3UIProgressBarSplit : public i3UICaptionControl	
{
	I3_EXPORT_CLASS_DEFINE( i3UIProgressBarSplit, i3UICaptionControl);

protected:
	REAL32			m_rProgress = 1.0f;	//min: 0.0f, max: 1.0f
	REAL32			m_rTextUV = 0.0f;

	REAL32			m_rValue = 0.0f;
	REAL32			m_rOldValue = 0.0f;

	REAL32			m_rAnimateSlop = 1.0f;
	REAL32			m_rAnimateTime = 0.0f;

	bool			m_bReadOnly = true;
	bool			m_bDragging = false;
	bool			m_bAnimating = false;	// 애니메이션 유무

	virtual void	_UpdateShapeState(void) override;
	virtual void	UpdateRenderObjects() override;

public:
	REAL32			getProgress(void)				{	return m_rProgress;}
	void			setProgress( REAL32 progress);

	REAL32			getAnimateSlop( void)			{ return m_rAnimateSlop; }
	void			setAnimateSlop( REAL32 slop)	{ m_rAnimateSlop = slop; }

	bool			getReadOnly( void)				{	return m_bReadOnly;	}
	void			setReadOnly( bool bReadOnly)	{	m_bReadOnly = bReadOnly;	}

	bool			isAnimating( void)				{ return m_bAnimating; }
	void			setAnimate( bool bEnable)		{ m_bAnimating = bEnable; }

	void			setValue( REAL32 rValue)		{ m_rOldValue = 0.f; m_rValue = rValue; m_rAnimateTime = 0.f;}

public:
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	OnEnabled( bool bEnable) override;
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnMouseMove( UINT32 nFlag, POINT point) override;

	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	virtual INT32	OnCommand(LuaState * pState) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void	OnTDKPropertyChanged( const char* szName) override;
#endif

	static	void	RegisterLuaFunction(LuaState * pState);
};
