#ifndef _PopupBase_h
#define _PopupBase_h

#include "GuiNotifyReceiver.h"

class CPopupBase: public CGuiNotifyReceiver
{
	I3_ABSTRACT_CLASS_DEFINE(CPopupBase, CGuiNotifyReceiver);
public:
	CPopupBase();
	virtual ~CPopupBase();

	virtual void OnInitControl(void);
	virtual void OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);

	virtual BOOL OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	

};

#endif //_PopupBase_h
