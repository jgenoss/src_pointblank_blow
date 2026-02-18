#pragma once

#include "CaseRunDefine.h"
#include "Vec3.h"

#include <map>
#include <string>

namespace CaseRun
{
	using namespace std;

	// 변수(Variable)
	struct Variable
	{
		std::wstring		m_Name;
		std::wstring		m_CurrentValue;
	};

	// Variable을 관리하기 위한 map type.
	typedef map< wstring, wstring>	VariableMap;

	class NodeBase;

	class Interface
	{
	public:
		void					setErrorMsg(const wchar_t *format, ...);
		const std::wstring &	getErrorMsg(void) const { return m_ErrorMsg; }

		enum MOUSE_ACTION
		{
			MOUSE_MOVE,
			MOUSE_BUTTON_DOWN,
			MOUSE_BUTTON_UP,
			MOUSE_BUTTON_DBLCLICK
		};

	public:
		Interface(void);
		~Interface(void);

		// Application 이름
		virtual wstring		GetAppName(void) = 0;

		// Main Window Handle
		virtual HWND		GetHWND(void) = 0;

		// Target이 테스트를 수행할 수 있는지 여부를 반환
		virtual bool		IsReady(void) = 0;

		// 상태 정보를 출력하기 위한 함수.
		// 각 Target은 이 함수를 override해, 필요한 문자열 정보를 게임 화면에 출력할 수 있어야 한다.
		virtual void		UpdateText( const wchar_t * pszTitle, const wchar_t * pszMsg) = 0;

		// 지정 경로의 UI Ctrl의 Pixel 단위 영역을 반환해야 한다.
		// false : 만약 경로의 Ctrl이 존재하지 않거나, 수행할 수 없는 상태를 나타내는 반환값.
		virtual bool		GetCtrlRect( const wchar_t * pszCtrlPath, RECT * pRect) = 0;

		//
		// Text를 이용해 Ctrl을 검색하고, 위치한 영역(Rect)를 얻어 온다.
		// - pszCtrlPath : 검색할 Ctrl의 경로.
		//					!= NULL : 지정 Ctrl을 포함하여, Children을 검색한다.
		//					== NULL : 화면 전체의 모든 (활성화된) Ctrl을 대상으로 Text를 검색한다.
		// - pszTxt      : 검색할 Text. NULL일 수 없음. 
		virtual bool		FindCtrlText_GetRect(const wchar_t * pszCtrlPath, const wchar_t * pszTxt, bool bMustBeSame, RECT * pRect) = 0;

		// 주어진 경로의 Ctrl이 사용자-입력을 받아들 일 수 있는 (Ready) 상태인지 확인한다.
		virtual bool		IsCtrlReady( const wchar_t * pszCtrlPath) = 0;

		// 지정 경로의 Ctrl의 Text Caption을 반환한다.
		virtual bool		GetCtrlText( const wchar_t * pszCtrlPath, std::wstring & str) = 0;
		

		////////////////////////////////////////
		// Event Handlers
		//

		// Server로 부터 test를 초기화하는 과정에서 Agent Node를 만나, 계정 정보를 전달받았을 때 호출된다.
		virtual void		OnAgent( float tmLocal, const wchar_t * pszID, const wchar_t * pszPW);

		// Test Action Node가 전달되었을 때 호출되는 함수. 이 핸들러는 모든 종류의 Node에 호출된다.
		virtual void		OnTest( NodeBase * pNode) = 0;

		// Test가 종료될 때 호출되는 핸들러.
		virtual void		OnFinishTest(void);

		// 화면 Indicator에 상태 정보를 갱신하고 싶을 때 호출된다.
		// Game 개발자는 이 핸들러에서 DirectX, OpenGL 등의 Gfx 함수를 통해, 필요한 정보를 화면에 출력(Indicator) 해주어야 한다.
		virtual void		OnShowIndicator( const RECT & rect, const wchar_t * pszMsg);

		// Indicator가 서서히 사라지는 동안에 Alpha(투명도) 값을 전달하기 위한 핸들러.
		// Game 개발자는 이 핸들러에서 Indicator의 반투명 상태를 적절히 구현해 주어야 한다.
		virtual void		OnIndicating( float alpha);


		/////////////////////////////////////////
		// Node Handlers
		// (Game 개발자는 아래의 모든 Handler들을 게임에 맞게 구현해 주어야 한다.

		// Exit Node가 전달되었을 때 호출된다.	Game을 종료해 주어야 한다.
		virtual TESTRESULT	OnExit(void) = 0;

