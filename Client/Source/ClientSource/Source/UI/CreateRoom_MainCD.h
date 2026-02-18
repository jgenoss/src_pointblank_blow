#if !defined( __CREATEROOM_MAINCD_H__)
#define __CREATEROOM_MAINCD_H__

#include "CreateRoom_Base.h"

struct MAIN_CD_ELEMENT
{
	i3::stack_wstring	m_wsconditon;
	int					m_ncuridx;
};

class CreateRoom_MainCD : public CreateRoom_Base
{

	enum CD_E_NAME
	{
		CD_E_WIN, CD_E_TIME, CD_E_PEOPLE, CD_E_AI_LEVEL, CD_E_AI_CNT, CD_E_MAX
	};

public:
	CreateRoom_MainCD(i3UIScene* pBase);
	~CreateRoom_MainCD();

public:

	virtual void	InitData(bool isCreateRoom) override;
	virtual void	UpdateforStage(CSI_STAGE* stage, bool firstupdate = false) override;
	virtual bool	ConfimRoomInfo(ROOM_INFO_ALL &info) override;

	void			ChangeWinConditon(INT32 cur);
	void			ChangeTimeLimit(INT32 cur);
	void			ChangeAiCnt(INT32 cur);
	void			ChangeAiLevel(INT32 cur);

private:

	void				MainCDUIUpdate();

	void				SetWin();
	void				SetTime();
	void				SetPeople();
	void				SetAIModeCondition();

	void				getTimeMinMax(INT32* idxMin, INT32* idxMax);
	UINT8				getMaxPeople();

private:

	UINT8				m_SubType;
	bool				m_bAiMode;

	i3::vector<UINT8>	m_GameTimeList;
	i3::vector<UINT8>	m_GameWinList;

	MAIN_CD_ELEMENT		m_Conditons[CD_E_MAX];

};

#endif