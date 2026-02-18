#if !defined( __I3_TDK_PROPERTY_CONTAINER_H)
#define __I3_TDK_PROPERTY_CONTAINER_H

#include "i3Base.h"
#include "i3TDKObjectBase.h"

class i3TDKProperty;

class I3_EXPORT_TDK i3TDKPropertyContainer : public i3TDKObjectBase
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3TDKPropertyContainer, i3TDKObjectBase);

protected:
	i3::vector<i3TDKProperty*>	m_PropertyList;

protected:
	void			DeleteAllProperties(void);
	
public:
	i3TDKPropertyContainer(void) {}
	virtual ~i3TDKPropertyContainer(void);

	void			AddProperty( i3TDKProperty * pProperty);
	void			RemoveProperty( INT32 Index);
	void			RemoveProperty( char * pszName);
	void			RemoveProperty( i3TDKProperty * pProperty);
	i3TDKProperty *	GetProperty( int Index)
	{
		I3ASSERT( Index >= 0);
		I3ASSERT( Index < (INT32)m_PropertyList.size());

		return m_PropertyList[ Index];
	}

	INT32				GetPropertyCount(void)								{ return (INT32)m_PropertyList.size(); }
	i3TDKProperty *	FindProperty( const char * pszPropertyName);
	INT32			GetPropertyIndex( i3TDKProperty * pProperty);

	virtual void	CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef = TRUE);

	// 하위 Property가 변경되었을 때, 그 사실을 알리기 위한 함수.
	// 일반적으로 자식 Property들에서 자신의 Owner에게 자신의 변경사항을 알리기 위해 이 함수를 호출한다.
	virtual	void	OnChangeProperty( i3TDKProperty * pProperty);

	// Owner가 자신의 Property들에게 자신의 값이 변경되었기 때문에 각 Property들이 Update되어야 함을 알리기 위함 함수.
	virtual void	OnUpdateData(void);

	// Override from i3PersistantElement
	virtual	void		OnQueryStrings( i3::vector<i3::rc_string>& StrList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif