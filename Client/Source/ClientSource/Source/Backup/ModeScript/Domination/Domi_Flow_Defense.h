#pragma once

#include "./Domination_FlowBase.h"

class gmode_domination;
class CDomi_Flow_Defense : public CDominationFlowBase
{
private:
	enum { DEFENCE_OBJECT_MAX = 4 };

	std::map<INT32, INT32> m_mapObjectIdx;					/*<Used Object Idx, Not Used Object Idx>*/

	i3Object*			m_pDefenceObj;

	bool				m_bEnableCrossHitOnObjecct;			//현재 미션 오브젝트에 Cross Hit이 적용 됐을 때.
	bool				m_bObjectActivated;					//서버로 부터 활성화 신호를 받았을 때.


	bool				m_bPressedKey;						//Special Key로 활성화 중 일때.
	REAL32				m_rPressedKeyTime;					

	UINT8				m_iDefenceCount;

	void				_ResetKeyState();					//'e' 버튼 눌렀던 것을 초기화.

	void				_Activate();			
	void				_Deactivate();

public:
	CDomi_Flow_Defense( gmode_domination* pmode);
	~CDomi_Flow_Defense(void);

	void						ProcessModeData( INT32 nIndex, INT32 nData);
	void						ProcessInput( REAL32 rDeltaSeconds);

	virtual	void				ResetObject();

	bool						CheckCrossHitMissionObject( CGameCharaBase* pCharacter);

	bool						IsPressedMissionkey() const { return m_bPressedKey;}
	REAL32						GetPressedMissionKeyTime() const { return m_rPressedKeyTime;}
};
