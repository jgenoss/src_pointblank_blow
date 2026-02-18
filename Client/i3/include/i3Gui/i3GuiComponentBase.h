// ! praptor - 미완성입니다. 작동하지 않습니다.

#ifndef _i3GuiComponentBase_h
#define _i3GuiComponentBase_h

class I3_EXPORT_GUI i3GuiComponentBase: public i3PersistantElement
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE(i3GuiComponentBase, i3PersistantElement);

public:
	void SetContainer(i3GuiObjBase* pGuiContainer);
    virtual void OnComponentUpdate(void) = 0;

	virtual UINT32	OnSave(i3ResourceFile* pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile* pResFile) override;

private:
	i3GuiObjBase* m_pGuiContainer = nullptr;
};

#endif //_i3GuiComponentBase_h