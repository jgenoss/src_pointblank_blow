#if !defined( __UI_OPTION_Grade_H__)
#define __UI_OPTION_Grade_H__

#include "UIOptionBase.h"

class UIOptionGrade : public UIOptionBase
{
	I3_CLASS_DEFINE( UIOptionGrade, UIOptionBase );

private:
	virtual void load_value() override {}
	virtual void save_value() override {}
	virtual void default_value() override {}
	virtual void apply_game() override {}
	virtual void apply_lua() override;
	virtual bool is_modified() override { return false; }

public:
	UIOptionGrade() {}
	virtual ~UIOptionGrade() {}

	virtual void OnCreate( i3GameNode * pParent) override;
	virtual void _InitializeAtLoad(i3UIScene * pScene) override;

};

#endif
