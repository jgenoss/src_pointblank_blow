#if !defined( __INPUT_CTRL_AI_FOR_DINO_H__)
#define __INPUT_CTRL_AI_FOR_DINO_H__

#include "GameCharaInputControlAI.h"

//리스폰->다음 패스로 이동 ->이동중 적군 만나면 공격 -> 적이 없어지면 다시 이동

class CInputCtrlAI_ForDino : public CGameCharaInputControlAI
{
	I3_CLASS_DEFINE( CInputCtrlAI_ForDino, CGameCharaInputControlAI);

public:
	virtual bool			MoveToTarget( i3GameObj* pTarget, REAL32 rMinLength, REAL32 rDeltaSeconds) override;
};

#endif // __INPUT_CTRL_AI_FOR_DINO_H__
