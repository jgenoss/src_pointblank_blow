#if !defined( __DINO_INFO_H__)
#define __DINO_INFO_H__

#include "CharaInfo.h"

#define DINO_WEAPON_ARRAY_COUNT	3

class CDinoInfo : public CCharaInfo
{
	I3_CLASS_DEFINE( CDinoInfo, CCharaInfo);

private:
	T_ItemID	m_ui32DinoWeaponID[DINO_WEAPON_ARRAY_COUNT] = { 0 };

	REAL32		m_rDinoCCRadius = 1.0f;
	REAL32		m_rDinoCCTall = 1.0f;
	REAL32		m_rDinoCCStepOffset = 1.0f;
	REAL32		m_rDinoHP_Ratio[SLOT_MAX_COUNT];			// [initialize at constructor]

	REAL32		m_rRexFootStepTrembleAmp = 1.0f;							//티렉스의 발걸음시 진동

	enum eCC_Skill { PRIMARY, SECONDARY, CC_SKILL_MAX};
	INT32		m_UIShapeIndex_CCSKill[CC_SKILL_MAX];				//CC모드 스킬 UIShapeIndex [initialize at constructor]
	i3::rc_wstring		m_astrCCSkillDesc[CC_SKILL_MAX];			//CCMode 스킬 설명

public:
	CDinoInfo();

	virtual void		ReadInfo( i3RegKey * pKey) override;
	virtual void		OnEncryptInfo( UINT8 u8Key) override;

	virtual T_ItemID	GetDinoWeaponID(INT32 idx)	const override;
	virtual void		SetDinoWeaponID(INT32 idx, T_ItemID itemID) override;

	virtual REAL32		GetDinoHP_Ratio(INT32 idx) const override;
	virtual void		SetDinoHP_Ratio(INT32 idx, REAL32 val) override;

	virtual REAL32		GetDinoCCRadius() const override;
	virtual REAL32		GetDinoCCTall() const override;
	virtual REAL32		GetDinoCCStepOffset() const override;

	virtual void 		SetDinoCCTall(REAL32 val) override { m_rDinoCCTall = val; }
	virtual void 		SetDinoCCStepOffset(REAL32 val) override { m_rDinoCCStepOffset = val; }
	virtual void 		SetDinoCCRadius(REAL32 val) override { m_rDinoCCRadius = val; }

	virtual REAL32		getFootStepTrembleAmp(void) const override;
	virtual void		SetFootStepTrembleAmp(REAL32 val) override;

	virtual INT32		getShapeIndexCCSkill( UINT8 idx) const override;
	virtual void		getCCSkillDescription( i3::rc_wstring& out, UINT8 idx) const override;
};

#endif
