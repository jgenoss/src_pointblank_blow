#include "StdAfx.h"

#include "GuiToolProject.h"
#include "i3UIEditorDef.h"
#include "i3UIControlTemplate.h"

I3_CLASS_INSTANCE( CGuiToolProject, i3PersistantElement);

CGuiToolProject::CGuiToolProject(void)
{
	m_nVersion = 2;
	m_pRoot = NULL;
	m_pTemplate = NULL;
	m_pTextureList = NULL;
}

CGuiToolProject::~CGuiToolProject(void)
{
	I3_SAFE_RELEASE( m_pRoot );
	I3_SAFE_RELEASE( m_pTemplate );

	I3_SAFE_RELEASE( m_pTextureList );
}

void CGuiToolProject::SetRoot( i3UIRoot * pNode)
{
	I3_REF_CHANGE( m_pRoot, pNode);
}

void CGuiToolProject::SetTemplate( i3UIControlTemplate * pTemplate )
{
	I3_REF_CHANGE( m_pTemplate, pTemplate);
}

void CGuiToolProject::SetTextureList( i3List * pResList)
{
	I3_REF_CHANGE( m_pTextureList, pResList);
}

void	CGuiToolProject::OnBuildObjectList( i3List * pList)
{
	if( m_pTextureList != NULL )
	{
		INT32	nCount = m_pTextureList->GetCount();
		INT32	i;
		for( i = 0; i< nCount; ++i)
		{
			i3GuiToolImageElement * pElm = (i3GuiToolImageElement*)m_pTextureList->GetItem( i);
			i3Texture * pTex = pElm->getTexture();
			if( pTex != NULL )
			{
				pTex->OnBuildObjectList( pList );
			}
		}
	}

	if(m_pTemplate != NULL )
	{
		m_pTemplate->OnBuildObjectList( pList );
	}

	if( m_pRoot != NULL )
	{
		m_pRoot->OnBuildObjectList( pList );
	}

	i3PersistantElement::OnBuildObjectList( pList );
}

#if defined( I3_COMPILER_VC )
#pragma pack( push, 4)
#endif

typedef struct _tagi3GuiToolProject
{
	char		m_ID[4];
	INT32		m_nResCount;
	OBJREF64	m_pTemplate;
	OBJREF64	m_pRoot;
}I3_PERSIST_GUITOOL_PROJECT;

#if defined( I3_COMPILER_VC )
#pragma pack( pop)
#endif

UINT32	CGuiToolProject::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_GUITOOL_PROJECT	proj;

	memset( &proj, 0, sizeof( proj));
	strcpy( proj.m_ID, "GPJ1");
	
	if( m_pTextureList != NULL )
	{
		proj.m_nResCount = m_pTextureList->GetCount();
	}
	else
		proj.m_nResCount = 0;

	proj.m_pTemplate	= (OBJREF64)pResFile->GetObjectPersistID( m_pTemplate);
	proj.m_pRoot		= (OBJREF64)pResFile->GetObjectPersistID( m_pRoot);

	Rc = pStream->Write( &proj, sizeof( proj));
	I3_CHKIO( Rc);
	Result += Rc;

	if( proj.m_nResCount > 0)
	{
		INT32	i;
		for( i = 0; i< proj.m_nResCount; ++i)
		{
			i3GuiToolImageElement * pElm = (i3GuiToolImageElement*)m_pTextureList->GetItem( i);
			i3Texture * pTexture = pElm->getTexture();

			OBJREF64 ref = (OBJREF64)pResFile->GetObjectPersistID( pTexture);
			Rc = pStream->Write( &ref, sizeof( OBJREF64));
			I3_CHKIO( Rc);
			Result += Rc;
		}
	}

	return Result;
}

