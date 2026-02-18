#include "i3OptPCH.h"
#include "i3OptConvertImgTo4BitGray.h"
#include "i3OptI3Image.h"
#include "i3OptPSPSwizzledImage.h"
#include "i3Base/itl/container_util.h"

I3_CLASS_INSTANCE( i3OptConvertImgTo4BitGray);

namespace
{
	struct INDEXCOUNT_DATA
	{
		INT32	nIndex = 0;//인덱스
		INT32	nCount = 0;//빈도(Count)
	};
}

i3OptConvertImgTo4BitGray::i3OptConvertImgTo4BitGray(void)
{
	m_Class = CLASS_TEXTURE;
}

//넘어온 이미지를 이용하여 4BIT_GRAY이미지를 만들어준다.
i3Texture * i3OptConvertImgTo4BitGray::ConvertImgTo4BitGray( i3Texture * pSrcTex , bool bSwizzled)
{
	I3ASSERT( pSrcTex != nullptr );

	i3Texture * pDestTex = ConvertImgTo8Gray( pSrcTex );
	i3Texture * pDestTex4 = Convert8to4( pDestTex );
	I3_SAFE_RELEASE( pDestTex );

	if( bSwizzled )
	{
		i3OptPSPSwizzledImage opt;
		pDestTex = opt.MakeSwizzledImage( pDestTex4 );
		return pDestTex;
	}
	else	return pDestTex4;
}

i3Texture * i3OptConvertImgTo4BitGray::ConvertImgTo8Gray( i3Texture * pSrcTex )
{
	I3ASSERT( pSrcTex != nullptr );
	
	INT32 nIndexLevelCount[256];
	INT32 nNewIndex[256];
	memset( nIndexLevelCount, 0, sizeof(nIndexLevelCount) );
	memset( nNewIndex, 0, sizeof(nNewIndex) );

	//
	i3Texture * pDestTex = i3Texture::new_object();
    pDestTex->Create( pSrcTex->GetWidth(), pSrcTex->GetHeight(), 1, I3G_IMAGE_FORMAT_I8, 0);

	UINT8	* pDestBuffer = (UINT8*)pDestTex->Lock(0);
	UINT8	* pSrcBuffer  = (UINT8*)pSrcTex->Lock(0);

	for( UINT32	y = 0 ; y < pSrcTex->GetHeight() ; ++y )
	{
		for( UINT32	x = 0 ; x < pSrcTex->GetWidth() ; ++x )
		{
			UINT8 R,G,B;
			R = pSrcBuffer[0];
			G = pSrcBuffer[1];
			B = pSrcBuffer[2];

			*pDestBuffer = (((R*30) + (G*59) + (B*11)) /100);
			nIndexLevelCount[*pDestBuffer]++;
			pSrcBuffer += 3;
			pDestBuffer++;
		}
	}
	pSrcTex->Unlock( 0);
	pDestTex->Unlock( 0);

	//빈도가 0이상인것들을 가려내서 최종 룩업테이블에 들어갈 인덱스를 정한다.
	i3::vector<INT32*>	pCountList;
	for( INT32	i = 0 ; i < 256 ; i++ )
	{
		if( nIndexLevelCount[i] > 0 )
		{
			INT32 * nIndex = new INT32;
			*nIndex = i;
			pCountList.push_back( nIndex );
			nNewIndex[i] = INT32(pCountList.size()) - 1;//새로이 지정된 인덱스저장
		}
	}

	//Make Color LookUp Table
	i3Clut * pClut = i3Clut::new_object();
	pClut->Create( INT32(pCountList.size()) );

	//I3TRACE( "===============================\n" );
	//I3TRACE( "8BIT GRAY COLOR LOOKUP TABLE\n" );
	//I3TRACE( "===============================\n" );
	//I3TRACE( "INDEX  | R | G | B | A | COUNT|\n");
	//I3TRACE( "===============================\n" );
	for( size_t	i = 0 ;i < pCountList.size() ; i++ )
	{
		I3COLOR Col;
		INT32	* pAlpha = pCountList[i];

		i3Color::Set( &Col , 255, 255, 255, (UINT8)*pAlpha );
		pClut->SetColor( i , &Col );
		//I3TRACE("%d\t\t%d\t%d\t%d\t%d\t%d\n" , i , i3Color::GetR(&Col), i3Color::GetG(&Col), i3Color::GetB(&Col), i3Color::GetA(&Col) , nIndexLevelCount[*pAlpha] );
	}
	//I3TRACE( "=================================\n" );

	pDestTex->SetClut(0 , pClut );

	//새로 지정된 룩업테이블의 값으로 픽셀의 CLUT인덱스를 재설정해준다.
	pDestBuffer = (UINT8*)pDestTex->Lock(0);
	for( UINT32	y = 0 ; y < pSrcTex->GetHeight() ; ++y )
	{
		for( UINT32	x = 0 ; x < pSrcTex->GetWidth() ; ++x )
		{
			*pDestBuffer = (UINT8)nNewIndex[*pDestBuffer];
			pDestBuffer++;
		}
	}
	pDestTex->Unlock( 0);

//	pCountList.SafeDeleteAll<INT32>();
	i3::cu::for_each_delete(pCountList);

	return pDestTex;
}

i3Texture * i3OptConvertImgTo4BitGray::Convert8to4( i3Texture * pSrcTex )
{
	I3ASSERT( pSrcTex != nullptr );

	i3Texture * pDestTex = i3Texture::new_object();
    pDestTex->Create( pSrcTex->GetWidth(), pSrcTex->GetHeight(), 1, I3G_IMAGE_FORMAT_I4, 0);

	ConvertClut8To4( pDestTex , pSrcTex );
	ConvertPixel8To4( pDestTex , pSrcTex);
	return pDestTex;
}

bool i3OptConvertImgTo4BitGray::ConvertClut8To4( i3Texture * pDestTex , i3Texture * pSrcTex )
{
	I3ASSERT( pSrcTex != nullptr );
	I3ASSERT( pDestTex != nullptr );

	i3Clut * pSrcClut	= (i3Clut*)pSrcTex->GetClut(0);
	INT32	nColorCount = pSrcClut->GetColorCount();

	//INT32	memIdx				= i3MemAlloc( sizeof(INT32) * nColorCount );
	INT32 * nIndexLevelCount	= (INT32*)i3MemAlloc( sizeof(INT32) * nColorCount );
	memset( nIndexLevelCount, 0, sizeof(INT32) * nColorCount );

	//그레이 스케일에서 가진 색상값의 빈도를 카운트 한다.
	UINT8 * pSrc = (UINT8*)pSrcTex->Lock(0);
	for( UINT32 y = 0; y < pSrcTex->GetHeight() ; y++ )
	{
		for( UINT32 x = 0; x < pSrcTex->GetWidth() ; x++ )
		{
			nIndexLevelCount[*pSrc]++;
			pSrc++;
		}
	}
	pSrcTex->Unlock( 0);

	//memIdx = i3MemAlloc( sizeof(INDEXCOUNT_DATA) * nColorCount );
	INDEXCOUNT_DATA * pIndexData = (INDEXCOUNT_DATA*)i3MemAlloc( sizeof(INDEXCOUNT_DATA) * nColorCount );
	//빈도가 0이상인것들이 가려내져있지 않은경우가 있을수도 있으므로.
	i3::vector<INDEXCOUNT_DATA*>	pIndexList;
	for( INT32	i = 0 ; i < nColorCount ; i++ )
	{
		pIndexData[i].nIndex = i;
		pIndexData[i].nCount = nIndexLevelCount[i];
		if( nIndexLevelCount[i] > 0 )
		{
			pIndexList.push_back(&pIndexData[i]);
		}
	}

	//최적화 계산 필요///////////////////////////////////////////////////////////////////
	//리스트에 들어있는 갯수가 16을 초과하는경우 빈도가 낮은 순으로 제거한다.
	if( pIndexList.size() > 16 )
	{
		size_t	ListCount = pIndexList.size();
 		while( ListCount > 16 )
		{
			INT32				nMinIndex = 0;
			INDEXCOUNT_DATA *	MinData = pIndexList[0];
			INDEXCOUNT_DATA *	pData	= nullptr;
			I3ASSERT( MinData != nullptr );

			//제일 낮은 빈도의 것을 고른다.
			for( size_t	i = 1 ;i < ListCount ; i++ )
			{
				pData = pIndexList[i];
				if( pData )
				{
					if( MinData->nCount > pData->nCount )
					{
						MinData = pData;
						nMinIndex = i;
					}
				}
			}
			
		//	pIndexList.Delete(nMinIndex);
			pIndexList.erase(pIndexList.begin() + nMinIndex);
			ListCount = pIndexList.size();
		}
	}

	// CLUT TABLE을 만들어준다.
	////////////////////////////////////////////////////////////////////////////////////////
	i3Clut * pDestClut = i3Clut::new_object();
	pDestClut->Create( INT32(pIndexList.size()) );

	//I3TRACE( "============================\n" );
	//I3TRACE( "4BIT GRAY COLOR LOOKUP TABLE\n" );
	//I3TRACE( "============================\n" );
	//I3TRACE( "INDEX  | R | G | B | A |\n");
	//I3TRACE( "========================\n" );
	for( size_t	i = 0 ;i < pIndexList.size() ; i++ )
	{
		I3COLOR COL;
		INDEXCOUNT_DATA * pListData = pIndexList[i];

		pSrcClut->GetColor( pListData->nIndex , &COL );
		pDestClut->SetColor( i , &COL );
		pListData = nullptr;
		//I3TRACE("%d\t\t%d\t%d\t%d\t%d\n" , i , i3Color::GetR(&COL), i3Color::GetG(&COL), i3Color::GetB(&COL), i3Color::GetA(&COL) );
	}
	//I3TRACE( "========================\n" );
	pDestTex->SetClut( 0, pDestClut );

	//바뀐 CLUT의 인덱스를 적용
	////////////////////////////////////////////////////////////////////////////////////////
	// RELEASE
	pIndexList.clear();
	i3MemFree( pIndexData );
	i3MemFree( nIndexLevelCount );

	return true;
}

