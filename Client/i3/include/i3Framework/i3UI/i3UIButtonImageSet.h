#ifndef __I3UI_BUTTONIMAGESET_H__
#define __I3UI_BUTTONIMAGESET_H__

#include "i3UIControl.h"
#include "i3UIImageBox.h"

class I3_EXPORT_FRAMEWORK i3UIButtonImageSet : public i3UICaptionControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIButtonImageSet, i3UICaptionControl );
protected:
	UINT32			m_CurShape = 0;
	REAL32			m_PassedTime = 0.0f;

	bool			m_bViewOnly = false;

	i3UIImageBox*	m_pIBOver = nullptr;
	i3UIImageBox*	m_pIBDown = nullptr;
	i3UIImageBox*	m_pIBDisabled = nullptr;
	
	virtual void	_UpdateShapeState(void) override;
	virtual bool	_PrepareTexForSprite() override;

//Lua glue È£Ãâ¿ë
public:
	void			SetShapeIdx( INT32 nIdx);
	void			SetShapeIdxByString( char * pszName);
	INT32			GetShapeIdx( void);
	const char *	GetShapeString( void);

	void			SetViewOnly( bool bViewOnly) { m_bViewOnly = bViewOnly; }
	bool			IsViewOnly( void) { return m_bViewOnly; }

public:
	i3UIButtonImageSet();
	virtual ~i3UIButtonImageSet(); 

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	OnBindTemplate(void) override;
	virtual void	OnEnabled( bool bEnable) override;

	virtual void	OnVisibleScene( void) override;
	virtual void	OnInvisibleScene( void) override;

	virtual void	OnDraw( i3UIRenderer * pRenderer) override;

	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse(void) override;
	virtual bool	OnKillFocus( i3UIControl * pByCtrl = nullptr) override;
	virtual bool	OnSetFocus() override;

	virtual void	OnKeyDown( UINT32 nKey) override;
	virtual void	OnKeyUp( UINT32 nKey) override;

	virtual void	OnMove( REAL32 x, REAL32 y) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;

	virtual i3UIControl*	OnSelect( void) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method ) override;

	virtual void	Init( void) override;

	virtual INT32	OnCommand(LuaState * pState) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
#endif

	static	void	RegisterLuaFunction(LuaState * pState);
};

#endif //__I3UI_BUTTON_H__
