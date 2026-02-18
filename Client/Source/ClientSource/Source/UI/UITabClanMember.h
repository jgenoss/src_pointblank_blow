/**
 * \file	UI\UITabClanMember.h
 *
 * Declares the user interface tab clan member class.
 */


#if !defined( __UI_TAB_CLAN_MEMBER_H__)
#define __UI_TAB_CLAN_MEMBER_H__

#include "UITabClan.h"

class UITabClanMember : public UITabClan_Normal
{
	I3_CLASS_DEFINE( UITabClanMember, UITabClan_Normal);

protected:
	void			_SetClanMemberInfo( INT32 slot, CLAN_MEMBER_BASIC_INFO * pInfo);
	void			_BuildClanMemberList( void);
	void			_UpdateClanMember( void);

public:
	UITabClanMember();
	~UITabClanMember();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

private:
	typedef i3::shared_ptr<CLAN_MEMBER_BASIC_INFO>		CLAN_MEMBER_BASIC_INFO_PTR;
	i3::vector<CLAN_MEMBER_BASIC_INFO_PTR>	m_vecClanMemberList;

};




#endif