UINT32	CGuiToolProject::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc,Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_GUITOOL_PROJECT proj;

	i3List * pList = i3List::NewObject();

	Rc = pStream->Read( &proj, sizeof(proj));
	I3_CHKIO( Rc);
	Result += Rc;
	
	if( proj.m_pTemplate != 0)
	{
		i3UIControlTemplate * pTemplate = (i3UIControlTemplate*)pResFile->FindObjectByID( (OBJREF) proj.m_pTemplate );
		I3ASSERT( pTemplate != NULL);

		SetTemplate( pTemplate );
	}

	if( proj.m_pRoot != 0)
	{
		i3UIRoot * pNode = (i3UIRoot *)pResFile->FindObjectByID( (OBJREF)proj.m_pRoot);
		I3ASSERT( pNode != NULL );

		SetRoot( pNode );
	}

	if( proj.m_nResCount > 0)
	{
		for( INT32	i = 0; i < proj.m_nResCount; ++i)
		{
			OBJREF64 ref;
			Rc = pStream->Read( &ref, sizeof( OBJREF64));
			I3_CHKIO( Rc);
			Result += Rc;

			if( ref != 0)
			{
				i3Texture * pTexture = (i3Texture*)pResFile->FindObjectByID( (OBJREF)ref);
				I3ASSERT( pTexture != NULL);

				BOOL	bIsInSameTex = FALSE;
				for( INT32 j = 0; j < pList->GetCount(); j++)
				{
					i3GuiToolImageElement * pElm = (i3GuiToolImageElement*) pList->GetItem( j);

					i3Texture * pTex = pElm->getTexture();
					I3ASSERT( pTex != NULL);

					if( i3String::Compare( pTexture->GetName(), pTex->GetName()) == 0)
					{
						I3TRACE("SameTexture is in List\n");
						bIsInSameTex = TRUE;
					}
				}

				if( bIsInSameTex == FALSE)
				{
					i3GuiToolImageElement * pElm = i3GuiToolImageElement::NewObject();
					pElm->SetTexture( pTexture);

					pTexture->AddRef();
					pList->Add( pElm );
				}
			}
		}

		SetTextureList( pList );
	}

	I3_SAFE_RELEASE( pList );

	return Result;
}

bool CGuiToolProject::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr( "Version", m_nVersion);

	char conv[MAX_PATH];
	INT32 nTexCount = 0;
	if( m_pTextureList != NULL )
	{
		nTexCount = m_pTextureList->GetCount();
	}

	pXML->addAttr( "TextureCount", nTexCount);
	pXML->addAttr( "Template",	pFile->GetObjectPersistID( m_pTemplate));
	pXML->addAttr( "Root", pFile->GetObjectPersistID( m_pRoot));

	if( nTexCount > 0)
	{
		for(INT32 i = 0; i< nTexCount; ++i)
		{
			i3GuiToolImageElement * pElm = (i3GuiToolImageElement*)m_pTextureList->GetItem( i);

			i3Texture * pTexture = pElm->getTexture();

			INT32 iVal = pFile->GetObjectPersistID( pTexture);

			sprintf( conv, "item_%d", i);

			pXML->addAttr( conv, iVal);
		}
	}

	return true;
}

bool CGuiToolProject::OnLoadXML(  i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->getAttr( "Version", &m_nVersion);

	char conv[MAX_PATH];
	INT32 nTexCount = 0;
	INT32 iVal;

	pXML->getAttr( "TextureCount", &nTexCount);
	pXML->getAttr( "Template",	&iVal);

	if( iVal != 0)
	{
		i3UIControlTemplate * pTemplate = (i3UIControlTemplate * )pFile->FindObjectByID((OBJREF)iVal);
		I3ASSERT( pTemplate != NULL);

		SetTemplate( pTemplate );
	}

	pXML->getAttr( "Root", &iVal);
	if( iVal != 0)
	{
		i3UIRoot* pRoot = (i3UIRoot*)pFile->FindObjectByID( (OBJREF) iVal);
		I3ASSERT( pRoot != NULL);

		SetRoot( pRoot);
	}

	if( nTexCount > 0)
	{
		i3List * pUITexList = i3List::NewObjectRef();
		INT32 idxTexture;
		for( INT32 i = 0; i< nTexCount; ++i)
		{
			sprintf( conv, "item_%d", i);

			pXML->getAttr( conv, &idxTexture);

			if( idxTexture != 0)
			{
				i3Texture * pTexture = (i3Texture*)pFile->FindObjectByID( (OBJREF) idxTexture);
				I3ASSERT( pTexture != NULL);

				i3GuiToolImageElement * pElm = i3GuiToolImageElement::NewObject();
				pElm->SetTexture( pTexture);

				pUITexList->Add( pElm);
			}
		}

		SetTextureList( pUITexList);
	}

	return true;
}
