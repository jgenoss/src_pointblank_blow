#include "i3OptPCH.h"
#include "i3OptExtractAnim.h"

I3_CLASS_INSTANCE( i3OptExtractAnim, i3SceneOptimizer);

i3OptExtractAnim::i3OptExtractAnim(void)
{
	m_szOutputPath[0] = 0;
}

i3OptExtractAnim::~i3OptExtractAnim(void)
{
}

BOOL i3OptExtractAnim::OnNode( i3Node * pNode)
{
	if( pNode->IsTypeOf( i3Transform::GetClassMeta()))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		if( pTrans->IsAnimated())
		{
			i3TransformSource * pTransSrc = pTrans->GetTransformSource(0);

			if( pTrans->GetName() != NULL)
			{
				if( pTransSrc->IsTypeOf( i3TransformSequence::GetClassMeta()))
				{
					m_NodeList.Add( pNode);
				}
			}
		}
	}
	else if( pNode->IsTypeOf( i3Skin::GetClassMeta()))
	{
		i3Skin * pBody = (i3Skin *) pNode;

		if( pBody->GetCurrentAnim() != NULL)
		{
			// i3Skin이 Animation을 가지고 있는 경우에는 다른 모든 경우를
			// 무시하고 i3Skin이 가지고 있는 Animation을 저장하도록 한다.
			m_NodeList.Clear();

			m_NodeList.Add( pBody);
			m_bSkin = true;

			return FALSE;
		}
	}
	else if( pNode->IsTypeOf( i3Body::GetClassMeta()))
	{
		i3Body * pBody = (i3Body *) pNode;

		if( pBody->GetCurrentAnim() != NULL)
		{
			m_NodeList.Clear();

			m_NodeList.Add( pBody);
			m_bSkin = true;
		}
	}

	return TRUE;
}

void i3OptExtractAnim::Trace( i3Node * pRoot)
{
	// 파일명이 지정되어있지 않다면 아무런 동작도 하지 않는다.
	if( m_szOutputPath[0] == 0)
		return;

	m_bSkin = false;
	

	i3SceneOptimizer::Trace( pRoot);

	{
		i3Animation * pAnim = NULL;
		INT32 i;

		if( m_bSkin == true)
		{
			i3Node * pNode = (i3Node *) m_NodeList.Items[0];

			if( pNode->IsTypeOf( i3Skin::GetClassMeta()))
			{
				i3Skin * pBody = (i3Skin *) m_NodeList.Items[0];

				pAnim = pBody->GetCurrentAnim();	
			}
			else
			{
				i3Body * pBody = (i3Body *) pNode;

				I3ASSERT( pNode->IsTypeOf( i3Body::GetClassMeta()));

				pAnim = pBody->GetCurrentAnim();
			}

			pAnim->AddRef();
		}
		else
		{
			if( m_NodeList.GetCount() > 0)
			{
				pAnim = i3Animation::NewObject();

				if( pAnim->Create( m_NodeList.GetCount()) == FALSE)
				{
					I3FATAL( "Could not create animation.");
					pAnim->Release();
					return;
				}

				for( i = 0; i < m_NodeList.GetCount(); i++)
				{
					i3Transform * pTrans = (i3Transform *) m_NodeList.Items[i];
					i3TransformSequence * pTransSeq = (i3TransformSequence *) pTrans->GetTransformSource(0);

					pAnim->SetTrack( i, pTransSeq, (char *) pTrans->GetName());
				}
			}
		}

		// Animation을 파일로 저장한다.

		if( pAnim != NULL)
		{
			if( pAnim->SaveToFile( m_szOutputPath) == FALSE)
			{
				I3FATAL( "Could not create %s animation file\n", m_szOutputPath);
			}

			pAnim->Release();
		}
	}
}

void i3OptExtractAnim::SetProperty( char * pszField, char * pszValue)
{
	char szI3aFile[256] = "";

	if( i3String::Compare( pszField, "Output") == 0)
	{
		//	파일 확장자를 'i3a'로 바꾼다.
		i3String::SwapExt( szI3aFile, pszValue, "i3a");

		setOutputPath( szI3aFile);
	}
}
