#pragma once

	//----------------------------------------------------------------------------------//
	//							상대방 정보 보기										//
	//----------------------------------------------------------------------------------//

struct OtherUserInfo 
{
	i3::rc_wstring			m_wstrOtherUserName;
	i3::vector<T_ItemID>	m_OtherUserItemIdList;
};


class ViewOtherUserItemContext : public i3::shared_ginst<ViewOtherUserItemContext>
{
public:

	ViewOtherUserItemContext();
	~ViewOtherUserItemContext();

	OtherUserInfo*				GetData() { return &m_OtherUserInfo; }
	const OtherUserInfo*		GetData() const { return &m_OtherUserInfo; }

private:
	
	OtherUserInfo		m_OtherUserInfo;

};