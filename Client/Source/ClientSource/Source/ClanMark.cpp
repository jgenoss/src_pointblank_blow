#include "pch.h"
#include "GlobalVariables.h"
#include "ClanMark.h"

#define		DUMP_MARK_FLAG(MarkFlag)	\
	do\
	{\
		INT32 nBackImageIdx = (markFlag >> 24) & 0xFF;\
		INT32 nShapeImageIdx = (markFlag >> 16) & 0xFF;\
		INT32 nBackColorIdx = (markFlag >> 8) & 0xFF;\
		INT32 nShapeColorIdx = (markFlag) & 0xFF;\
		I3PRINTLOG(I3LOG_NOTICE,"Clan Mark ID : [%p] [%d %d %d %d]", MarkFlag, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);\
	} while (false)



//	아래 모두 254(0xFE) 갯수를 넘어서는 안된다. komet (반드시 이미지 갯수와 일치해야 하며 함부로 변경해서는 안된다.)

#define _CLAN_MARK_COLOR_MAX			10		//	조합 컬러 갯수 (0xFF은 비클랜원 값이므로 클랜 마크 Index로 사용하면 안됨)


UINT32	GetUsingLayerPageCount(CLAN_MARK_LAYER_TYPE Type)
{
	INT32	ImageCount = 0;
	switch (Type)
	{
	case CLAN_MARK_LAYER_FRAME		:	ImageCount = CClanMarkManager::GetMarkBackImageCount();		break;
	case CLAN_MARK_LAYER_CONTENT	:	ImageCount = CClanMarkManager::GetMarkShapeImageCount();	break;
	default:	I3ASSERT_0;	break;
	}

	INT32 PageCount = ImageCount / (CLAN_MARK_COLUMN_MAX * CLAN_MARK_ROW_MAX) + 1;

	return PageCount;
}


struct	CLAN_MARK_IMAGE
{
	i3Texture *		pFilledText = nullptr;
	i3Texture *		pOutline = nullptr;
};


inline	INT32		CalcuImageIndex2TexIndex(INT32 ImageIndex)
{
#ifdef _DEBUG
	I3ASSERT(ImageIndex > 0);
#endif

	// 1 베이스 인덱스라서 -1합니다.
	return (ImageIndex - 1) % (CLAN_MARK_COLUMN_MAX * CLAN_MARK_ROW_MAX);
}

inline	INT32		CalcClanMarkTexPageFromImgIndex(INT32 ImageIndex)
{
#ifdef _DEBUG
	I3ASSERT(ImageIndex > 0);
#endif

	// 1 베이스 인덱스라서 -1합니다.
	return (ImageIndex - 1) / (CLAN_MARK_COLUMN_MAX * CLAN_MARK_ROW_MAX);
}


inline	POINT32		CalcuTex(INT32 ShapeIndex)
{
	I3ASSERT( (ShapeIndex - 1) >= 0 );
	POINT32		Pos;
	INT32		TexIndex = (ShapeIndex - 1) % (CLAN_MARK_COLUMN_MAX * CLAN_MARK_ROW_MAX);	//CalcuImageIndex2TexIndex(ShapeIndex);
	Pos.x	= (TexIndex) % CLAN_MARK_COLUMN_MAX;
	Pos.y	= (TexIndex) / CLAN_MARK_ROW_MAX;
	return Pos;
};


const char * g_ClanMarkDefaultTex[MAX_CLAN_MARK_SIZE] = 
{
	"Gui/Main/UI_Source04.i3i",			//	기본 마크 작은 사이즈
	"Gui/Main/UI_Source04.i3i"			//	기본 마크 중간 사이즈
};

const char * g_ClanMarkDefaultTex_V10[MAX_CLAN_MARK_SIZE] = 
{
	"Gui/Main/UI_Source04_V10.i3i",			//	기본 마크 작은 사이즈
	"Gui/Main/UI_Source04_V10.i3i"			//	기본 마크 중간 사이즈
};


//	마크 조합 색상표
const UINT8 g_ClanMarkUserColor[_CLAN_MARK_COLOR_MAX+1][4] = 
{
	{ 255,255,255,255},
	{ 121,23,0, 255 },		//	빨강
	{ 227,96,0, 255 },		//	주황
	{ 255,211,3, 255 },		//	노랑
	{ 196,213,0, 255 },		//	잔디
	{ 128,217,0, 255 },		//	연두
	{ 32,171,0, 255 },		//	초록
	{ 3,181,255, 255 },		//	밝은 파랑
	{ 0,96,227, 255 },		//	파랑
	{ 0,35,107, 255 },		//	짙은 파랑
	{ 115,37,117, 255 },	//	보라
};

//	바탕 테두리 색상표
const UINT8 g_ClanMarkBackFrameColor[_CLAN_MARK_COLOR_MAX+1][4] = 
{
	{ 255,255,255,255},
	{ 238,190,0,255 },
	{ 255,180,0,255 },
	{ 255,126,0,255 },
	{ 0,115,66,255 },
	{ 0,67,158,255 },
	{ 255,204,0,255 },
	{ 0,67,158,255 },
	{ 77,232,255,255 },
	{ 238,190,0,255 },
	{ 248,138,255,255 },
};

//	모양 테두리 색상표
const UINT8 g_ClanMarkShapeFrameColor[_CLAN_MARK_COLOR_MAX+1][4] = 
{
	{ 255,255,255,255},
	{ 255,190,45,255 },
	{ 255,210,0,255 },
	{ 95,0,0,255 },
	{ 75,85,0,255 },
	{ 0,67,158,255 },
	{ 0,70,40,255 },
	{ 0,45,85,255 },
	{ 80,190,255,255 },
	{ 240,255,5,255 },
	{ 0,195,205,255 },
};

I3_CLASS_INSTANCE( CClanMarkManager);//, i3ElementBase);

CClanMarkManager::CClanMarkManager(void)
{
	for(INT32 i=0; i<MAX_CLAN_MARK_SIZE; i++)
	{
		m_pCombiClanMarkTex[i] = nullptr;
		
		m_nInfoListCount[i] = 0;

		m_pClanMarkInfo[i] = nullptr;
		m_pLatestClanMarkInfo[i] = nullptr;
		m_pDefaultClanMarkTex[i] = nullptr;
		m_nDefaultClanMarkU[i] = 0;
		m_nDefaultClanMarkV[i] = 0;

		m_nDefaultClanMarkWH[i] = 0;
	
	}	
}

CClanMarkManager::~CClanMarkManager(void)
{
	for ( INT32 LayerType = 0 ; LayerType < MAX_CLAN_MARK_LAYER ; LayerType++ )
	{
		for ( INT32 SizeIndex = 0 ; SizeIndex < MAX_CLAN_MARK_SIZE ; ++SizeIndex )
		{
			for ( size_t i = 0 ; i < m_ClanMarkTex[LayerType][SizeIndex].size() ; ++i )
			{
				CLAN_MARK_IMAGE * pMarkImg = m_ClanMarkTex[LayerType][SizeIndex][i];
				I3_SAFE_RELEASE(pMarkImg->pFilledText);
				I3_SAFE_RELEASE(pMarkImg->pOutline);
				I3_SAFE_DELETE( pMarkImg);
			}
			m_ClanMarkTex[LayerType][SizeIndex].clear();
		}
	}


	for(INT32 i=0; i<MAX_CLAN_MARK_SIZE; i++)
	{
		I3MEM_SAFE_FREE( m_pClanMarkInfo[i]);

		m_nInfoListCount[i] = 0;

		m_pLatestClanMarkInfo[i] = nullptr;

		I3_SAFE_RELEASE( m_pCombiClanMarkTex[i]);
		I3_SAFE_RELEASE( m_pDefaultClanMarkTex[i]);		
	}
}

void CClanMarkManager::_ResetClanMarkInfo(CLAN_MARK_SIZE type)
{
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);

	INT32 w = 0, h = 0;
//	INT32 maxCount = 0;	
	INT32 maxWidth = 0, maxHeight = 0;
	INT32 surfaceWidth = CLAN_MARK_CREATE_TEX_SIZE;
	INT32 surfaceHeight = CLAN_MARK_CREATE_TEX_SIZE;

	switch(type)
	{
	case CLAN_MARK_SMALL:
		{
			w = h = CLAN_MARK_SMALL_PIXEL;
//			maxCount = CLAN_MARK_SMALL_MAX_COUNT;
		}
		break;
	case CLAN_MARK_MEDIUM:
		{
			w = h = CLAN_MARK_MID_PIXEL;
//			maxCount = CLAN_MARK_MID_MAX_COUNT;	
		}
		break;
	}				

	//	하나의 텍스처에서 담을 수 있는 최대 갯수의 마크의 넓이를 구한다.	
	while( maxWidth < surfaceWidth - w)
	{
		maxWidth += w;
	}
	
	while( maxHeight < surfaceHeight - h)
	{
		maxHeight += h;
	}

	if( maxHeight == 0)
		maxHeight = 64;

	//	하나의 텍스처에 담길수 있는 마크 정보를 초기화 합니다. 
	//	(u,v,w,h 값은 여기서 미리 계산해 둡니다. 다른 곳에서 uv값을 설정해서는 안됩니다.)
	INT32 maxCount = (INT32)m_pInfoList[type].size();
	if( maxCount > 0)
	{
		INT32 u = 0, v = 0;

		ClanMarkInfo * pArrayInfo = m_pInfoList[type][0];
		i3mem::FillZero(pArrayInfo, sizeof(ClanMarkInfo) * maxCount);
		
		for(INT32 cnt=0; cnt<maxCount; cnt++)
		{
			ClanMarkInfo * pNewInfo = (ClanMarkInfo*) pArrayInfo + cnt;

			u = (cnt * w) % maxWidth;
			v = ((cnt * w) / maxHeight) * h;
			
			if( u + w >= surfaceWidth || v + h >= surfaceHeight)
			{
				I3PRINTLOG(I3LOG_FATAL, "클랜 마크 이미지를 저장할 텍스처 크기 한도를 넘었네요.여기 들어오면 안되요. komet");
				return;
			}

			pNewInfo->_flag = 0;
			pNewInfo->_used = false;
			pNewInfo->_texU = (UINT16)u;
			pNewInfo->_texV = (UINT16)v;
			pNewInfo->_texW = (UINT16)w;
			pNewInfo->_texH = (UINT16)h;
			pNewInfo->_image = m_pCombiClanMarkTex[type];
		}
	}

	m_nInfoListCount[type] = 0;
}

