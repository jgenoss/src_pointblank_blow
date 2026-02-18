#if !defined( __I3_ACTION_SET_WORLD_SECTION_H)
#define __I3_ACTION_SET_WORLD_SECTION_H

#include "i3GameResDefine.h"

class I3_EXPORT_FRAMEWORK i3ActionSetWorldSection : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionSetWorldSection, i3Action);
protected:
	INT32				m_idxSection = -1;

	// Volatile members

public:
	INT32			getWorldSection(void)					{ return m_idxSection; }
	void			setWorldSection( INT32 idx)				{ m_idxSection = idx; }

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

class I3_EXPORT_FRAMEWORK i3ActionSetMapSector : public i3Action
{
	I3_EXPORT_CLASS_DEFINE( i3ActionSetMapSector, i3Action);
protected:
	INT32				m_idxSector = -1;

	// Volatile members

public:
	INT32			getMapSector(void)					{ return m_idxSector; }
	void			setMapSector( INT32 idx)			{ m_idxSector = idx; }

	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};


#endif
