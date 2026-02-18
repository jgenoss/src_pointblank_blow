#ifndef __POPUP_QUERY_PASSWORD_H__
#define __POPUP_QUERY_PASSWORD_H__

#include "PopupBase.h"

typedef		void	(*PopupQueryPasswordCallBack)(BOOL OK, const string & Password, void * pUserContext);


struct	QueryPasswordParam
{
	QueryPasswordParam();

	const char *	pMsg;
	BOOL			OnlyNumber;
	BOOL			ShowPassword;
	INT32			LimitCount;
	PopupQueryPasswordCallBack	Callback;
	void *			pUserContext;
};

class CPopupQueryPassword : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupQueryPassword);

public:
	CPopupQueryPassword(void);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3BinList * plist, i3GameNode * pControl);
	virtual void OnInitControl(void);
	virtual void OnExitKey(void);

	// pArg1 = 질의메시지(const char*), pArg2 = PopupQueryPasswordCallBack.
	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	virtual	BOOL	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);

private:
	i3GuiStatic* m_pJoinWin;

	i3GuiEditBox* m_pJoinCaption;
	i3GuiEditBox* m_pJoinMessage;
	i3GuiEditBox* m_pJoinInput;
	i3GuiButton* m_pJoinConfirm;
	i3GuiButton* m_pJoinCancel;


	PopupQueryPasswordCallBack	m_pCallback;
	void *						m_pUserContext;
};

#endif	//	__POPUP_REPAIR_H