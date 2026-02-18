#ifndef _OUTGAMECOMMANDSET_H__
#define _OUTGAMECOMMANDSET_H__

class COutGameQACommandSet :	public i3ElementBase
{
	I3_CLASS_DEFINE(COutGameQACommandSet, i3ElementBase);

	enum COMMAND_TYPE
	{
		COMMAND_RANDOMBOX,
	};

	struct ItemInfo
	{
		UINT32 m_itemID = 0;
		i3::rc_wstring m_itemName;
	};

public:
	~COutGameQACommandSet();

	void ProcessCommand(REAL32 rDeltaSeconds);

	//랜덤박스 테스트 커맨드
	bool CheckRandomBoxCommand(const i3::wliteral_range& wImeBufRng);

	
private:
	//전역 함수 이벤트 설정.
	//이벤트 타입과 인자만 넘겨받는다. 이벤트로 던지는 파라매터들은 케이스별로 다르므로
	//이 이후에 추가하는 사람들이 알아서 구현하게.
	void SetEventForCommand(COMMAND_TYPE commandType, const void * pArg = nullptr, const void * pArg1 = nullptr, const void * pArg2 = nullptr, LPCSTR szFile = nullptr, int nLine = 0);

	//랜덤박스 테스트 커맨드
	bool	m_bIsRandomBoxTestEnabled = false;
	UINT32	m_itemID = 0;
	T_ItemDBIdx	m_itemDBIdx = 0;
	UINT32	m_iterationCount = 0;
	REAL32	m_timeBuffer = 0.0f;

	i3::vector<ItemInfo> resultBuffer;
	void	ProcessRandomBoxCommand(REAL32 rDeltaSeconds);
	void    SetCSVLog();

public:
	bool	isRandomBoxTestEnabled()	{return m_bIsRandomBoxTestEnabled;}
	void	AddResultToBuffer(UINT32 itemID, const i3::rc_wstring& itemName);
};

#endif