#include "StdAfx.h"

#include "GuiToolProject.h"
#include "i3GuiEditorDef.h"
#include "i3Base/smart_ptr/make_shared.h"

I3_CLASS_INSTANCE( CGuiToolProject); //, i3PersistantElement);

CGuiToolProject::CGuiToolProject(void)
{
	m_pRoot = NULL;
	m_pTemplate = NULL;
}

CGuiToolProject::~CGuiToolProject(void)
{
	I3_SAFE_RELEASE( m_pRoot );
	I3_SAFE_RELEASE( m_pTemplate );
}

void CGuiToolProject::SetRoot( i3GameNode * pNode)
{
	if( pNode != NULL )
	{
		pNode->AddRef();
	}

	I3_SAFE_RELEASE( m_pRoot );

	m_pRoot = pNode;
}


void CGuiToolProject::SetTemplate( i3GuiControlTemplate * pTemplate )
{
	if( pTemplate != NULL )
	{
		pTemplate->AddRef();
	}

	I3_SAFE_RELEASE( m_pTemplate );

	m_pTemplate = pTemplate;
}

void CGuiToolProject::SetTextureList( const i3::shared_ptr<i3::vector<i3GuiToolImageElement*> >& pResList)
{
	m_pTextureList = pResList;
}


void	CGuiToolProject::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pTextureList != NULL )
	{
		const size_t	nCount = m_pTextureList->size();
		
		for(size_t i = 0; i< nCount; ++i)
		{
			i3GuiToolImageElement * pElm = (*m_pTextureList)[i];
			i3Texture * pTex = pElm->getTexture();
			if( pTex != NULL )
			{
				pTex->OnBuildObjectList( List );
			}
		}
	}

	if(m_pTemplate != NULL )
	{
		m_pTemplate->OnBuildObjectList( List );
	}

	if( m_pRoot != NULL )
	{
		m_pRoot->OnBuildObjectList( List );
	}

	i3PersistantElement::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack( push, 4)
#endif

namespace pack
{
	struct GUITOOL_PROJECT
	{
		char		m_ID[4] = { 'G', 'P', 'J', '1' };
		INT32		m_nResCount = 0;
		OBJREF64	m_pTemplate = 0;
		OBJREF64	m_pRoot = 0;
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack( pop)
#endif

UINT32	CGuiToolProject::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUITOOL_PROJECT	proj;
		
	if( m_pTextureList != NULL )
	{
		proj.m_nResCount = (INT32)m_pTextureList->size();
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
			i3GuiToolImageElement * pElm = (*m_pTextureList)[i];
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
	pack::GUITOOL_PROJECT proj;

//	List * pList = List::new_object();
	i3::shared_ptr<i3::vector<i3GuiToolImageElement*> > pList = i3::make_shared< i3::vector<i3GuiToolImageElement*> >();

	Rc = pStream->Read( &proj, sizeof(proj));
	I3_CHKIO( Rc);
	Result += Rc;
	
	if( proj.m_pTemplate != 0)
	{
		i3GuiControlTemplate * pTemplate = (i3GuiControlTemplate*)pResFile->FindObjectByID( (OBJREF) proj.m_pTemplate );
		I3ASSERT( pTemplate != NULL);

		SetTemplate( pTemplate );
	}

	if( proj.m_pRoot != 0)
	{
		i3GameNode * pNode = (i3GameNode *)pResFile->FindObjectByID( (OBJREF)proj.m_pRoot);
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
				for( size_t j = 0; j < pList->size(); j++)
				{
					i3GuiToolImageElement * pElm = (*pList)[j];

					i3Texture * pTex = pElm->getTexture();
					I3ASSERT( pTex != NULL);

					if( i3::generic_is_iequal( pTexture->GetName(), pTex->GetName()) )
					{
						I3TRACE("SameTexture is in List\n");
						bIsInSameTex = TRUE;
					}
				}

				if( bIsInSameTex == FALSE)
				{
					i3GuiToolImageElement * pElm = i3GuiToolImageElement::new_object();
					pElm->SetTexture( pTexture);

					pTexture->AddRef();
					pList->push_back( pElm );
				}
			}
		}

		SetTextureList( pList );
	}

//	I3_SAFE_RELEASE( pList );

	return Result;
}
