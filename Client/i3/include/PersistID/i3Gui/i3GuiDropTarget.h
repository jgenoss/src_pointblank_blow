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
	I3_CLASS_DEFINE(i3GuiDropTarget);

public:
	i3GuiDropTarget();
	virtual ~i3GuiDropTarget();

	void SetManager(i3GuiDragDropManager* pDragDropManager);
	void SetTargetArea(INT32 x, INT32 y, UINT32 w, UINT32 h);
	INT32 GetState(void) const;
	void DragNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y);
	void DropNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y);
	void Block(void);
	void Unblock(void);
	void EndDrop(void);

	virtual void OnComponentUpdate(void);

	virtual UINT32 OnSave(i3ResourceFile* pResFile);
	virtual UINT32 OnLoad(i3ResourceFile* pResFile);

private:
	BOOL _IsInTargetArea(INT32 x, INT32 y);

private:
	i3GuiDragDropManager* m_pDragDropManager;
	i3GuiDragSource* m_pDragSource;
	INT32 m_nState;
	INT32 m_nCommand;
	INT32 m_x;
	INT32 m_y;
	UINT32 m_w;
	UINT32 m_h;
};

#endif //_i3GuiDropTarget_h