void CClanMarkManager::Create(void)
{
	i3GameResTexture * pResTex = nullptr;
	i3ResourceManager * pResMng = g_pFramework->GetResourceManager();

	char TexFileDir[MAX_PATH] = "Chara/Clan";
	char TexFileName[MAX_PATH] = {};

	for ( INT32 LayerType = 0 ; LayerType < MAX_CLAN_MARK_LAYER ; LayerType++ )
	{
		const char * LayerName = "";
		switch(LayerType)
		{
		case CLAN_MARK_LAYER_FRAME		:	LayerName = "Frames";	break;
		case CLAN_MARK_LAYER_CONTENT	:	LayerName = "Contents";	break;
		}

		for ( INT32 SizeIndex = 0 ; SizeIndex < MAX_CLAN_MARK_SIZE ; ++SizeIndex )
		{
			const char * SizeName = "";
			switch(SizeIndex)
			{
			case CLAN_MARK_SMALL		:	SizeName = "Small";		break;
			case CLAN_MARK_MEDIUM		:	SizeName = "Medium";	break;
			}

			INT32 LayerPageCount = GetUsingLayerPageCount(static_cast<CLAN_MARK_LAYER_TYPE>(LayerType));
			for ( INT32 TexCount = 0; TexCount < LayerPageCount ; ++TexCount )
			{
				CLAN_MARK_IMAGE	* pMarkImage = new CLAN_MARK_IMAGE();
				MEMDUMP_NEW( pMarkImage, sizeof(CLAN_MARK_IMAGE));

				i3::snprintf(TexFileName, MAX_PATH, "%s/%s/%s_Filled_%02d.i3i", TexFileDir, LayerName, SizeName,  TexCount+1);
				pMarkImage->pFilledText	= pResMng->LoadTexture(TexFileName);
				if ( pMarkImage->pFilledText == nullptr )
				{
					I3PRINTLOG(I3LOG_FATAL, "could not found file : %s", TexFileName);
				}
				if ( pMarkImage->pFilledText->GetFormat() != I3G_IMAGE_FORMAT_BGRA_8888 )
				{
					I3PRINTLOG(I3LOG_FATAL, "can not support format! support type is BGRA_8888: %s", TexFileName);
				}

				i3::snprintf(TexFileName, MAX_PATH, "%s/%s/%s_Outline_%02d.i3i", TexFileDir, LayerName, SizeName,  TexCount+1);
				pMarkImage->pOutline		= pResMng->LoadTexture(TexFileName);
				I3ASSERT( pMarkImage->pOutline != nullptr && "텍스쳐를 못찾았습니다. 2");
				if ( pMarkImage->pOutline == nullptr )
				{
					I3PRINTLOG(I3LOG_FATAL, "could not found file : %s", TexFileName);
				}
				if ( pMarkImage->pOutline->GetFormat() != I3G_IMAGE_FORMAT_BGRA_8888 )
				{
					I3PRINTLOG(I3LOG_FATAL, "can not support format! support type is BGRA_8888: %s", TexFileName);
				}

				m_ClanMarkTex[LayerType][SizeIndex].push_back(pMarkImage);
			}
		}
	}

	//	기본 PB 클랜 마크
	{
		for(INT32 i=0; i<MAX_CLAN_MARK_SIZE; i++)
		{
			i3Texture * pTex = nullptr;

			i3::string pszPath;
			if( g_pEnvSet->m_nUIVersion == UFV_1_5)
			{
				pszPath = g_ClanMarkDefaultTex[i];
				pResTex = (i3GameResTexture*) pResMng->FindResourceByPath( pszPath.c_str());
			}
			else
			{
				pszPath = g_ClanMarkDefaultTex_V10[i];
				pResTex = (i3GameResTexture*) pResMng->FindResourceByPath(  pszPath.c_str());
			}

			if( pResTex == nullptr)
			{
				pTex = pResMng->LoadTexture(  pszPath.c_str());
			}
			else
			{
				pTex = pResTex->getTexture();
				I3_MUST_ADDREF(pTex);
			}

			I3ASSERT( pTex != nullptr);

			m_pDefaultClanMarkTex[i] = pTex;
		}

		//	PB 클랜 마크 작은사이즈 좌표
		m_nDefaultClanMarkU[0] = 174;
		m_nDefaultClanMarkV[0] = 2;
		m_nDefaultClanMarkWH[0] = 20;			// CLAN_MARK_SMALL_PIXEL 대신에 매직넘버가 직접 들어가네요??
		//	PB 클랜 마크 중간사이즈 좌표
		m_nDefaultClanMarkU[1] = 228;
		m_nDefaultClanMarkV[1] = 45;
		m_nDefaultClanMarkWH[1] = CLAN_MARK_MID_PIXEL;
	}

	//	조합 클랜 마크를 저장할 텍스처 한장씩만 생성
	for(INT32 i=0; i<MAX_CLAN_MARK_SIZE;	i++)
	{		
		INT32 max = 0;

		m_pCombiClanMarkTex[i] = i3Texture::new_object(); 
		I3ASSERT( m_pCombiClanMarkTex[i] != nullptr);
#if defined( I3_DEBUG)
		m_pCombiClanMarkTex[i]->SetName( "CombinClanMark");
#endif

		//	텍스처 생성
		m_pCombiClanMarkTex[i]->Create(CLAN_MARK_CREATE_TEX_SIZE, CLAN_MARK_CREATE_TEX_SIZE, 1, I3G_IMAGE_FORMAT_BGRA_8888, 0);
		
		switch(i)
		{
		case CLAN_MARK_SMALL:		
			{
				max = CLAN_MARK_SMALL_MAX_COUNT;	
			}
			break;
		case CLAN_MARK_MEDIUM:			
			{
				max = CLAN_MARK_MID_MAX_COUNT;
			}
			break;
		}				

		m_pClanMarkInfo[i] = (ClanMarkInfo*) i3MemAlloc(sizeof( ClanMarkInfo) * max);
		I3ASSERT( m_pClanMarkInfo[i] != nullptr);
		i3mem::FillZero( m_pClanMarkInfo[i], sizeof( ClanMarkInfo) * max);
		
		//	미리 갯수에 맞게 빈공간들을 만들어 둡니다. 이후 게임종료 이외에 노드를 삭제하거나 노드가 추가되서는 안됩니다.
		for(INT32 cnt=0; cnt<max; cnt++)
		{
			ClanMarkInfo * pNewInfo = (ClanMarkInfo*) m_pClanMarkInfo[i] + cnt;

			m_pInfoList[i].push_back( pNewInfo);
		}		

		//	uv 초기화
		_ResetClanMarkInfo((CLAN_MARK_SIZE) i);
	}
}

void CClanMarkManager::GetUserColorTable(INT32 idx, COLOR * pOut)
{
	I3ASSERT( pOut != nullptr);

	I3_BOUNDCHK(idx, CClanMarkManager::GetMarkColorCount() + 1);

	i3Color::Set( pOut, g_ClanMarkUserColor[idx][0], 
						g_ClanMarkUserColor[idx][1], 
						g_ClanMarkUserColor[idx][2], 
						g_ClanMarkUserColor[idx][3]);
}

void	CClanMarkManager::GetLayerColorTable(CLAN_MARK_LAYER_TYPE LayerType, INT32 idx, COLOR * pOut)
{
	I3ASSERT( pOut != nullptr);
			I3_BOUNDCHK(idx, CClanMarkManager::GetMarkColorCount() + 1);

	switch(LayerType)
	{
	case CLAN_MARK_LAYER_FRAME		:
		{
			i3Color::Set( pOut, g_ClanMarkBackFrameColor[idx][0], 
								g_ClanMarkBackFrameColor[idx][1], 
								g_ClanMarkBackFrameColor[idx][2], 
								g_ClanMarkBackFrameColor[idx][3]);
		}
		break;
	case CLAN_MARK_LAYER_CONTENT	:
		{

			i3Color::Set( pOut, g_ClanMarkShapeFrameColor[idx][0], 
								g_ClanMarkShapeFrameColor[idx][1], 
								g_ClanMarkShapeFrameColor[idx][2], 
								g_ClanMarkShapeFrameColor[idx][3]);
		}
		break;
	}
}


