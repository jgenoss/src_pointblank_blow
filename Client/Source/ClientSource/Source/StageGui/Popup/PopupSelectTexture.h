#ifndef __POPUP_SELECT_TEXTURE_H
#define __POPUP_SELECT_TEXTURE_H

#include "PopupBase.h"

class i3GuiRoot;
class CStageMapEditor;

class CPopupSelectTexture : public CPopupBase
{
	I3_CLASS_DEFINE(CPopupSelectTexture);

private:
	CStageMapEditor *	m_pStage;
	i3GuiRoot *			m_pRoot;

	i3GuiStatic*		m_pSTD_Win;

	i3GuiList*			m_pSTD_Scroll;
	i3GuiEditBox*		m_pSTD_Caption;
	i3GuiStatic*		m_pSTD_Preview;
	i3GuiButton*		m_pSTD_OK;
	i3GuiButton*		m_pSTD_Cancel;	

public:
	CPopupSelectTexture(void);
	virtual ~CPopupSelectTexture(void);

	void			CreateSelectTexture(CStageMapEditor * pStage, i3GuiRoot * pRoot);
	virtual BOOL	OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code);
	
	virtual void	OnOpenPopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnClosePopup(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnExitKey(void);

	virtual void	OnUpdate(RT_REAL32 rDeltaSeconds);
	virtual void	OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify) {};
	virtual BOOL	OnLinkControl(i3BinList * plist, i3GameNode * pControl) { return TRUE; }
	virtual void	OnPreGuiNotify(void) {};
};

#endif	//	__POPUP_SELECT_TEXTURE_H