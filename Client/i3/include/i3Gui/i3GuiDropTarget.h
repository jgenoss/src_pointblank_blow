// ! praptor - 미완성입니다. 작동하지 않습니다.

#ifndef _i3GuiDropTarget_h
#define _i3GuiDropTarget_h

#include "i3GuiComponentBase.h"

enum DROP_STATE_TYPE
{
	DROP_STATE_IDLE = 0,
	DROP_STATE_SELECT,
	DROP_STATE_DROP,
	DROP_STATE_BLOCK,
	MAX_DROP_STATE
};

enum DROP_COMMAND_TYPE
{
	DROP_COMMAND_WITHOUT = 0,
	DROP_COMMAND_DRAG_ON,
	DROP_COMMAND_DROP_ON,
	DROP_COMMAND_BLOCK,
	DROP_COMMAND_UNBLOCK,
	DROP_COMMAND_END_DROP,
	MAX_DROP_COMMAND
};

class i3GuiDragDropManager;
class i3GuiDragSource;
class i3GuiDropTarget;

class I3_EXPORT_GUI i3GuiDropTarget: public i3GuiComponentBase
{
	I3_EXPORT_CLASS_DEFINE(i3GuiDropTarget, i3GuiComponentBase);

public:
	void SetManager(i3GuiDragDropManager* pDragDropManager);
	void SetTargetArea(INT32 x, INT32 y, UINT32 w, UINT32 h);
	INT32 GetState(void) const;
	void DragNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y);
	void DropNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y);
	void Block(void);
	void Unblock(void);
	void EndDrop(void);

	virtual void OnComponentUpdate(void) override;

	virtual UINT32	OnSave(i3ResourceFile* pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile* pResFile) override;

private:
	bool _IsInTargetArea(INT32 x, INT32 y);

private:
	i3GuiDragDropManager* m_pDragDropManager = nullptr;
	i3GuiDragSource* m_pDragSource = nullptr;
	INT32 m_nState = DROP_STATE_IDLE;
	INT32 m_nCommand = DROP_COMMAND_WITHOUT;
	INT32 m_x = 0;
	INT32 m_y = 0;
	UINT32 m_w = 0;
	UINT32 m_h = 0;
};

#endif //_i3GuiDropTarget_h