bool CClanMarkManager::SetClanMarkTex(i3GuiStatic * pControl, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx)
{
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);
	I3ASSERT( pControl != nullptr);

	//	모두 0이면 기본 클랜 마크로 셋팅합니다.
	if( nBackImageIdx == 0 && nShapeImageIdx == 0 && nBackColorIdx == 0 && nShapeColorIdx == 0)
	{
		return SetDefaultClanMarkTex(pControl, type);
	}
	//	비클랜원이다.
	//1else if( MAKE_CLANMARK_FLAG(nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx) == CLAN_MARK_FLAG_NONE)
	if( nBackImageIdx == 0xFF || nShapeImageIdx == 0xFF)
	{
		return false;
	}
	//	인덱스는 1 base 이다. 하나라도 0이면 인자가 잘못된 것입니다.
	else if( nBackImageIdx == 0 || nShapeImageIdx == 0 || nBackColorIdx == 0 || nShapeColorIdx == 0)
	{
		return false;
	}

	i3GuiImage * pImage = pControl->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);
	I3ASSERT( pImage != nullptr);

	//	클랜 마크 이미지
	ClanMarkInfo * pClanMarkInfo = GetClanMarkTex(type, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);
	I3ASSERT( pClanMarkInfo != nullptr);
	I3ASSERT( pClanMarkInfo->_image != nullptr);

	//	다른 클랜 마크 일경우만 셋팅하도록
	if( pImage->GetTexture() != pClanMarkInfo->_image || 
		pImage->getPixelU() != pClanMarkInfo->_texU ||
		pImage->getPixelV() != pClanMarkInfo->_texV ||
		pImage->getPixelWidth() != pClanMarkInfo->_texW ||
		pImage->getPixelHeight() != pClanMarkInfo->_texH	)
	{ 
		pImage->SetTexture(pClanMarkInfo->_image, pClanMarkInfo->_texU, pClanMarkInfo->_texV, pClanMarkInfo->_texW, pClanMarkInfo->_texH);

		pControl->SetAllShapeEnable(false);
		pControl->SetShapeEnable(GUI_SHAPE_OFFSET, true);
		pControl->UpdateRenderObjects();	//	Sprite를 강제 업데이트 하기위해...이거 없으면 GUI에서 텍스처 정보를 업데이트 안합니다. komet
		
	}

	return true;
}

bool CClanMarkManager::SetClanMarkTex(i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx)
{
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);
	I3ASSERT( pSprite != nullptr);

	//	모두 0이면 기본 클랜 마크로 셋팅합니다.
	if( nBackImageIdx == 0 && nShapeImageIdx == 0 && nBackColorIdx == 0 && nShapeColorIdx == 0)
	{
		return SetDefaultClanMarkTex(pSprite, nSpriteIdx, type);
	}
	//	비클랜원이다.
	else if( MAKE_CLANMARK_FLAG(nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx) == CLAN_MARK_FLAG_NONE)
	{
		return false;
	}
	//	인덱스는 1 base 이다. 하나라도 0이면 인자가 잘못된 것입니다.
	else if( nBackImageIdx == 0 || nShapeImageIdx == 0 || nBackColorIdx == 0 || nShapeColorIdx == 0)
	{
		return false;
	}

	//	클랜 마크 이미지
	ClanMarkInfo * pClanMarkInfo = GetClanMarkTex(type, nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);
	I3ASSERT( pClanMarkInfo != nullptr);
	I3ASSERT( pClanMarkInfo->_image != nullptr);

	I3ASSERT( pSprite->GetCount() > 0);

	VEC2D * pUV1 = pSprite->GetTextureCoord(nSpriteIdx, 0);
	VEC2D * pUV2 = pSprite->GetTextureCoord(nSpriteIdx, 1);

	INT32 u = (INT32)(pUV1->x * (REAL32) pClanMarkInfo->_image->GetWidth() - 0.5f);
	INT32 v = (INT32)(pUV1->y * (REAL32) pClanMarkInfo->_image->GetHeight() - 0.5f);
	INT32 w = (INT32)((pUV2->x * (REAL32) pClanMarkInfo->_image->GetWidth() + 0.5f) - u);
	INT32 h = (INT32)((pUV2->y * (REAL32) pClanMarkInfo->_image->GetHeight() + 0.5f) - v);

	//	다른 클랜 마크 일경우만 셋팅하도록
	if( u != pClanMarkInfo->_texU || v != pClanMarkInfo->_texV ||
		w != pClanMarkInfo->_texW || h != pClanMarkInfo->_texH)
	{
		pSprite->SetTexture(pClanMarkInfo->_image);
		pSprite->SetTextureCoord(nSpriteIdx, (REAL32)pClanMarkInfo->_texU, (REAL32)pClanMarkInfo->_texV, 
								(REAL32)pClanMarkInfo->_texU + pClanMarkInfo->_texW, 
								(REAL32)pClanMarkInfo->_texV + pClanMarkInfo->_texH);
	}

	return true;
}

bool CClanMarkManager::SetDefaultClanMarkTex( i3GuiStatic * pControl, CLAN_MARK_SIZE type)
{
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);
	I3ASSERT( m_pDefaultClanMarkTex[type] != nullptr);

	I3ASSERT( pControl != nullptr);

	i3GuiImage * pImage = pControl->GetControlData()->GetShape()->getImage(GUI_SHAPE_OFFSET);
	I3ASSERT( pImage != nullptr);

	INT32 u=0, v=0, w=0, h=0;
	switch(type)
	{
	case CLAN_MARK_SMALL:
		{
			u = m_nDefaultClanMarkU[0];
			v = m_nDefaultClanMarkV[0];
			//w = h = CLAN_MARK_SMALL_PIXEL;
			w = h = 20;
		}
		break;
	case CLAN_MARK_MEDIUM:
		{
			u = m_nDefaultClanMarkU[1];
			v = m_nDefaultClanMarkV[1];
			w = h = CLAN_MARK_MID_PIXEL;
		}
		break;
	default:
		return false;
	}
	
	if( pImage->GetTexture() != m_pDefaultClanMarkTex[type] || 
		pImage->getPixelU() != u ||
		pImage->getPixelV() != v ||
		pImage->getPixelWidth() != w ||
		pImage->getPixelHeight() != h)
	{
		pImage->SetTexture(m_pDefaultClanMarkTex[type], u, v, w, h);

		pControl->SetAllShapeEnable(false);
		pControl->SetShapeEnable(GUI_SHAPE_OFFSET, true);
		pControl->UpdateRenderObjects();	//	Sprite를 강제 업데이트 하기위해...이거 없으면 GUI에서 텍스처 정보를 업데이트 안합니다. komet
	}

	return true;
}

