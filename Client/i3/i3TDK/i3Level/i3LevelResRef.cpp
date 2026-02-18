#include "stdafx.h"
#include "i3LevelResRef.h"

I3_CLASS_INSTANCE( i3LevelResRef);

i3LevelResRef::~i3LevelResRef(void)
{
	I3_SAFE_RELEASE( m_pRes);
}

void i3LevelResRef::BindScene( i3LevelScene * pScene)
{
	if( m_pRes != nullptr)
	{
		m_pRes->OnChangeScene( pScene);
	}

	INT32 i;
	i3LevelResRef * pChild;

	for( i = 0; i < getChildCount(); i++)
	{
		pChild = (i3LevelResRef *) getChild( i);

		pChild->BindScene( pScene);
	}
}

void i3LevelResRef::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pRes != nullptr)
	{
		m_pRes->OnBuildObjectList( List);
	}

	i3TreeNode::OnBuildObjectList( List);
}

#if defined(I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct RES_REF
	{
		UINT8		m_ID[4] = { 'L', 'R', 'R', '1' };
		OBJREF		m_pRes = 0;
		UINT32		m_IconStyle = 0;
		UINT32		pad[7] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3LevelResRef::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::RES_REF data;

	result = i3TreeNode::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_pRes		=	pResFile->GetObjectPersistID( m_pRes);
	data.m_IconStyle =	getIconStyle();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3LevelResRef::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::RES_REF	data;

	result = i3TreeNode::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pRes != 0)
	{
		i3LevelRes * pRes = (i3LevelRes *) pResFile->FindObjectByID( data.m_pRes);
		I3ASSERT( pRes != nullptr);

		setRes( pRes);
	}

	setIconStyle( data.m_IconStyle);

	return result;
}
