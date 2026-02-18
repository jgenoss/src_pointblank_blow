#if !defined( __UI_OPTION_Game_H__)
#define __UI_OPTION_Game_H__

#include "UIOptionBase.h"

class UIOptionGame : public UIOptionBase
{
	I3_CLASS_DEFINE( UIOptionGame, UIOptionBase );

private:
	// GamePlay 관련 변수들
	bool m_bEnableFrindlyMark;	//아군 마크 표시 + 닉네임 표시
	bool m_bEnableFrindlyName;

	bool m_bEnableBlood;	// 혈흔 표시
	
	INT32 m_nHandType;	// 무기드는 손( default:0오른손, 1:왼손)
	INT32 m_nCrossHairType;
	bool m_bCrossHairEffect;	
	bool m_bMiniMapMode;	//미니맵 회전 여부

	bool m_nGameInterface;
	bool m_bTeamBand;
	bool m_bTextEffect;

	INT32 m_nLanguage;
	i3::vector<UINT16> m_LanguageList;

public:
	void clicked(int idx1, int idx2);
	void _SetPushedButton(INT32 btnType, INT32 value);

private:
	virtual void load_value() override;
	virtual void save_value() override;
	virtual void default_value() override;
	virtual void apply_game() override;
	virtual void apply_lua() override;
	virtual bool is_modified() override;

	bool is_modified_basic();
	bool is_modified_key();
	
	bool  InsertLanguage(char* language, INT16 languageType);
	void  SetMultiLanguage(bool bEnable);
	void  SelectLanguage(INT32 index);
	bool  CheckLanguagePack();
	INT32 FindLangListIndexToLangID(INT32 languageType);

public:
	UIOptionGame();
	virtual ~UIOptionGame();

	virtual void OnCreate( i3GameNode * pParent) override;

	static void cb_ExitClient(void* pThis,INT32 nParam);
};

#endif
