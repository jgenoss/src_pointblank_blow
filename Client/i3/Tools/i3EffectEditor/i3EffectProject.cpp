#include "stdafx.h"
#include "i3EffectProject.h"

I3_CLASS_INSTANCE( i3EffectProject); //, i3PersistantElement);

i3EffectProject::i3EffectProject(void)
{
	m_pWorld = NULL;
	m_pSky = NULL;
	m_pModel = NULL;
}

i3EffectProject::~i3EffectProject(void)
{
	I3_SAFE_RELEASE( m_pWorld);
	I3_SAFE_RELEASE( m_pSky);
	I3_SAFE_RELEASE( m_pModel);

	RemoveAllTimeSeq();
}

void i3EffectProject::SetWorld( i3Node * pNode)
{
	if( pNode != NULL)
		pNode->AddRef();

	I3_SAFE_RELEASE( m_pWorld);

	m_pWorld = pNode;
}

void i3EffectProject::SetSky( i3Node * pNode)
{
	if( pNode != NULL)
		pNode->AddRef();

	I3_SAFE_RELEASE( m_pSky);

	m_pSky = pNode;
}

void i3EffectProject::SetModel( i3Node * pNode)
{
	if( pNode != NULL)
		pNode->AddRef();

	I3_SAFE_RELEASE( m_pModel);

	m_pModel = pNode;
}

void i3EffectProject::AddTimeSeq( i3TimeSequence * pTimeSeq)
{
	pTimeSeq->AddRef();

	m_TimeSeqList.push_back( pTimeSeq);
}

void i3EffectProject::RemoveAllTimeSeq(void)
{
	
	i3TimeSequence * pTimeSeq;

	for(size_t i = 0; i < m_TimeSeqList.size(); i++)
	{
		pTimeSeq = m_TimeSeqList[i];

		pTimeSeq->Release();
	}

	m_TimeSeqList.clear();
}

void i3EffectProject::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	
	i3TimeSequence * pTimeSeq;

	if( m_pWorld != NULL)
		m_pWorld->OnBuildObjectList( List);

	if( m_pSky != NULL)
		m_pSky->OnBuildObjectList( List);

	if( m_pModel != NULL)
		m_pModel->OnBuildObjectList( List);

	for(size_t i = 0; i < m_TimeSeqList.size(); i++)
	{
		pTimeSeq = m_TimeSeqList[i];

		pTimeSeq->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct EFFECT_PROJECT
	{
		INT8		m_ID[4] = { 'E', 'P', 'J', '1' };
		OBJREF		m_pWorld = 0;
		OBJREF		m_pSky = 0;
		UINT32		m_TimeSeqCount = 0;
		OBJREF		m_pModel = 0;
		INT8		m_Reserved[124] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3EffectProject::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0, i;
	i3Stream * pStream = pResFile->GetStream();
	pack::EFFECT_PROJECT proj;

	proj.m_pWorld		= (OBJREF) pResFile->GetObjectPersistID( m_pWorld);
	proj.m_pSky			= (OBJREF) pResFile->GetObjectPersistID( m_pSky);
	proj.m_TimeSeqCount = m_TimeSeqList.size();
	proj.m_pModel		= (OBJREF) pResFile->GetObjectPersistID( m_pModel);

	Rc = pStream->Write( &proj, sizeof(proj));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3EffectProject::OnSave()", "Could not write project data.");
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0; i < m_TimeSeqList.size(); i++)
	{
		OBJREF ref = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *) m_TimeSeqList[i]);

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3EffectProject::OnSave()", "Could not write time sequence node reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3EffectProject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0, i;
	i3Stream * pStream = pResFile->GetStream();
	pack::EFFECT_PROJECT proj;

	Rc = pStream->Read( &proj, sizeof(proj));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3EffectProject::OnLoad()", "Could not read project data.");
		return STREAM_ERR;
	}
	Result += Rc;

	// World
	if( proj.m_pWorld != 0)
	{
		i3Node * pNode = (i3Node *) pResFile->FindObjectByID( proj.m_pWorld);
		I3ASSERT( pNode != NULL);

		SetWorld( pNode);
	}

	// Sky
	if( proj.m_pSky != 0)
	{
		i3Node * pNode = (i3Node *) pResFile->FindObjectByID( proj.m_pSky);
		I3ASSERT( pNode != NULL);

		SetSky( pNode);
	}

	// Model
	if( proj.m_pModel != 0)
	{
		i3Node * pNode = (i3Node *) pResFile->FindObjectByID( proj.m_pModel);
		I3ASSERT( pNode != NULL);

		SetModel( pNode);
	}

	for( i = 0; i < proj.m_TimeSeqCount; i++)
	{
		i3TimeSequence * pSeq;
		OBJREF ref;

		Rc = pStream->Read( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3EffectProject::OnLoad()", "Could not read time sequence node reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		pSeq = (i3TimeSequence *) pResFile->FindObjectByID( ref);
		I3ASSERT( pSeq != NULL);

		AddTimeSeq( pSeq);
	}

	return Result;
}

bool i3EffectProject::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr("World", (OBJREF)pFile->GetObjectPersistID(m_pWorld));
	pXML->addAttr("Sky", (OBJREF)pFile->GetObjectPersistID(m_pSky));
	pXML->addAttr("TimeSeqCount", m_TimeSeqList.size());
	pXML->addAttr("Model", (OBJREF)pFile->GetObjectPersistID(m_pModel));

	char szTemp[MAX_PATH];
	for (size_t i = 0; i < m_TimeSeqList.size(); i++)
	{
		i3::sprintf(szTemp, "TimeSeqList%d", i);
		pXML->addAttr(szTemp, (OBJREF)pFile->GetObjectPersistID((i3PersistantElement *)m_TimeSeqList[i]));
	}

	return true;
}

bool i3EffectProject::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 iVal;

	OBJREF objref;
	pXML->getAttr("World", &objref);
	if (objref > 0)
	{
		m_pWorld = (i3Node *)pFile->FindObjectByID(objref);
		I3ASSERT(objref != NULL);
	}

	pXML->getAttr("Sky", &objref);
	if (objref > 0)
	{
		m_pSky = (i3Node *)pFile->FindObjectByID(objref);
		I3ASSERT(objref != NULL);
	}
	UINT32 nSeqCount;
	pXML->getAttr("TimeSeqCount", &nSeqCount);

	pXML->getAttr("Model", &objref);
	if (objref > 0)
	{
		m_pModel = (i3Node *)pFile->FindObjectByID(objref);
		I3ASSERT(objref != NULL);
	}

	char szTemp[MAX_PATH];
	for (INT32 i = 0; i < nSeqCount; i++)
	{
		i3::sprintf(szTemp, "TimeSeqList%d", i);
		pXML->getAttr(szTemp, &objref);
		i3TimeSequence* pTimeSeq = (i3TimeSequence *)pFile->FindObjectByID(objref);
		I3ASSERT(pTimeSeq != NULL);
		AddTimeSeq(pTimeSeq);
	}
	return true;
}