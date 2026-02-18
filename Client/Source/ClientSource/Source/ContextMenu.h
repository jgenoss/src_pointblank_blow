#ifndef _ContextMenu_h
#define _ContextMenu_h

#if legacy_gui_function
#include "GuiNotifyReceiver.h"

class CContextMenu: public CGuiNotifyReceiver
{
	I3_CLASS_DEFINE(CContextMenu, CGuiNotifyReceiver);
public:
	CContextMenu();
	virtual ~CContextMenu();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify) override;
	virtual bool OnLinkControl(const i3::vector_set<STRCINFO*>* plist, i3GameNode * pControl) override;
	virtual void OnInitControl(void) override;
	virtual void OnUpdate(REAL32 rDeltaSeconds) override;

	void Reset(void);	// Close & ClearCommand
	void Open(i3EventReceiver* observer,INT32 requestId);
	void AddCommand(ContextMenuCommand command);

#if defined(CHANGE_ADDCOMMAND_METHOD)
	ContextMenuCommand*		GetCommandMap()		{return m_commandMap;}
	UINT32					GetCommandCount()	{return m_commandCount;}
#endif

private:
	void _OnCommand(INT32 commandIdx);

private:
	ContextMenuCommand m_commandMap[CMC_MAX];
	UINT32 m_commandCount;
	i3EventReceiver* m_pObserver;
	INT32 m_requestId;
	char m_commandText[CMC_MAX][MAX_STRING_COUNT];

	i3GuiStatic* m_pContextGui;
	i3GuiPopupList* m_pList;
};

#endif
#endif
