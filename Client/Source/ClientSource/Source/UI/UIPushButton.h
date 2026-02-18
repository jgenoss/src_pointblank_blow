#if !defined( __UI_PUSH_BUTTON_H__)
#define __UI_PUSH_BUTTON_H__

#include "UIPopupBase.h"

//CreateRoom의 푸시 버튼 전용 등록 enum.
namespace CR_PushButton
{
	enum BTN_VALUE
	{
		JUMPIN_IMPOSSIBLE = 0, JUMPIN_POSSIBLE, JUMPIN_MAX,
		TEAM_BALANCE_NONE = 0, TEAM_BALANCE_SKILL, TEAM_BALANCE_MEMBER, TEAM_BALANCE_MAX,
		AUTO_TEAM_CHANGE_NOUSE = 0, AUTO_TEAM_CHANGE_USE, AUTO_TEAM_CHANGE_MAX
	};

	enum BTN_TYPE
	{
		TYPE_JUMP_IN = 0,		// 점프인(난입) 버튼
		TYPE_TEAM_BALANCE,		// 팀밸런스 버튼
		TYPE_AUTO_TEAM_CHANGE,	// 자동 공수교대 버튼
		TYPE_MAX
	};
}

struct PushButton
{
	UINT8 ui8Value;											//버튼의 분별 값. (ENUM)
	i3UIButton * pNormalBtn;
	i3::string	str_btn_name;

	PushButton(void)
	{
		ui8Value		= 0xFF;
		pNormalBtn		= nullptr;
		str_btn_name.clear();
	}
	
	void SteBtnInfo(UINT8 value, i3UIButton* btn, i3::string btn_name)
	{
		ui8Value		= value;
		pNormalBtn		= btn;
		str_btn_name	= btn_name;
	}

	void SetPushed()			{ pNormalBtn->UnSelectedNormalState();	}
	bool GetPushed(void)		{ return pNormalBtn->isPushFlag();		}
};

class PushButton_LIST
{
public:
	PushButton_LIST();
	~PushButton_LIST();

public:
	/** breif PushBtn으로 설정을 해놓은 버튼들을 다 불러옴. 즉, 푸시 버튼이 먼저 선행되어야 함 */
	void	InitList(i3UIScene * pScene );

	void	PushBtn(i3::string str_btn_name, UINT8 btn_value);

	void	SetValue(UINT8 btn_value);
	UINT8	GetValue()						{ return m_ui8Value; }

private:
	i3::vector<PushButton*>	m_vbtnlist;								//가지고 있는 버튼들
	UINT8					m_ui8Value;								//가지고 있는 버튼들 중 활성화 되어 있는 버튼 분별 값.
};

#endif