bool CClanMarkManager::SetDefaultClanMarkTex( i3Sprite2D * pSprite, INT32 nSpriteIdx, CLAN_MARK_SIZE type)
{
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);
	I3ASSERT( m_pDefaultClanMarkTex[type] != nullptr);

	I3ASSERT( pSprite != nullptr);

	INT32 texU=0, texV=0, texW=0, texH=0;
	switch(type)
	{
	case CLAN_MARK_SMALL:
		{
			texU = m_nDefaultClanMarkU[0];
			texV = m_nDefaultClanMarkV[0];
			//texW = texH = CLAN_MARK_SMALL_PIXEL;
			texW = texH = 20;
		}
		break;
	case CLAN_MARK_MEDIUM:
		{
			texU = m_nDefaultClanMarkU[1];
			texV = m_nDefaultClanMarkV[1];
			texW = texH = CLAN_MARK_MID_PIXEL;
		}
		break;
	default:
		return false;
	}

	I3ASSERT( pSprite->GetCount() > 0);

	VEC2D * pUV1 = pSprite->GetTextureCoord(nSpriteIdx, 0);
	VEC2D * pUV2 = pSprite->GetTextureCoord(nSpriteIdx, 1);

	INT32 u = (INT32)(pUV1->x * (REAL32) m_pDefaultClanMarkTex[type]->GetWidth() - 0.5f);
	INT32 v = (INT32)(pUV1->y * (REAL32) m_pDefaultClanMarkTex[type]->GetHeight() - 0.5f);
	INT32 w = (INT32)((pUV2->x * (REAL32) m_pDefaultClanMarkTex[type]->GetWidth() + 0.5f) - u);
	INT32 h = (INT32)((pUV2->y * (REAL32) m_pDefaultClanMarkTex[type]->GetHeight() + 0.5f) - v);

	//	다른 클랜 마크 일경우만 셋팅하도록
	if( u != texU || v != texV || w != texW || h != texH)
	{
		pSprite->SetTexture(m_pDefaultClanMarkTex[type]);
		pSprite->SetTextureCoord(nSpriteIdx, (REAL32)texU, (REAL32)texV, (REAL32)texU+texW, (REAL32)texV+texH);
	}

	return true;
}

ClanMarkInfo * CClanMarkManager::GetClanMarkTex(CLAN_MARK_SIZE type, INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx)
{
	I3ASSERT_ONCE(type >= 0 && type < MAX_CLAN_MARK_SIZE);

	I3ASSERT_ONCE(nBackImageIdx >= 0 && nBackImageIdx <= CClanMarkManager::GetMarkBackImageCount());
	I3ASSERT_ONCE(nShapeImageIdx >= 0 && nShapeImageIdx <= CClanMarkManager::GetMarkShapeImageCount());
	I3ASSERT_ONCE(nBackColorIdx >= 0 && nBackColorIdx <= CClanMarkManager::GetMarkColorCount());
	I3ASSERT_ONCE(nShapeColorIdx >= 0 && nShapeColorIdx <= CClanMarkManager::GetMarkColorCount());

	//	0이면 기본 클랜 마크로 인식
	if(nBackImageIdx == 0 || nShapeImageIdx == 0 || nBackColorIdx == 0 || nShapeColorIdx == 0)
	{
		return nullptr;
	}
	//	비클랜원이다.
	else if( MAKE_CLANMARK_FLAG(nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx) == CLAN_MARK_FLAG_NONE)
	{
		return nullptr;
	}

	//	인덱스를 가지고 flag로 변환
	UINT32 flag = MAKE_CLANMARK_FLAG(nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);

	//	클랜 마크 리스트에 등록되어 있는지 검사
	ClanMarkInfo * pInfo = _FindClanMarkInfo(type, flag);

	if( pInfo == nullptr)
	{		
		INT32 max = 0;
		switch(type)
		{
		case CLAN_MARK_SMALL:		max = CLAN_MARK_SMALL_MAX_COUNT;	break;
		case CLAN_MARK_MEDIUM:			max = CLAN_MARK_MID_MAX_COUNT;		break;
		}	

		if( m_nInfoListCount[type] >= max)
			_ResetClanMarkInfo(type);		//	새로 만들 공간이 없으면 모두 싹 정리

		//	등록되어 있지 않으면 새로 만든다.
		pInfo = _MakeClanMarkInfo(type, flag);

		//	여기 걸리면 알고리즘에 문제가 생긴겁니다. komet
		I3ASSERT( pInfo != nullptr);		
	}
		
	return pInfo;
}

ClanMarkInfo * CClanMarkManager::_FindClanMarkInfo(CLAN_MARK_SIZE type, UINT32 flag)
{
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);

	//	클랜 마크 리스트에서 원하는 클랜 마크를 찾아서 반환한다.
	for(size_t i=0; i<m_pInfoList[type].size(); i++)
	{
		ClanMarkInfo * pInfo = m_pInfoList[type][i];

		if( pInfo->_used && pInfo->_type == type && pInfo->_flag == flag)
			return pInfo;
	}

	return nullptr;
}

ClanMarkInfo * CClanMarkManager::_FindEmptyInfo(CLAN_MARK_SIZE type)
{
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);

	//	클랜 마크 리스트에서 원하는 클랜 마크를 찾아서 반환한다.
	for(size_t i=0; i<m_pInfoList[type].size(); i++)
	{
		ClanMarkInfo * pInfo = m_pInfoList[type][i];

		if( pInfo->_used == false)
			return pInfo;
	}

	return nullptr;
}

