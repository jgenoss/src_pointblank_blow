#include "i3OptPCH.h"
#include "i3OptExternRef.h"

#include "i3Base/string/ext/extract_filename.h"

//I3_CLASS_INSTANCE( i3OptExternRef, i3SceneOptimizer);
I3_CLASS_INSTANCE( i3OptExternRef);

i3OptExternRef::i3OptExternRef(void)
{
	m_Class = CLASS_NA;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}


bool i3OptExternRef::OnNode( i3Node * pNode, i3Node * pParent)
{
	INT32 i;

	if( i3::kind_of<i3AttrSet*>(pNode))
	{
		i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			i3RenderAttr * pAttr = pAttrSet->GetAttr( i);

			if( i3::kind_of<i3TextureBindAttr* >(pAttr))
			{
				i3TextureBindAttr * pTexBind = (i3TextureBindAttr *) pAttr;

				if( pTexBind->GetTexture() != nullptr)
					m_TexList.push_back( pTexBind->GetTexture());
			}
		}
	}
	else if( i3::kind_of<i3Particle* >(pNode))
	{
		i3Particle * pParticle = (i3Particle *) pNode;

		if( pParticle->getTexture() != nullptr)
		{
			m_TexList.push_back( pParticle->getTexture());
		}

		if( pParticle->getNormalMap() != nullptr)
		{
			m_TexList.push_back( pParticle->getNormalMap());
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3OptExternRef::Trace( i3Node * pRoot)
{
	
	char szName[MAX_PATH], szFull[MAX_PATH];

	m_TexList.clear();

	i3SceneOptimizer::Trace( pRoot);

	for(size_t i = 0; i < m_TexList.size(); i++)
	{
		i3Texture * pTex = m_TexList[ i];

		if( pTex->hasName())
		{
			i3::extract_filetitle(pTex->GetNameString(), szName);

			sprintf( szFull, "%s/%s.i3i", m_szBasePath, szName);
		}
		else
		{
			sprintf( szFull, "%s/%p_%dx%d.i3i", m_szBasePath, pTex, pTex->GetWidth(), pTex->GetHeight());
		}

		pTex->SetName( szFull);

		if( m_pUserProc != nullptr)
		{
			m_pUserProc( pTex, m_pUserData);
		}

		pTex->SetExternRes( m_bExternTexture == true);
	}
}