bool  i3OptConvertImgTo4BitGray::ConvertPixel8To4( i3Texture * pDestTex, i3Texture * pSrcTex )
{
	I3ASSERT( pSrcTex != nullptr );

	i3Clut * pSrcClut	= ( i3Clut *) pSrcTex->GetClut(0);
	i3Clut * pDestClut	= ( i3Clut *) pDestTex->GetClut(0);

	UINT8 * pSrc  = (UINT8*)pSrcTex->Lock(0);
	UINT8 * pDest = (UINT8*)pDestTex->Lock(0);

	INT32	nWidth = pSrcTex->GetWidth() >> 1;//두 픽셀을 하나의 픽셀로 합치기 위해
	//Set Color to Gray Index
	for( INT32 y = 0; y < (INT32)pSrcTex->GetHeight(); y++ )
	{
		UINT8 LEVEL1,LEVEL2;

		for( INT32 x = 0; x < nWidth; x++ )
		{
			LEVEL1 = GetIndexAtClut4( pSrcClut, pDestClut , *pSrc );

			pSrc++;

			LEVEL2 = GetIndexAtClut4( pSrcClut, pDestClut , *pSrc);
			*pDest =( (LEVEL2 << 4) | LEVEL1 );

			pSrc++;
			pDest++;
		}
	}

	pSrcTex->Unlock( 0);
	pDestTex->Unlock( 0);
	return true;
}

//이부분도 최적화 대상
//주어진 4bit Clut인덱스중에서 8bit Clut인덱스가 위치하는곳을 리턴
UINT8 i3OptConvertImgTo4BitGray::GetIndexAtClut4( i3Clut * pSrcClut , i3Clut * pDestClut , UINT8	nIndexAt8 )
{
	I3COLOR col;
	UINT8 LEVELSRC,LEVELDEST,LEVELBEFORE;

	pSrcClut->GetColor( nIndexAt8 , &col );
	LEVELSRC = i3Color::GetA( &col );

	LEVELBEFORE = 0;
	//테이블중에 맞는 인덱스가 있는지 확인한다.
	for( UINT8	i = 0 ; i < pDestClut->GetColorCount() ; i++ )
	{
		pDestClut->GetColor( i , &col );
		LEVELDEST = i3Color::GetA( &col );
			
		if( LEVELSRC == LEVELDEST )	return i;
		
		if( LEVELSRC < LEVELDEST )
		{
			if( i == 0 )	return 0;
			else
			{
				//일치하는 값이 없는경우 좌,우에있는 인덱스들과의 거리를 계산하여 가까운곳의 인덱스를 리턴
				if( LEVELDEST - LEVELSRC > LEVELSRC - LEVELBEFORE )
				{
					return i - 1;
				}
				else
				{
					return i;
				}
			}
		}
		else
		{
			LEVELBEFORE = LEVELDEST;
		}
	}

	return 0;
}