		// Key Action이 발생했을 때 호출된다. 구체적인 키의 값은 문자열로 정의되는데, KeyString.cpp를 참조.
		// Key가 길게 눌려지는 동안 계속 호출된다.
		virtual void		OnKeyPress( float tmLocal, const wchar_t * pszKeyName, bool bPress);

		// Mouse Button이 눌러졌을 때 호출된다.
		virtual void		OnMouseAction(float tmLocal, MOUSE_ACTION action, POINT pt, BUTTON btn);

		// Game의 매 Frame마다 호출된다.
		virtual void		OnTick( float tm) = 0;

		// 현재 UI Ctrl의 Caption Test를 설정한다.
		virtual TESTRESULT	OnUISetText( float tmLocal, const wchar_t * pszCtrlPath, const wchar_t * pszText) = 0;

		// 지정 경로의 UI Ctrl의 항목을 선택할 때 호출된다. 단, ComboCtrl, ListCtrl 또는 ListView 형태와 같이 여러 항목을 가지고 있는 경우에만 해당된다.
		virtual TESTRESULT	OnUISelectItem( float tmLocal, const wchar_t * pszCtrlPath, const wchar_t * pszText, bool bExact, bool dblClick) = 0;

		// 지정 경로의 UI Ctrl의 Caption Text를 모두 검색해, 찾고자 하는 문자열이 있는지 확인하는 함수.
		virtual TESTRESULT	OnUIScanText( float tmLocal, const wchar_t * pszCtrlPath, const wchar_t * pszText, bool bExact, bool bSubCtrl) = 0;

		// 지정 경로의 UI Ctrl의 Check 상태가 주어진 상태(CTRL_STATE)와 일치하는지 확인하는 함수.
		virtual TESTRESULT	OnUICheckState( float tmLocal, const wchar_t * pszCtrlPath, CTRL_STATE state) = 0;

		virtual TESTRESULT	OnUICheckTopWindow( float tmLocal, const wchar_t * pszCtrlPath) = 0;

		// 게임 내의 변수 값을 나열해 반환해 주는 함수.
		// 게임이 갖고 있는 컨텐츠를 CaseRunModule에 전달해 주기 위한 함수이며, 변수명 및 Value명은 게임에 맞추어 사전 정의되어야 한다.
		// ex. "모든 총기류", "모든 스테이지"
		// 서버는 이 함수를 통해 전달받은 나열값으로 foreach Node를 구동하게 된다.
		virtual vector<std::wstring *> * EnumVariableValue( const wchar_t * pszVar, const wchar_t * pValue, int count) = 0;

		// 주인공(Player)를 특정 위치까지 이동시키는 함수.
		// 이동 대상은 절대 좌표이거나, 오브젝트 및 캐릭터일 수 있다.
		virtual TESTRESULT	OnMoveTo( float tmLocal, LOCTYPE type, wstring & value) = 0;

		// 주인공(Player)가 특정 방향을 바라보도록 하는 함수.
		// 주목 대상은 <Theta, Rho> 형식의 값이거나, 오브젝트 및 캐릭터일 수 있다.
		virtual TESTRESULT	OnLookAt( float tmLocal, DIRTYPE type, wstring & value) = 0;

		// Game-Specific한 함수를 호출하는 Interface function
		// pszFunction에서 주어질 수 있는 함수는 각 Game마다 별도로 정의해야 한다.
		virtual TESTRESULT	OnCustom( float tmLocal, const wchar_t * pszFunction, const wchar_t * param1, const wchar_t * param2) = 0;

		// Variable을 지정할 때 호출되는 함수.
		virtual TESTRESULT	OnStore(wstring & var, wstring & value);

	public:
		// Variable용 내부 함수.
		// Game에서는 호출하지 않아야 한다.
		TESTRESULT			internal_Store( const wchar_t * pszVar, const wchar_t * pszValue);
		TESTRESULT			internal_Load( const wchar_t * pszVar, const wchar_t * pszCtrlID);
		TESTRESULT			internal_Calc( const wchar_t * pszV1, const wchar_t * pszV2, VAROP op, const wchar_t * pszV3);
		TESTRESULT			internal_Compare( const wchar_t * pszV1, VARCOMPARE func, const wchar_t * pszV2);

		void				internal_Indicate( const RECT & rect, const wchar_t * pszMsg);
		void				internal_Tick( float tm);

		bool				ParseValue( const wchar_t * pszStr, wstring & str);

	protected:
		std::wstring		m_UserID;
		std::wstring		m_Password;
		VariableMap			m_VarMap;			// 변수 관리를 위한 map

	protected:
		std::wstring		m_ErrorMsg;

	protected:
		float				m_tmIndicator;

	protected:
		Vec3				ParseVector3( const wchar_t * pszStr);
	};
}
