#if !defined( __CREATEROOM_ROOMINFO_H__)
#define __CREATEROOM_ROOMINFO_H__

#include "CreateRoom_Base.h"

class CreateRoom_RoomInfo : public CreateRoom_Base
{
public:
	CreateRoom_RoomInfo(i3UIScene* pBase);
	~CreateRoom_RoomInfo();

public:

	virtual void	InitData(bool isCreateRoom) override;
	virtual bool	ConfimRoomInfo(ROOM_INFO_ALL &info) override;

	void			ResetPasswordEditbox();

private:

	void				RoomInfoUIUpdate(const i3::wliteral_range& wRoomTitleRng, char * szPassword);

	bool				SetTitle(i3::stack_wstring &wstrTitleName);
	bool				SetPassword();

private:

	i3UIEditBox *		m_pTitleBox;
	i3UIEditBox *		m_pPwBox;
	i3UICheckBox*		m_pPwCheckBox;
};

#endif