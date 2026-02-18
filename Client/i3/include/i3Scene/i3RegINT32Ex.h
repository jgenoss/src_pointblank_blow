#if !defined( __I3_REG_INT32EX_H__)
#define __I3_REG_INT32EX_H__

#include "i3RegINT32.h"

class i3RegKey;

class I3_EXPORT_SCENE	i3RegINT32Ex :	public i3RegINT32
{
	I3_EXPORT_CLASS_DEFINE( i3RegINT32Ex, i3RegINT32);

private:
	i3RegKey* m_pRegKey;			// [initialize at constructor]

public:
	i3RegINT32Ex();
	virtual ~i3RegINT32Ex();

	i3RegKey* GetRegKey() { return m_pRegKey; }

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32  OnLoad( i3ResourceFile * pResFile) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
};

#endif //#if !defined( __I3_REG_INT32EX_H__)