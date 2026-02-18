#if !defined( __I3_Level_PROPERTY_CONTAINER_H)
#define __I3_Level_PROPERTY_CONTAINER_H

#include "i3LevelElement.h"

class i3LevelProperty;

class I3_EXPORT_TDK i3LevelPropertyContainer : public i3LevelElement
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3LevelPropertyContainer, i3LevelElement);

protected:
	i3::vector<i3LevelProperty*>	m_PropertyList;

	void			AddProperty( i3LevelProperty * pProp);
	void			RemoveProperty( i3LevelProperty * pProp);
	void			RemoveProperty( INT32 idx);
	INT32			FindPropertyByName( const char * pszName);
	INT32			GetPropertyIndex( i3LevelProperty * pProp);
	void			RemoveAllProperties(void);
	INT32			GetPropertyCount(void)							{ return (INT32)m_PropertyList.size(); }
	i3LevelProperty *	GetProperty( INT32 idx)							{ return m_PropertyList[idx]; }

public:
	virtual void	OnChangeProperty( i3LevelProperty * pProp);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif