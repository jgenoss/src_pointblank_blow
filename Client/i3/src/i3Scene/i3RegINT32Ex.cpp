#include "i3SceneDef.h"
#include "i3RegINT32Ex.h"
#include "i3RegKey.h"

I3_CLASS_INSTANCE( i3RegINT32Ex);

i3RegINT32Ex::i3RegINT32Ex()
{
	m_nDataType = I3REG_TYPE_INT32EX;
	m_pRegKey = i3RegKey::new_object();
}

i3RegINT32Ex::~i3RegINT32Ex()
{
	i3::destroy_instance(m_pRegKey);
}

UINT32	i3RegINT32Ex::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Result, Rc = 0;

	Result = i3RegINT32::OnSave( pResFile);
	I3_CHKIO( Result);

	Rc = m_pRegKey->OnSave(pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3RegINT32Ex::OnLoad( i3ResourceFile * pResFile)
{
	UINT32				Result, Rc = 0;

	Result = i3RegINT32::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = m_pRegKey->OnLoad(pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

void	i3RegINT32Ex::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegINT32::OnBuildObjectList(List);
	m_pRegKey->OnBuildObjectList(List);
}