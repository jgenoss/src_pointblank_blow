#ifndef __I3UISCENE_H__
#define __I3UISCENE_H__

#include "i3UIDefines.h"
#include "i3UIControl.h"

class i3UIToolTip;

////////////////////////////////////////////////////////////////////////////////////////////
// 설명
////////////////////////////////////////////////////////////////////////////////////////////

// UI의 단위. 이 단위로 start up script가 실행이된다. 에디터에서는 이 단위로 편집을 한다.
// UIManager에서 Load UI를 통해 읽어들이면, 다 읽은 후 바로 RunScript를 실행.
// UIEditor를 통해 읽어들이면, 편집 상태에 따라 조절.

// UIManager는 Rendering용 Virtual Texture( Cache가 준비된)를 1개만, 항상 가진다.
// ( Reset가능, 소멸자에서 close)
// UIManager는 전체 Scene에서 사용할 통합 Library를 항상 갖는다.
// ( Reset가능, 소멸자에서 close)
// Editor에서 편집 단위당으로 조절 가능하도록하기 위함.
class I3_EXPORT_FRAMEWORK i3UIScene : public i3GameNode
{
	I3_EXPORT_CLASS_DEFINE( i3UIScene, i3GameNode);

protected:
	INT32					m_nWidth = -1;
	INT32					m_nHeight = -1;

	INT32					m_Depth = 0;		// Scene간의 Depth default 0

	i3::rc_string			m_strStartupScript;
	LuaState		*		m_pLuaContext = nullptr;

	i3UIToolTip*			m_pDefToolTip = nullptr;

	bool					m_bLoadedBuffer = false;

	virtual	void			_ForceEnable( bool bFlag) override;

	// volatile
	bool					m_bPreloadingEnable = false;
	bool					m_consoleScene = false;
	bool					m_manualDraw = false;

public:
	i3UIScene(void);
	virtual ~i3UIScene(void);

	static void				PreRegisterLuaFunction(const char* szFunName, LuaFunction Fun);

public:
	void					setScriptFile( const char * pszFileName);
	const char*				getScriptFile(void) const;

	INT32					getWidth(void)	{	return m_nWidth;}
	INT32					getHeight(void)	{	return m_nHeight;}

	void					setWidth(INT32 width)		{	m_nWidth = width;}
	void					setHeight( INT32 height)	{	m_nHeight = height;}

	void					setDepth( INT32 depth)		{ m_Depth = depth; }
	INT32					getDepth( void)				{ return m_Depth; }

public:
	void					OpenLuaContext(void);
	void					CloseLuaContext(void);
	LuaState		*		getLuaContext(void)						{	return m_pLuaContext;}

	bool					StartGUI(const char *pFilenameIn);
	void					ResetUI(void);

	void					OnEvent( INT32 code, INT32 arg1 = 0, INT32 arg2 = 0, INT32 arg3 = 0);
	void					OnSendEvent( INT32 code, i3UIControl * pCtrl, INT32 arg2 = 0, INT32 arg3 = 0, INT32 arg4 = 0);
	void					OnSendNotify( INT32 code, i3UIControl * pRecv, i3UIControl * pFrom, INT32 arg1 = 0, INT32 arg2 = 0);

public:
	void					AddControl( i3UIControl * pCtrl, bool bAddGg = true);
	bool					Attach( bool bRunScript, HINSTANCE inst = nullptr);
	void					Detach(void);

	void					EnableScene( bool bEnable);

	void					ShowTop( void);

	void					RenderCtrl(void);				// UITool의 Edit Mode에서 Rendering 시키기 위한 Interface function

	virtual	void			OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void			OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void			OnDraw( i3UIRenderer * pRenderer);
	
#if defined( I3_DEBUG)
	virtual void			GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
	virtual void			OnTDKPropertyChanged( const char* szName) override;
#endif

	i3UIToolTip*			getDefToolTip( void)					{	return m_pDefToolTip;	}
	void					setDefToolTip( i3UIToolTip* pToolTip)	{	m_pDefToolTip = pToolTip;	}

	static	void			RegisterLuaFunction(LuaState * pState);


	// Undo System
	void					CreateControl( i3UIControl* pControl);
	void					DeleteControl( i3UIControl* pControl);

	virtual bool			IsConsoleScene() const { return m_consoleScene; }
	virtual void			SetConsoleScene(bool consoleScene) { m_consoleScene = consoleScene; }

	virtual bool			IsManualDraw() { return false; } // ms_callbackPostDraw 등에서 직접그리는씬
	virtual void			SetManualDraw(bool manualDraw) { m_manualDraw = manualDraw; }

private:
	void					SetEnable() { I3ASSERT_0; }		// 아예 SetEnable 함수 못쓰게 막는다...

	/////////////////////////////////////////////////
	// Test Automation
public:
	void					TA_GetCtrlPath(char * pszPath, INT32 limit);
	i3UIControl *			TA_FindCtrlByPath(const char * pszPath);
	i3UIControl *			TA_ScanText( const wchar_t * pszText, bool bExact);

};

#endif