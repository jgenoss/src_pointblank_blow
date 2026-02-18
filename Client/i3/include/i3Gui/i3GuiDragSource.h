// ! praptor - 미완성입니다. 작동하지 않습니다.

#ifndef _i3GuiDragSource_h
#define _i3GuiDragSource_h

#include "i3GuiComponentBase.h"

enum DRAG_STATE_TYPE
{
	DRAG_STATE_IDLE = 0,
	DRAG_STATE_PUSHED,
	DRAG_STATE_DRAG,
	DRAG_STATE_DROP,
	DRAG_STATE_CANCEL,
	MAX_DRAG_STATE
};

enum DRAG_COMMAND_TYPE
{
	DRAG_COMMAND_WITHOUT = 0,
	DRAG_COMMAND_PUSH,
	DRAG_COMMAND_MOVE,
	DRAG_COMMAND_POP,
	DRAG_COMMAND_TARGET_IN,
	DRAG_COMMAND_END_CANCEL,
	MAX_DRAG_COMMAND
};

class i3GuiDragDropManager;

class I3_EXPORT_GUI i3GuiDragSource: public i3GuiComponentBase
{
	I3_EXPORT_CLASS_DEFINE(i3GuiDragSource, i3GuiComponentBase);

public:
	void SetManager(i3GuiDragDropManager* pDragDropManager);
	INT32 GetState(void) const;
	void Push(INT32 x, INT32 y);
	void Move(INT32 x, INT32 y);
	void Pop(INT32 x, INT32 y);
	void TargetIn(void);

	virtual void OnComponentUpdate(void) override;

	virtual UINT32	OnSave(i3ResourceFile* pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile* pResFile) override;

private:
	i3GuiDragDropManager* m_pDragDropManager = nullptr;
	INT32 m_nState = DRAG_STATE_IDLE;
	INT32 m_nCommand = DRAG_COMMAND_WITHOUT;
	INT32 m_x = 0;
	INT32 m_y = 0;
};

#endif //_i3GuiDragSource_h