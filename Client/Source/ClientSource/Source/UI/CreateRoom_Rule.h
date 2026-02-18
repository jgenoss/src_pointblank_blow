#if !defined( __CREATEROOM_RULE_H__)
#define __CREATEROOM_RULE_H__

#include "CreateRoom_Base.h"

class CreateRoom_Rule : public CreateRoom_Base
{
public:

	CreateRoom_Rule(i3UIScene* pBase);
	~CreateRoom_Rule();

public:

	virtual void	InitData(bool isCreateRoom) override;

	CSI_RULE*		SelectRule(int nRule, bool bSpecial);
	CSI_RULE*		GetSelectedRule();

private:

	void			RuleUIUpdate();

private:

	i3::vector<CSI_RULE*> m_vNormalRules;
	i3::vector<CSI_RULE*> m_vSpecialRules;

	CSI_RULE*		m_pSelectedRule;
};

#endif