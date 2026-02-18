#if !defined( __I3_EXP_CUI_FRAME_MSG_HANDLER_H)
#define __I3_EXP_CUI_FRAME_MSG_HANDLER_H

class i3Export;

class i3EXPFrameMsgHandler : public CUIFrameMsgHandler 
{
protected:
	i3Export *	m_pExport;

public:
	i3EXPFrameMsgHandler(i3Export *pBookmark);

	int ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif
