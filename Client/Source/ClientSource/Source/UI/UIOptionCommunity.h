#if !defined( __UI_OPTION_Community_H__)
#define __UI_OPTION_Community_H__

#include "UIOptionBase.h"

#include "ProgressEdit.h"

class UIOptionCommunity : public UIOptionBase
{
	I3_CLASS_DEFINE( UIOptionCommunity, UIOptionBase );

	enum { MAX_MACRO = 5, };

private:
	// Community관련 변수들
	INT32 m_nWhisperType;
	INT32 m_nInviteType;

	// Macro
	struct Macro
	{
		Editbox edit;
		i3::rc_wstring wstr_word;
		bool type;
	};
	Macro m_Macro[MAX_MACRO];

	i3UIButton *	m_pTwitchButton;

public:
	void clicked(int idx1, int idx2);
	void SetTwitchButton();

private:
	virtual void load_value() override;
	virtual void save_value() override;
	virtual void default_value() override;
	virtual void apply_game() override;
	virtual void apply_lua() override;
	virtual void set_lua();
	virtual bool is_modified() override;

	bool is_modified_basic();
	bool is_modified_macro();

	void _setTwitchVisible();

public:
	UIOptionCommunity();
	virtual ~UIOptionCommunity();

	virtual void _InitializeAtLoad( i3UIScene * pScene ) override;
	virtual void OnCreate( i3GameNode * pParent) override;
	virtual bool OnKey_Escape() override;

	virtual void entrance_scn() override;
	virtual void show_scn() override;
	virtual void hide_scn() override;
	virtual void exit_scn() override;
};

#endif
