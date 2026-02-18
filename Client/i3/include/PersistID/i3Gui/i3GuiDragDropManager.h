// ! praptor - 미완성입니다. 작동하지 않습니다.

#ifndef _i3GuiDragDropManager_h
#define _i3GuiDragDropManager_h

#include "i3GuiComponentBase.h"

class i3GuiDropTarget;
class i3GuiDragSource;

class I3_EXPORT_GUI i3GuiDragDropManager: public i3GuiComponentBase
{
	I3_CLASS_DEFINE(i3GuiDragDropManager);

public:
	i3GuiDragDropManager();
	virtual ~i3GuiDragDropManager();

	void RegisterDropTarget(i3GuiDropTarget* pDropTarget);
	void UnregisterDropTarget(i3GuiDropTarget* pDropTarget);
	void DragNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y);
	void DropNotify(i3GuiDragSource* pDragSource, INT32 x, INT32 y);
	
	virtual void OnComponentUpdate(void);

	virtual UINT32 OnSave(i3ResourceFile* pResFile);
	virtual UINT32 OnLoad(i3ResourceFile* pResFile);

private:
	i3List m_listDropTarget;
};

#endif //_i3GuiDragDropManager_h