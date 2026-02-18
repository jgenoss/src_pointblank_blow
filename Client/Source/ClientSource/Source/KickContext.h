#pragma once

//
// 강퇴와 관련된 것들만 따로 분리..  (많지는 않아보인다..)
//

class KickContext : public i3::shared_ginst<KickContext>
{
public:
	void		SetKickedByVote()		{ m_KickedByVote = true; }
	void		ClearKickedByVote()		{ m_KickedByVote = false; }
	bool		IsKickedByVote() const	{ return m_KickedByVote; }
	
private:
	
	bool		m_KickedByVote = false;

};