ClanMarkInfo * CClanMarkManager::_MakeClanMarkInfo(CLAN_MARK_SIZE type, UINT32 flag)
{	
	//	새로운 클랜 마크 이미지를 저장해야 할 새로운 페이지가 필요하면 512x512 텍스처를 생성합니다.
	//	하나의 새로운 텍스처에는 작은 마크 이미지(25x25 픽셀)일 경우 모두 20*20개를 담을수 있습니다.
	//	하나의 새로운 텍스처에는 중간 마크 이미지(52x52 픽셀)일 경우 모두 9*9개를 담을수 있습니다.
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);

	//	클랜 마크 리스트에 등록되어 있는지 검사	
	ClanMarkInfo * pNewInfo = _FindEmptyInfo(type);
	
	if( pNewInfo == nullptr)
	{
		return nullptr;
	}

	i3Texture* pSurfaceTex = pNewInfo->_image;

	INT32 u = pNewInfo->_texU;
	INT32 v = pNewInfo->_texV;

	//	텍스처에 새 마크 추가하기 (마크를 조합하여 최종 이미지를 pTex에 가지고 있는다.)
	MakeClanMarkTex(pSurfaceTex, type, flag, u, v);

	pNewInfo->_used = true;
	pNewInfo->_type = type;
	pNewInfo->_flag = flag;	
	pNewInfo->_image = pSurfaceTex;

	m_nInfoListCount[type]++;

	//	가장 마지막에 생성된 클랜 마크
	m_pLatestClanMarkInfo[type] = pNewInfo; 

	//////////////////////////////////////////////////////////////////////
#if 0
	//	조합된 클랜 마크 이미지를 파일로 덤프
	char temp[256] = "";
	i3ImageFile file; 
	sprintf_s(temp, "ClanMark%d.i3i", (INT32)type);
	file.SaveI3I(temp, pNewInfo->_image);
#endif

	return pNewInfo;
}

bool CClanMarkManager::MakeClanMarkShapeTex( i3Texture * pOutTex, CLAN_MARK_SIZE type, CLAN_MARK_LAYER_TYPE LayerType, 
											INT32 nImageIdx, INT32 nColorIdx, INT32 nDstTexU, INT32 nDstTexV)
{
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);
	I3ASSERT(nDstTexU >= 0);
	I3ASSERT(nDstTexV >= 0);
	I3_BOUNDCHK(nDstTexU, (INT32) pOutTex->GetWidth());
	I3_BOUNDCHK(nDstTexV, (INT32) pOutTex->GetHeight());

	I3ASSERT( pOutTex != nullptr);
	I3ASSERT( nImageIdx > 0);




	SIZE32	Size = {};
	CLAN_MARK_IMAGE	TexClanMarkImg;
	if ( ! GetMarkTexture(TexClanMarkImg, type, LayerType, nImageIdx, Size) )
		return false;


	COLOR userCol[2];
	GetUserColorTable(nColorIdx, &userCol[0]);		//	프레임 색상
	GetLayerColorTable(CLAN_MARK_LAYER_CONTENT, nColorIdx, &userCol[1]);	//	테두리 색상 (그래픽팀에서 지정된 색상만 가능)

	i3Texture * pTexClanMark[2] =
	{
		TexClanMarkImg.pFilledText,
		TexClanMarkImg.pOutline,
	};

	I3ASSERT( pTexClanMark[0] != nullptr);
	I3ASSERT( pTexClanMark[1] != nullptr);

	pOutTex->Lock(0);

	//	0: 배경 이미지/ 1: 배경 테두리 이미지/ 2: 모양 이미지/ 3: 모양 테두리 이미지	
	for(INT32 i=0; i<2; i++)
	{
		INT32 gapX = 1, gapY = 1;
		POINT32 cnt = CalcuTex(nImageIdx);

		//	source position
		INT32 offsetSrcX = (Size.w * cnt.x) + (gapX * cnt.x);
		INT32 offsetSrcY = (Size.h * cnt.y) + (gapY * cnt.y);

		//	destination position
		INT32 offsetDstX = nDstTexU;
		INT32 offsetDstY = nDstTexV;

		i3Texture * pSrcTex = pTexClanMark[i];
		pSrcTex->Lock(0);

		UINT32 * pSrcBuffer = ((UINT32 *) pSrcTex->GetLockedBuffer(0)) + (offsetSrcY * pSrcTex->GetWidth()) + offsetSrcX;
		UINT32 * pDstBuffer = ((UINT32 *) pOutTex->GetLockedBuffer(0)) + (offsetDstY * pOutTex->GetWidth()) + offsetDstX;

		for(INT32 y = 0 ; y < static_cast<INT32>(Size.h) ; y++)
		{
			if( y + nDstTexV < (INT32) pOutTex->GetHeight())
			{
				for(INT32 x = 0 ; x < static_cast<INT32>(Size.w) ; x++)
				{
					if( x + nDstTexU >= (INT32) pOutTex->GetWidth())
						continue;
					
					INT32 srcA = ((pSrcBuffer[x] >> 24) & 0xFF);
					INT32 srcR = ((pSrcBuffer[x] >> 16) & 0xFF);
					INT32 srcG = ((pSrcBuffer[x] >> 8) & 0xFF);
					INT32 srcB = ((pSrcBuffer[x] >> 0) & 0xFF);

					REAL32 weightSrcR = srcR / 255.0f;
					REAL32 weightSrcG = srcG / 255.0f;
					REAL32 weightSrcB = srcB / 255.0f;

					UINT8 r, g, b, a;
					if( i==0)	//	shape 픽셀
					{ 
						if(srcA > 0)
						{
							a = (UINT8)srcA;
							r = (UINT8)(((i3Color::GetR(&userCol[i]) / 255.0f) * weightSrcR) * 255.0f);
							g = (UINT8)(((i3Color::GetG(&userCol[i]) / 255.0f) * weightSrcG) * 255.0f);
							b = (UINT8)(((i3Color::GetB(&userCol[i]) / 255.0f) * weightSrcB) * 255.0f);			
						}
						else
						{
							a = (UINT8)((pSrcBuffer[x] >> 24) & 0xFF);
							r = (UINT8)((pSrcBuffer[x] >> 16) & 0xFF);
							g = (UINT8)((pSrcBuffer[x] >> 8) & 0xFF);
							b = (UINT8)((pSrcBuffer[x] >> 0) & 0xFF);
						}

						pDstBuffer[x] = ((a << 24) | (r << 16) | (g << 8) | (b << 0));	
					}
					else if ( i==1)	//	frame 픽셀 
					{
						INT32 dstA = ((pDstBuffer[x] >> 24) & 0xFF);   
						INT32 dstR = ((pDstBuffer[x] >> 16) & 0xFF);
						INT32 dstG = ((pDstBuffer[x] >> 8) & 0xFF);
						INT32 dstB = ((pDstBuffer[x] >> 0) & 0xFF);						
						
						REAL32 weight = srcA / 255.0f;

						r = (UINT8)((((dstR / 255.0f) * (1.0f - weight)) + (((i3Color::GetR(&userCol[i]) / 255.0f) * weight))) * 255.0f);
						g = (UINT8)((((dstG / 255.0f) * (1.0f - weight)) + (((i3Color::GetG(&userCol[i]) / 255.0f) * weight))) * 255.0f);
						b = (UINT8)((((dstB / 255.0f) * (1.0f - weight)) + (((i3Color::GetB(&userCol[i]) / 255.0f) * weight))) * 255.0f);
					
						//r = (UINT8)((dstR + (((i3Color::GetR(&userCol[i]) / 255.0f) * weightSrcR) * 255.0f)) * 0.5f);
						//g = (UINT8)((dstG + (((i3Color::GetG(&userCol[i]) / 255.0f) * weightSrcG) * 255.0f)) * 0.5f);
						//b = (UINT8)((dstB + (((i3Color::GetB(&userCol[i]) / 255.0f) * weightSrcB) * 255.0f)) * 0.5f);

						a = (UINT8)(srcA > dstA ? srcA : dstA);		

						pDstBuffer[x] = ((a << 24) | (r << 16) | (g << 8) | (b << 0));	
					}					
				}
			}

			pSrcBuffer += pSrcTex->GetWidth();
			pDstBuffer += pOutTex->GetWidth();
		}

		pSrcTex->Unlock(0);
	}
	
	pOutTex->Unlock(0);

	return true;
}

