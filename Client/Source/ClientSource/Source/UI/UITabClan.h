#if !defined( __UI_TAB_CLAN_H__)
#define __UI_TAB_CLAN_H__

#include "UITabBase.h"
#include "UIPhaseClanMember.h"

class UITabClan : public UITabBase
{
	I3_ABSTRACT_CLASS_DEFINE( UITabClan, UITabBase);
	
public:
	UITabClan();
	~UITabClan();

	virtual void OnCreate( i3GameNode * pParent) override;

	virtual bool IsManagementTab() const = 0;
		
	virtual void OnExitEnd() override;

	UIPhaseClanMember*	GetPhaseClan() const { return static_cast<UIPhaseClanMember*>( const_cast<UITabClan*>(this)->getParent() ); }

	bool		 isVisitUser( void) {return g_pFramework->GetClanContext()->IsClanVisit(); }

protected:	
	virtual void	OnLoadAllScenes() override;

};

class UITabClan_Normal : public UITabClan
{
	I3_ABSTRACT_CLASS_DEFINE( UITabClan_Normal, UITabClan );
public:
	UITabClan_Normal();
	~UITabClan_Normal();
	
	virtual bool IsManagementTab() const override { return false; }
};


class UISubTabClan_Mgt : public UITabClan
{
	I3_ABSTRACT_CLASS_DEFINE( UISubTabClan_Mgt, UITabClan );

public:
	UISubTabClan_Mgt();
	~UISubTabClan_Mgt();

	virtual void OnExitEnd() override;
	virtual bool IsManagementTab() const override { return true; }
};



#endif