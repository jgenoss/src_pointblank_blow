#ifndef __I3UI_MESSAGEBOX_H__
#define __I3UI_MESSAGEBOX_H__

#include "i3UIFrameWnd.h"
#include "i3UIStaticText.h"

enum	I3UI_MESSAGEBOX_TYPE
{
	I3UI_MBTYPE_TIMEOUT = -1,		// TIME OUT. 지정된 시간이나 조건이 되면 자동으로 사라짐.
	I3UI_MBTYPE_OK = 0,			// 확인
	I3UI_MBTYPE_YESNO,			// 예, 아니오
	I3UI_MBTYPE_YESNOCANCEL,	// 예, 아니오, 취소		
	

	I3UI_MBTYPE_COUNT,
};

// 하위 Ctrl로 ComboBox는 사용할 수 없습니다.
// ComboBox의 이벤트를 MessageBox가 받을 수 없음. (i3::same_of<i3UIFrameWnd*>로 처리하기때문에 MessageBox가 호출되지 않음)
class I3_EXPORT_FRAMEWORK i3UIMessageBox : public i3UIFrameWnd
{
	I3_EXPORT_CLASS_DEFINE( i3UIMessageBox, i3UIFrameWnd);

protected:
	VEC2D					m_vMinSize = { 250.0f, 100.0f };
	VEC2D					m_vMaxSize = { 500.0f, 500.0f };
	VEC2D					m_vMINBtnSize = { 70.0f, 20.0f };
	
	REAL32					m_topSpace = 10.0f;
	REAL32					m_bottomSpace = 10.0f;	// 버튼의 아랫쪽 공간( template마다 texture의 uv가 다르기 때문에 어울리는 모양을 위해서는 손수 지정해줘야 하는부분이 있다.)
	REAL32					m_leftSpace = 10.0f;
	REAL32					m_rightSpace = 10.0f;
	REAL32					m_innerSpace = 10.0f;
	

	I3UI_MESSAGEBOX_TYPE	m_Type = I3UI_MBTYPE_TIMEOUT;
	i3UIButton			*	m_pBtns[I3UI_MBTYPE_COUNT] = { nullptr, nullptr, nullptr };
	i3UIStaticText		*	m_pMessageText = nullptr;

	virtual void	_UpdateShapeState(void) override;
			void	_CalcBoxSize(void);
			void	_CalcMessagePosition( void);
			void	_CalcButtonPosition(void);

	//caption, msg, button의 text를 바탕으로 미리 size를 계산

	void	_PreCalcComponentsSize( void);

public:
	virtual ~i3UIMessageBox();

	void	Clear( void);
	void	PreCalcBoxSize( void);
			
	I3UI_MESSAGEBOX_TYPE getType(void)	{	return m_Type;}

			void	SetMsgBoxType( I3UI_MESSAGEBOX_TYPE type);
			
			void	SetMsgString(const char *pszString);

			void	SetBtnCaption( INT32 idx, const char * pszString);

			//void	setMessagePos( REAL32 x, REAL32 y);
			void	setBottomSpace( REAL32 space)	{	m_bottomSpace = space;}

	virtual void	OnBindTemplate(void) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;
	virtual void	OnEnabled(bool bEnable) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

#if defined( I3_DEBUG)
	virtual void	GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List) override;
#endif

	static void		RegisterLuaFunction(LuaState * pState);
};

#endif //__I3UI_BUTTON_H__
