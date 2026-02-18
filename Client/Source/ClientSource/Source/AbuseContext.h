#pragma once

class AbuseContext : public i3::shared_ginst<AbuseContext>
{
public:
	AbuseContext() {}
	
	void				SetAbuseInfo(ABUSING_LEVEL, UINT32);
	void				ResetAbuse();
	bool				IsAbusing() const		{ return m_AbuseFlag; };
	UINT8				GetAbuseLevel() const	{ return m_AbuseLevel; };
	UINT32				GetAbuseTime() const	{ return m_AbuseTime; };

private:
	//작성자 : 하동익
	UINT32				m_AbuseTime = 0;
	bool				m_AbuseFlag = false;
	UINT8				m_AbuseLevel = ABUSING_LEVEL_NONE;
};
 