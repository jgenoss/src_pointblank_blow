#ifndef _PTTREEWORK_LIVE_H_
#define _PTTREEWORK_LIVE_H_

#include "PTTreeworkBase.h"

enum LIVE_STATE
{
	LS_WORKINGFOLDER_OPENED,
	LS_SELCHANGED,
	LS_RCLICKED,
	LS_ITEM_ADDED,
	LS_REPACK_MENU_CLICKED,
	LS_DELETE_MENU_CLICKED,
	LS_EXPORT_MENU_CLICKED,
	LS_NEWPACK_MENU_CLICKED,
	LS_MAKE_NEW_PACK,
};

class PTTreeworkLive : public PTTreeworkBase
{
public:
	PTTreeworkLive();
	virtual ~PTTreeworkLive();

	virtual void Create(const char* workdir, CDragDropTreeCtrl* treeCtrl);
	virtual void Destroy();

	void SetState(LIVE_STATE state) { m_state = state; }
	LIVE_STATE GetState() const		{ return m_state;  }

	// 이벤트 처리
	void		OpenWorkingFolder(CDragDropTreeCtrl* treeCtrl);
	void		Selchanged(NMHDR* nmhdr);
	void		RClicked(NMHDR* nmhdr);
	HTREEITEM	AddItem(HTREEITEM itemParent, PTFile* fileSelected, HTREEITEM hInsAfter, int iSelImage, int iNonSelImage, long lParam);
	void		RePackMenuClicked();
	void		DeleteMenuClicked();
	void		ExportMenuClicked();
	void		NewPackMenuClicked();
	void		MakeNewPack(const char* name);

private:
	void PTTreeworkLive::NodeTraverDel(HTREEITEM item);

private:
	LIVE_STATE m_state;
};

#endif