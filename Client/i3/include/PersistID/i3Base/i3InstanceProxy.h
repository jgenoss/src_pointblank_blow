#if !defined( __I3_INSTANCE_PROXY_H)
#define __I3_INSTANCE_PROXY_H

#include "i3PersistantElement.h"

class I3_EXPORT_BASE i3InstanceProxy : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3InstanceProxy);
protected:
	// Persist members
	char *					m_pszProxyClass;

	// Volatile members
	i3PersistantElement *	m_pInst;

public:
	i3InstanceProxy(void);
	virtual ~i3InstanceProxy(void);

	char *			getProxyClassName(void)					{ return m_pszProxyClass; }
	void			SetProxyClassName( const char * pszName);

	i3PersistantElement *	getInstance(void)				{ return m_pInst; }
	void					setInstance( i3PersistantElement * pObj)
	{
		I3_REF_CHANGE( m_pInst, pObj);
	}

	virtual void	OnQueryStrings( i3StringList * pStrList);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
