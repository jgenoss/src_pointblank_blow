#if !defined( __I3_INSTANCE_PROXY_H)
#define __I3_INSTANCE_PROXY_H

#include "i3PersistantElement.h"

class I3_EXPORT_BASE i3InstanceProxy : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3InstanceProxy, i3PersistantElement);
protected:
	// Persist members
	i3::rc_string			m_strProxyClass;

	// Volatile members
	i3PersistantElement *	m_pInst = nullptr;

public:
	virtual ~i3InstanceProxy(void);

	const i3::rc_string&		getProxyClassName(void)	const	{ return m_strProxyClass; }
	void						SetProxyClassName( const char * pszName);

	i3PersistantElement *	getInstance(void)				{ return m_pInst; }
	void					setInstance( i3PersistantElement * pObj)
	{
		I3_REF_CHANGE( m_pInst, pObj);
	}

	virtual void	OnQueryStrings( i3::vector<i3::rc_string>& StrList) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
