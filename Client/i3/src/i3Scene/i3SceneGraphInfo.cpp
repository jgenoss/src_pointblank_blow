#include "i3SceneDef.h"
#include "i3SceneGraphInfo.h"
#include "i3SceneUtil.h"

I3_CLASS_INSTANCE( i3SceneGraphInfo);

i3SceneGraphInfo::~i3SceneGraphInfo(void)
{
	I3_SAFE_RELEASE( m_pCommonSg);
	I3_SAFE_RELEASE( m_pInstanceSg);
}


void i3SceneGraphInfo::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3ResourceObject::CopyTo( pDest, method);

	i3SceneGraphInfo * pInfo = (i3SceneGraphInfo *) pDest;

	if( method == I3_COPY_REF)
	{
		pInfo->setCommonSg( getCommonSg());
		pInfo->setInstanceSg( getInstanceSg());
	}
	else
	{
		i3Node * pClone;

		pInfo->setCommonSg( getCommonSg());

		if( getInstanceSg() != nullptr)
		{
			pClone = i3Scene::CreateClone( getInstanceSg(), method);

			pInfo->setInstanceSg( pClone);
		}
	}
	//
	pInfo->m_bExternTextureSkipped = m_bExternTextureSkipped;		// 복사시 정보 추가..
	//
}

void i3SceneGraphInfo::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( IsExternRes() == false)
	{
		if( getCommonSg() != nullptr)
		{
			getCommonSg()->OnBuildObjectList( List);
		}

		if( getInstanceSg() != nullptr)
		{
			getInstanceSg()->OnBuildObjectList( List);
		}
	}

	i3ResourceObject::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 SGINFO
	{
		UINT8		m_ID[4] = { 'S', 'G', 'I', '1' };
		OBJREF64	m_pCommonSg = 0;
		OBJREF64	m_pInstanceSg = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3SceneGraphInfo::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::SGINFO data;

	Result = i3ResourceObject::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_pCommonSg = (OBJREF64) pResFile->GetObjectPersistID( getCommonSg());
	data.m_pInstanceSg = (OBJREF64) pResFile->GetObjectPersistID( getInstanceSg());

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3SceneGraphInfo::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::SGINFO data;
	i3Node * pNode;

	Result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_pCommonSg != 0)
	{
		pNode = (i3Node *) pResFile->FindObjectByID( (OBJREF) data.m_pCommonSg);
		I3ASSERT( pNode != nullptr);

		setCommonSg( pNode);
	}

	if( data.m_pInstanceSg != 0)
	{
		pNode = (i3Node *) pResFile->FindObjectByID( (OBJREF) data.m_pInstanceSg);
		I3ASSERT( pNode != nullptr);

		setInstanceSg( pNode);
	}

	return Rc;
}
bool i3SceneGraphInfo::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3ResourceObject::OnSaveXML(pFile, pXML);

	if (bResult == false)
		return false;

	pXML->addAttr("CommonSg", (OBJREF64)pFile->GetObjectPersistID(getCommonSg()));
	pXML->addAttr("InstanceSg", (OBJREF64)pFile->GetObjectPersistID(getInstanceSg()));

	return bResult;
}

bool i3SceneGraphInfo::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3ResourceObject::OnLoadXML(pFile, pXML);

	if (bResult == false)
		return false;
	OBJREF64 objref64;
	pXML->getAttr("CommonSg", &objref64);
	if (objref64 != 0)
	{
		i3Node* pNode = (i3Node *)pFile->FindObjectByID(OBJREF(objref64));
		I3ASSERT(pNode != nullptr);

		setCommonSg(pNode);
	}

	pXML->getAttr("InstanceSg", &objref64);
	if (objref64 != 0)
	{
		i3Node* pNode = (i3Node *)pFile->FindObjectByID(OBJREF(objref64));
		I3ASSERT(pNode != nullptr);

		setInstanceSg(pNode);
	}

	return bResult;
}