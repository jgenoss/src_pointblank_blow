#if !defined( __I3_Level_ICON_H)
#define __I3_Level_ICON_H

#include "i3LevelElement3D.h"
#include "i3LevelControl.h"

class I3_EXPORT_TDK i3LevelIcon : public i3LevelControl
{
	I3_EXPORT_CLASS_DEFINE(i3LevelIcon, i3LevelControl);

public:
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif