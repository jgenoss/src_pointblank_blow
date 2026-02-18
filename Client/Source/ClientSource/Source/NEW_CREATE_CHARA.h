#pragma once

// 바깥으로 뺀다...(내포구조체로 좋지 않아 보인다.. )

struct NEW_CREATE_CHARA
{	
	UINT8					m_ui8BuyType = 0;
	T_ItemID				m_TItemID = 0;										// 캐릭터 아이템 ID (슬롯)
	UINT8					m_ui8Class = 0;
	i3::rc_wstring			m_wstrCharaName;						// NET_CHARA_NICK_NAME_SIZE = 33 (매크로가 다소 자의적이다)

	bool					_bCheck = false;
};

inline void		Reset( NEW_CREATE_CHARA* inout)
{

	inout->m_TItemID = 0;
	inout->m_ui8Class = CHARA_CLASS_NONE;

	inout->m_wstrCharaName.clear();
	inout->_bCheck = false;
}
