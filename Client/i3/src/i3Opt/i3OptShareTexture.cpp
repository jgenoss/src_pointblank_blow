#include "i3OptPCH.h"
#include "i3OptShareTexture.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3OptShareTexture);

i3OptShareTexture::i3OptShareTexture(void)
{
	m_Class = CLASS_TEXTURE;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

void i3OptShareTexture::_ReplaceTexBind( i3::vector<i3RenderAttr*>& List, i3Texture * pFindTex, i3Texture * pNewTex)
{
	i3TextureBindAttr * pBind;

	for(size_t i = 0; i < List.size(); i++)
	{
		pBind = (i3TextureBindAttr *) List[i];

		if( pBind->GetTexture() == pFindTex)
		{
			pBind->SetTexture( pNewTex);
		}
	}
}

void i3OptShareTexture::Trace( i3Node * pRoot)
{
	i3::vector<i3Texture*> TexList;
	i3::vector<UINT32>		CRCList;
	i3::vector<i3RenderAttr*> BindList;
	INT32 cnt = 0;
	i3TextureBindAttr * pTexBind;
	i3Texture * pTex;
	UINT32 crc, crc1, crc2;

	i3Scene::FindAttrByType( pRoot, i3TextureBindAttr::static_meta(), BindList);

	// 존재하는 Texture들을 검사하고, 각 Texture들의 CRC Code를
	// 생성한다.
	for(size_t i = 0; i < BindList.size(); i++)
	{
		pTexBind = (i3TextureBindAttr *) BindList[i];

		pTex = pTexBind->GetTexture();

		if( pTex != nullptr)
		{
			if( i3::kind_of<i3TextureCube* >(pTex))
				continue;

			if( i3::vu::index_of(TexList, pTex) == TexList.size() )
			{
				// TexList에 추가하면서 CRC 코드도 구한다.
				TexList.push_back( pTex);

				pTex->Lock( 0);

				crc = CRC32( 0xFFFFFFFF, (UINT8 *) pTex->GetLockedBuffer( 0), pTex->GetDataSize());

				CRCList.push_back(crc);

				pTex->Unlock( 0);
			}
		}
	}

	// 발견된 Texture들의 CRC를 비교해, 동일한 Texture가 있는지 확인한다.

	if (!CRCList.empty())
	{

		for(size_t i = 0; i < CRCList.size() - 1; i++)
		{
			crc1 = CRCList[i];

			for(size_t j = i + 1; j < CRCList.size(); j++)
			{
				crc2 = CRCList[j];

				if( crc1 == crc2)
				{
					// 같은 Pixel Data를 가진 이미지이다.
					i3Texture * pTex1, * pTex2;

					pTex1 = TexList[i];
					pTex2 = TexList[j];

					if( pTex1->GetLevelCount() > pTex2->GetLevelCount())
					{
						// pTex2를 제거한다.
						_ReplaceTexBind( BindList, pTex2, pTex1);
					}
					else
					{
						// pTex1을 제거한다.
						_ReplaceTexBind( BindList, pTex1, pTex2);

						CRCList[ i] = crc2;
						TexList[ i] = pTex2;

						crc1 = crc2;
						pTex1 = pTex2;

					}

					CRCList.erase(CRCList.begin() + j);
					TexList.erase(TexList.begin() + j);
					j--;
					cnt++;
				}
			}
		}
	}

	I3TRACE( "%d 개 Texture가 제거되었습니다.\n", cnt);
}
