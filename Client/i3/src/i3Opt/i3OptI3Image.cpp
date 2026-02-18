#include "i3OptPCH.h"
#include "i3OptI3Image.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3OptI3Image);

i3OptI3Image::i3OptI3Image(void)
{
	m_Class = CLASS_TEXTURE;
}

UINT32	i3OptI3Image::GetColorIndexCount(i3Texture* pSrcTex)
{
	INT Count = 0;

	if( pSrcTex->GetClut(0) != nullptr)
	{
		i3::vector<bool*> TestList;
//		TestList.SetOnceMode();
		INT PixelBits = i3Gfx::GetImageFormatBitCount( pSrcTex->GetPersistFormat());

		bool* bArr = new bool[PixelBits*32];
		char* pData = pSrcTex->Lock(0);

		for(UINT i = 0; i < pSrcTex->GetHeight(); i++)
		{
			for(UINT j = 0; j < pSrcTex->GetWidth(); j++)
			{
				TestList.push_back(&bArr[(*pData)]);
				pData++;
			}
		}
		pSrcTex->Unlock( 0);

		i3::vu::remove_duplicates(TestList);

		Count = INT(TestList.size());
		I3_SAFE_DELETE_ARRAY( bArr );
		TestList.clear();
	}

	return Count;	
}

i3Texture* i3OptI3Image::MakeI3I(i3Texture* pSrcTex)
{
	i3Texture* pDestTex;
	char * pSrcData, * pDestData;
	I3G_IMAGE_FORMAT format;

	if( pSrcTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED)
		return pSrcTex;

	if( (pSrcTex->GetFormat() & ~I3G_IMAGE_FORMAT_MASK_PLATFORM) == I3G_IMAGE_FORMAT_I4)
		return pSrcTex;

	if(GetColorIndexCount(pSrcTex) <= 16 && GetColorIndexCount(pSrcTex) != 0) 
	{
		format = I3G_IMAGE_FORMAT_I4;
	}
	else 
	{	
		return pSrcTex;
	}

	pDestTex = i3Texture::new_object();
	pDestTex->Create( pSrcTex->GetWidth(), pSrcTex->GetHeight(), 1, format, 0);

	{
		i3Clut * pDestClut, * pSrcClut;
		INT32 i;

		pSrcClut = pSrcTex->GetClut( 0);
		pDestClut = i3Clut::new_object_ref();

		pDestClut->Create( 16);

		for( i = 0; i < 16; i++)
		{
			COLORREAL col;

			pSrcClut->GetColor( i, &col);
			pDestClut->SetColor( i, &col);
		}

		pDestTex->SetClut( 0, pDestClut);
	}

	pSrcData = pSrcTex->Lock(0);
		
	// Runtime Data
	{
		pDestData = pDestTex->Lock( 0);

		i3Image_Convert( pSrcTex->GetFormat(), pSrcData, pSrcTex->GetWidth(), pSrcTex->GetHeight(), 1, pSrcTex->GetClut( 0),
						pDestTex->GetFormat(), pDestData, 1, pDestTex->GetClut(0) );

		pDestTex->Unlock( 0);
	}

	// Persistent Data
	if( pDestTex->GetPersistFormat() != pDestTex->GetFormat())
	{
		//INT32 hMem, Sz;
		INT32 Sz;

		Sz = i3Gfx::CalcRasterBufferSize( pDestTex->GetWidth(), pDestTex->GetHeight(), 1, pDestTex->GetPersistFormat());
		
//		hMem = i3MemAlloc(Sz); 
		pDestData = (char *) i3MemAlloc(Sz);

		i3Image_Convert( pSrcTex->GetFormat(), pSrcData, pSrcTex->GetWidth(), pSrcTex->GetHeight(), 1, pSrcTex->GetClut( 0),
						pDestTex->GetPersistFormat(), pDestData, 1, pDestTex->GetClut(0));

		pDestTex->SetPersistData( 0, (INT8 *) pDestData);
	}

	pSrcTex->Unlock( 0);

	return pDestTex;
}

