#if !defined( __UI_POPUP_CLAN_BASE_H__)
#define __UI_POPUP_CLAN_BASE_H__

#include "UIPopupBase.h"

struct SearchInfo {
	bool bDuplicateName;               // 중복여부
	bool bDuplicateAddress;            // 중복여부
	i3::rc_wstring wstrClanName; // 클랜 이름 및 URL 저장 변수
	i3::rc_wstring wstrClanURL;  // 클랜 이름 및 URL 저장 변수
};

class UIPopupClanBase : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupClanBase, UIPopupBase);
	CStrFilterPtr		m_StringFilter;
	CStrFilterPtr		m_StringFilter2;

public:
	UIPopupClanBase(void);
	virtual ~UIPopupClanBase(void);

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	bool	_OnPopupCheckOverlapping( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName, bool bDuplicate);			// virtual 제거..
	bool	_OnPopupCheckOverlappingAddress( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName, bool bDuplicate);  // virtual 제거..

	bool	_CheckNickNameOverLap( const i3::wliteral_range& wTextRng) const;
	bool	_CheckClanAddressOverLap(const i3::wliteral_range& wTextRng) const;
	bool	_CheckClanAddressValidate(const i3::wliteral_range& wTextRng) const;

	// "은/는 사용하실 수 있습니다." 저장 함수
	void    UseName( i3::stack_wstring& wstrSaveChar, const i3::rc_wstring& wstrName);
};

#endif