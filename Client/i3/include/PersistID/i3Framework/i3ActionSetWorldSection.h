#if !defined( __I3_ACTION_SET_WORLD_SECTION_H)
#define __I3_ACTION_SET_WORLD_SECTION_H

#include "i3GameResDefine.h"

class I3_EXPORT_FRAMEWORK i3ActionSetWorldSection : public i3Action
{
	I3_CLASS_DEFINE( i3ActionSetWorldSection);
protected:
	INT32				m_idxSection;

	// Volatile members

public:
	i3ActionSetWorldSection(void);
	virtual ~i3ActionSetWorldSection(void);

	INT32			getWorldSection(void)					{ return m_idxSection; }
	void			setWorldSection( INT32 idx)				{ m_idxSection = idx; }

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