bool CClanMarkManager::MakeClanMarkTex(i3Texture * pOutTex, CLAN_MARK_SIZE type, 
									   INT32 nBackImageIdx, INT32 nShapeImageIdx, INT32 nBackColorIdx, INT32 nShapeColorIdx, 
									   INT32 nDstTexU, INT32 nDstTexV)
{
	//	인덱스를 가지고 클랜 마크 플래그로 변환
	UINT32 markFlag = MAKE_CLANMARK_FLAG(nBackImageIdx, nShapeImageIdx, nBackColorIdx, nShapeColorIdx);

	return MakeClanMarkTex(pOutTex, type, markFlag, nDstTexU, nDstTexV);
}

bool CClanMarkManager::MakeClanMarkTex(i3Texture * pOutTex, CLAN_MARK_SIZE type, UINT32 markFlag, INT32 nDstTexU, INT32 nDstTexV)
{
	I3_BOUNDCHK(type, MAX_CLAN_MARK_SIZE);
	I3ASSERT( pOutTex != nullptr);

	INT32 nBackImageIndex = (markFlag >> 24) & 0xFF;
	INT32 nShapeImageIndex = (markFlag >> 16) & 0xFF;
	INT32 nBackColorIndex = (markFlag >> 8) & 0xFF;
	INT32 nShapeColorIndex = (markFlag) & 0xFF;
	

	//	클랜마크 인덱스는 1 베이스입니다. (0값은 default 클랜 마크로 고정) komet
	/*I3ASSERT_ONCE( nBackImageIdx > 0	&& nBackImageIdx <= GetMarkBackImageCount());
	I3ASSERT_ONCE( nShapeImageIdx > 0	&& nShapeImageIdx <= GetMarkShapeImageCount());
	I3ASSERT_ONCE( nBackColorIdx > 0	&& nBackColorIdx <= GetMarkColorCount());
	I3ASSERT_ONCE( nShapeColorIdx > 0	&& nShapeColorIdx <= GetMarkColorCount());*/

	if(nBackImageIndex == 0)	return false;
	if(nShapeImageIndex == 0) return false;
	if(nBackColorIndex == 0) return false;
	if(nShapeColorIndex == 0) return false;


	SIZE32	Size = {};
	CLAN_MARK_IMAGE TexClanMarkBack, TexClanMarkShape;

	if ( ! GetMarkTexture(TexClanMarkBack, type, CLAN_MARK_LAYER_FRAME, nBackImageIndex, Size) )
	{
		//DUMP_MARK_FLAG(markFlag);
		return false;
	}

	if ( ! GetMarkTexture(TexClanMarkShape, type, CLAN_MARK_LAYER_CONTENT, nShapeImageIndex, Size) )
	{
		//DUMP_MARK_FLAG(markFlag);
		return false;
	}

	//	바탕 색상
	COLOR userCol[4];
	GetUserColorTable(nBackColorIndex, &userCol[0]);

	//	바탕 테두리 색상 (그래픽팀에서 지정된 색상만 가능)
	GetLayerColorTable(CLAN_MARK_LAYER_FRAME, nBackColorIndex, &userCol[1]);

	//	모양 색상
	GetUserColorTable(nShapeColorIndex, &userCol[2]);

	//	모양 테두리 색상 (그래픽팀에서 지정된 색상만 가능)
	GetLayerColorTable(CLAN_MARK_LAYER_CONTENT, nShapeColorIndex, &userCol[3]);


	i3Texture * pTexClanMark[4] =
	{
		TexClanMarkBack.pFilledText,
		TexClanMarkBack.pOutline,
		TexClanMarkShape.pFilledText,
		TexClanMarkShape.pOutline,
	};

	// 생성시에 체크를 하기 때문에 nullptr일 수 없지만
	// 태국 라이브에서 크래쉬가 났기 때문에 방어 코드 넣었음.
	for (INT32 i = 0 ; i < 4 ; ++i)
	{
		if ( pTexClanMark[i] == nullptr )
		{
			DUMP_MARK_FLAG(markFlag);
			return false;
		}
	}


	//		
	i3Texture* pDstClanTex = pOutTex;
	pDstClanTex->Lock(0);

	//	0: 배경 이미지/ 1: 배경 테두리 이미지/ 2: 모양 이미지/ 3: 모양 테두리 이미지	
	for(INT32 i=0; i<4; i++)
	{
		INT32 gapX = 1, gapY = 1;
		POINT32 cnt = {};

		if( i == 0 || i == 1)
		{
			cnt = CalcuTex(nBackImageIndex);
		}
		else if( i == 2 || i == 3)
		{
			cnt = CalcuTex(nShapeImageIndex);
		}

		//	source position
		INT32 offsetSrcX = (Size.w * cnt.x) + (gapX * cnt.x);
		INT32 offsetSrcY = (Size.h * cnt.y) + (gapY * cnt.y);

		//	destination position
		INT32 offsetDstX = nDstTexU;
		INT32 offsetDstY = nDstTexV;

		i3Texture * pSrcClanTex = pTexClanMark[i];
		pSrcClanTex->Lock(0);

		UINT32 * pSrcBuffer = ((UINT32 *) pSrcClanTex->GetLockedBuffer(0)) + (offsetSrcY * pSrcClanTex->GetWidth()) + offsetSrcX;
		UINT32 * pDstBuffer = ((UINT32 *) pDstClanTex->GetLockedBuffer(0)) + (offsetDstY * pDstClanTex->GetWidth()) + offsetDstX;

		for(INT32 y = 0 ; y < static_cast<INT32>(Size.h) ; y++)
		{
			if( y + nDstTexV < (INT32) pDstClanTex->GetHeight())
			{
				for(INT32 x = 0 ; x < static_cast<INT32>(Size.w) ; x++)
				{
					if( x + nDstTexU >= (INT32) pDstClanTex->GetWidth())	continue;

					INT32 srcA = ((pSrcBuffer[x] >> 24) & 0xFF);
					//INT32 srcR = ((pSrcBuffer[x] >> 16) & 0xFF);
					//INT32 srcG = ((pSrcBuffer[x] >> 8) & 0xFF);
					//INT32 srcB = ((pSrcBuffer[x] >> 0) & 0xFF);

					UINT8 r=0, g=0, b=0, a=0;
					if( i == 0)
					{	//	첫 바탕화면은 무조건 복사한다. (이전 위치의 픽셀은 무시된다.)
						r = i3Color::GetR(&userCol[i]); 
						g = i3Color::GetG(&userCol[i]);
						b = i3Color::GetB(&userCol[i]);
						a = (UINT8)srcA;

						pDstBuffer[x] = ((a << 24) | (r << 16) | (g << 8) | (b << 0));	
					}
					else
					{
						//	원본 이미지에 그려야할 픽셀이 있다.
						if(srcA > 0)
						{				
							INT32 dstA = ((pDstBuffer[x] >> 24) & 0xFF);
							INT32 dstR = ((pDstBuffer[x] >> 16) & 0xFF);
							INT32 dstG = ((pDstBuffer[x] >> 8) & 0xFF);
							INT32 dstB = ((pDstBuffer[x] >> 0) & 0xFF);						

							//	목표 이미지에 이미 픽셀이 있으므로 원본 이미지의 픽셀과 혼합한다.
							if(dstA > 0)
							{
								REAL32 weight = srcA / 255.0f;

								r = (UINT8)((((dstR / 255.0f) * (1.0f - weight)) + (((i3Color::GetR(&userCol[i]) / 255.0f) * weight))) * 255.0f);
								g = (UINT8)((((dstG / 255.0f) * (1.0f - weight)) + (((i3Color::GetG(&userCol[i]) / 255.0f) * weight))) * 255.0f);
								b = (UINT8)((((dstB / 255.0f) * (1.0f - weight)) + (((i3Color::GetB(&userCol[i]) / 255.0f) * weight))) * 255.0f);
								a = (UINT8)(srcA > dstA ? srcA : dstA);
							}
							//	목표 이미지에 픽셀이 없다.
							else
							{
								r = i3Color::GetR(&userCol[i]);
								g = i3Color::GetG(&userCol[i]);
								b = i3Color::GetB(&userCol[i]);
								a = (UINT8)srcA;
							}

							pDstBuffer[x] = ((a << 24) | (r << 16) | (g << 8) | (b << 0));	
						}
					}
				}
			}

			pSrcBuffer += pSrcClanTex->GetWidth();
			pDstBuffer += pDstClanTex->GetWidth();
		}

		pSrcClanTex->Unlock(0);
	}
	
	pDstClanTex->Unlock(0);

	return true;
}

void CClanMarkManager::OnRevive( i3RenderContext * pCtx)
{
	_RebuildClanMarkTexture();
}

void CClanMarkManager::_RebuildClanMarkTexture(void)
{
	for(INT32 type=0; type<MAX_CLAN_MARK_SIZE; type++)
	{
		for(size_t i=0; i<m_pInfoList[type].size(); i++)
		{
			ClanMarkInfo * pInfo = m_pInfoList[type][i];

			//	텍스처에 마크 그리기 (예전 값을 가지고 픽셀 복원)
			if( pInfo->_used)
				MakeClanMarkTex(pInfo->_image, pInfo->_type, pInfo->_flag, pInfo->_texU, pInfo->_texV);
		}

#if 0
		char temp[256] = "";
		i3ImageFile file; 
		sprintf_s(temp, "ClanMark_Restore%d.i3i", (INT32)type);
		file.SaveI3I(temp, m_pCombiClanMarkTex[type]);
#endif
	}
}


INT32	CClanMarkManager::GetMarkBackImageCount()
{
	return g_pConfigEx->GetGUI().CustomClanMarkFrameCount;
}


INT32	CClanMarkManager::GetMarkShapeImageCount()
{
	return g_pConfigEx->GetGUI().CustomClanMarkContentCount;
}


INT32	CClanMarkManager::GetMarkColorCount()
{
	return _CLAN_MARK_COLOR_MAX;
}

bool 		CClanMarkManager::GetMarkTexture(CLAN_MARK_IMAGE & MarkTex, CLAN_MARK_SIZE SizeType, CLAN_MARK_LAYER_TYPE LayerType, INT32 ImageIndex, SIZE32 & Size)
{
	switch (SizeType)
	{
	case CLAN_MARK_SMALL	:	Size.w = Size.h = CLAN_MARK_SMALL_PIXEL;	break;
	case CLAN_MARK_MEDIUM		:	Size.w = Size.h = CLAN_MARK_MID_PIXEL;		break;
	default					:	return false;
	}

	//INT32 TexIndex = CalcuImageIndex2TexIndex(ImageIndex);
//	if( TexIndex > MAX_CLAN_MARK_SIZE || TexIndex < 0)
//		return false;

	INT32 TexIndex = CalcClanMarkTexPageFromImgIndex(ImageIndex);
	if( TexIndex >= (INT32)m_ClanMarkTex[LayerType][SizeType].size() || TexIndex < 0)		// 경고좀 확인 좀...
		return false;

	CLAN_MARK_IMAGE * pTex = (CLAN_MARK_IMAGE *)m_ClanMarkTex[LayerType][SizeType][TexIndex];
	if ( ! pTex )
		return false;

	MarkTex = *pTex;
	return true;
}
