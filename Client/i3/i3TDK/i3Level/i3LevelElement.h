#if !defined( __I3_Level_ELEMENT_H)
#define __I3_Level_ELEMENT_H

class i3LevelScene;
class i3LevelControl;

class I3_EXPORT_TDK i3LevelElement : public i3GameObjRef
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3LevelElement, i3GameObjRef);
	
public:	
	virtual void	OnAttachScene(i3LevelScene * pScene) {}
	virtual void	OnDetachScene(i3LevelScene * pScene) {}

	virtual void	OnPrepareSerialize(void) {}

	virtual void	OnAfterPaste(void) {}
};

#endif