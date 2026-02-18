#if !defined( __I3_Level_CHARA_H)
#define __I3_Level_CHARA_H

#include "i3LevelElement3D.h"

class I3_EXPORT_TDK i3LevelChara : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE( i3LevelChara, i3LevelElement3D);

protected:
	i3Chara *		m_pChara = nullptr;

public:
	i3LevelChara(void);

	virtual void		BindRes( i3LevelRes * pRes) override;
	virtual i3GameObj *	CreateGameInstance(void) override;

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif