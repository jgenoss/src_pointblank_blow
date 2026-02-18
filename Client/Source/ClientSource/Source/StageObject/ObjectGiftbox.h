#pragma once

#include "GameControledObject.h"

class CGameObjectGiftbox : public CGameControledObject
{
public:
	I3_CLASS_DEFINE(CGameObjectGiftbox, CGameControledObject);

	CGameObjectGiftbox();
	virtual ~CGameObjectGiftbox();

	virtual void InitObject() override;
	virtual void ResetObject() override;
	virtual void EnableObject() override;
	virtual void ProcessObject(REAL32 tm) override;
	virtual void SetControledObjectState(INT32 state) override;
	virtual INT32 GetControledObjectState() override { return -1; }

	void Collide(CGameCharaBase* hittedPlayer);
};