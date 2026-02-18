#include "i3OptPCH.h"
#include "i3OptConvertTextureFormat.h"

I3_CLASS_INSTANCE( i3OptConvertTextureFormat);

i3OptConvertTextureFormat::i3OptConvertTextureFormat(void)
{
	m_Class = CLASS_TEXTURE;
}

void i3OptConvertTextureFormat::FindTextureCoordMinMax( i3VertexArray * pDest, i3VertexArray * pSrc )
{
	INT32 i = 0;
	INT32 j = 0;
	i3VertexFormat format;

	m_rTextureCoordMin_U = 1.0f;
	m_rTextureCoordMax_U = 0.0f;
	m_rTextureCoordMin_V = 1.0f;
	m_rTextureCoordMax_V = 0.0f;

	format = * pDest->GetFormat();

	// Texture Coord를 돌면서 U,V의 최대최소값을 찾습니다.
	// 이값은 후에 Texture좌표의 Nomalize와 Scale에 사용되어집니다.
	for( i = 0; i < (INT32) pSrc->GetCount(); i++)
	{
		for( j = 0; j < format.GetTextureCoordSetCount(); j++)
		{
			VEC2D vec;

			pSrc->GetTextureCoord( j, i, &vec);

			m_rTextureCoordMin_U = MIN(m_rTextureCoordMin_U, i3Vector::GetX(&vec));
			m_rTextureCoordMax_U = MAX(m_rTextureCoordMax_U, i3Vector::GetX(&vec));

			m_rTextureCoordMin_V = MIN(m_rTextureCoordMin_V, i3Vector::GetY(&vec));
			m_rTextureCoordMax_V = MAX(m_rTextureCoordMax_V, i3Vector::GetY(&vec));
		}
	}
}

void i3OptConvertTextureFormat::ConvertTextureFormat( i3VertexArray * pDest, i3VertexArray * pSrc, INT32 which, INT32 index )
{
	VEC2D vec;
	pSrc->GetTextureCoord( which, index, &vec);

	if(m_rTextureCoordMax_U > 1.0f || m_rTextureCoordMax_V > 1.0f) 
	{
		i3Vector::SetX(&vec, (i3Vector::GetX(&vec) + abs(m_rTextureCoordMin_U)) / (m_rTextureCoordMax_U - m_rTextureCoordMin_U) );
		i3Vector::SetY(&vec, (i3Vector::GetY(&vec) + abs(m_rTextureCoordMin_V)) / (m_rTextureCoordMax_V - m_rTextureCoordMin_V) );
	}

	pDest->SetTextureCoord( which, index, &vec);
}

// 2개의 좌표(U, V)만을 지원하는 함수입니다.
void i3OptConvertTextureFormat::SetTextureCoordScale( i3Node* pNode )
{
	if(m_rTextureCoordMax_U > 1.0f || m_rTextureCoordMax_V > 1.0f ) 
	{
		i3Node* pParentNode;

		i3AttrSet* pTexAttrSet = i3AttrSet::new_object_ref();
		i3TextureCoordScaleAttr *pTexAttr = i3TextureCoordScaleAttr::new_object_ref();

		VEC3D vec;
		i3Vector::Set(&vec, m_rTextureCoordMax_U, m_rTextureCoordMax_V, 1.0f);
		pTexAttr->SetScaleFactor( &vec);
		pTexAttr->SetOutputElementCount(2);
		pTexAttrSet->AddAttr( (i3RenderAttr*)pTexAttr );

		pParentNode = pNode->GetParent();
		pNode->GetParent()->AddChild(pTexAttrSet);					
		pTexAttrSet->AddChild(pNode);
		pParentNode->RemoveChild(pNode);
	}
}
