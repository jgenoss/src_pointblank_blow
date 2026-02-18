#if !defined( __I3_LevelL_HITPOINT_H)
#define __I3_LevelL_HITPOINT_H

#include "i3LevelControl.h"

class i3LevelPath;

class I3_EXPORT_TDK i3LevelHitPoint : public i3LevelControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelHitPoint, i3LevelControl);

private:
	i3TextNodeDX*			m_pTextNode = nullptr;

public:
	i3LevelHitPoint(void);
	virtual ~i3LevelHitPoint(void);

	void			Create(void);
	void			SetName(char* pchName, VEC3D* vec);
	void			UpdateNode();
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
};

#endif