// 이 함수의 매개변수 첫번째는 i3Texture*형의 배열형태가 와야합니다.
i3Texture* i3OptI3Image::MakeI3IMipmap(i3Texture** pSrcTex, UINT MipmapLevelCount)
{
	UINT MaxWidth = 0;
	UINT MaxHeight = 0;
	i3Texture* pDestTex;
	I3G_IMAGE_FORMAT format;
	char * pSrcData, * pDestData;
	UINT i;


	// CLUT에서 사용되는 인덱스의 갯수가 16개 미만이라면, 4bit Format으로 변환합니다.
	UINT32 nColorCount = GetColorIndexCount(pSrcTex[0]);
	if( ( 0 < nColorCount ) && ( nColorCount <= 16 ) )
	{
		format = I3G_IMAGE_FORMAT_I4;
	}
	else 
	{	
		format = pSrcTex[0]->GetFormat();
	}


	// 밉맵배치를 데이터 사이즈에 맞추어 재배치합니다.(가장 큰사이즈순서대로 0레벨부터 배치)
	for( i = 0; i<MipmapLevelCount; i++)
	{
		for(UINT j = i; j < MipmapLevelCount; j++)
		{
			if(i == j) continue;

			if(pSrcTex[i]->GetDataSize(0) <= pSrcTex[j]->GetDataSize(0)) 
			{
				i3Texture* pSwapTex; 

				pSwapTex =  pSrcTex[i];
				pSrcTex[i] =  pSrcTex[j];
				pSrcTex[j] =  pSwapTex;			
			}
		}
	}

	UINT width = MaxWidth = pSrcTex[0]->GetWidth();
	UINT height = MaxHeight = pSrcTex[0]->GetHeight();

	// PSP에서는 사용할수 없다는 경고메세지를 출력합니다.
#if defined(I3G_PSP)
	if(MaxWidth > 512 || MaxHeight > 512) 
	{
		MessageBoxA("This Image is not Suppot PSP");
	}
#endif

	// 밉맵 레벨이 맞지 않을경우 함수를 반환합니다.
	for(i = 1; i < MipmapLevelCount; i++)
	{
		width = width>>1;
		height = height>>1;

		if(pSrcTex[i]->GetWidth() != width || pSrcTex[i]->GetHeight() != height) 
		{
			for(i = 0; i < MipmapLevelCount; i++)
			{
				I3_MUST_RELEASE(pSrcTex[i]);
			}

			I3ASSERT_0;
		}
	}

	// 밉맵 레벨에 맞추어 텍스쳐를 생성합니다.
	pDestTex = i3Texture::new_object();
	pDestTex->Create( MaxWidth, MaxHeight, MipmapLevelCount, format, 0);

	// 각 밉맵 레벨에 따라 텍스쳐를 Setting합니다.
	for(i = 0; i < MipmapLevelCount; i++)
	{
		pDestTex->SetClut( i, pSrcTex[i]->GetClut(0));

		pSrcData = pSrcTex[i]->Lock(0);
		pDestData = pDestTex->Lock( i);

		i3Image_Convert( pSrcTex[i]->GetFormat(), pSrcData, pSrcTex[i]->GetWidth(), pSrcTex[i]->GetHeight(), 1, pSrcTex[i]->GetClut( 0),
						 pDestTex->GetFormat(), pDestData, 1, pDestTex->GetClut(i) );

		pSrcTex[i]->Unlock( 0);
		pDestTex->Unlock( i);

		I3_MUST_RELEASE(pSrcTex[i]);
	}
	
	// 생성되어진 텍스쳐를 반환합니다.
	return pDestTex;
}

void i3OptI3Image::Trace( i3Node * pRoot)
{
	i3::vector<i3RenderAttr*> BindList;
	i3::vector<i3Texture*> TexList;
	i3TextureBindAttr * pTexBind;
	i3Texture * pTex, * pNewTex;

	i3Scene::FindAttrByExactType( pRoot, i3TextureBindAttr::static_meta(), BindList);

	// 모든 Texture를 우선 구한다.
	for(size_t i = 0; i < BindList.size(); i++)
	{
		pTexBind = (i3TextureBindAttr *) BindList[i];

		pTex = pTexBind->GetTexture();

		if( pTex != nullptr)
			TexList.push_back( pTex);
		else
			I3TRACE( "SDFSDF\n");
	}
	
	i3::vu::remove_duplicates(TexList);

	for( size_t i = 0; i < TexList.size(); i++)
	{
		pTex = TexList[i];

		pNewTex = MakeI3I( pTex);
		if( pNewTex != pTex)
		{
			if( pTex->hasName())
				pNewTex->SetName( pTex->GetName());

			ReplaceTextureBind( BindList, pNewTex, pTex);

			I3_MUST_RELEASE(pNewTex);
		}
	}

}

void i3OptI3Image::ReplaceTextureBind( i3::vector<i3RenderAttr*>& BindList, i3Texture * pNewTex, i3Texture * pSrcTex)
{
	
	i3TextureBindAttr * pTexBind;

	for(size_t i = 0; i < BindList.size(); i++)
	{
		pTexBind = (i3TextureBindAttr *) BindList[i];

		if( pTexBind->GetTexture() == pSrcTex)
		{
			pTexBind->SetTexture( pNewTex);
		}
	}
}
