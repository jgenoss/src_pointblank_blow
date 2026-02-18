#include "i3OptPCH.h"
#include "i3OptMipmapGen.h"
#include "i3OptResizeTexture.h"
#include "i3Input/dxerr.h"
#include "i3Gfx/i3GfxUtilDX.h"

I3_CLASS_INSTANCE( i3OptMipmapGen);

i3OptMipmapGen::i3OptMipmapGen(void)
{
	m_Class = CLASS_TEXTURE;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

const char * i3OptMipmapGen::getFilterName( INT32 idx)
{
	static const char * s_pFilterName[] = 
	{
		"D3D - Point",
		"D3D - Linear",
		"D3D - Anisotropic",
		"D3D - Pyramidal Quad",
		"D3D - Gaussian Quad",
		"D3DX - Point",
		"D3DX - Linear",
		"D3DX - Triangle",
		"D3DX - Box"
	};

	I3ASSERT( (idx >= 0) && (idx < FILTER_COUNT));

	return s_pFilterName[ idx];
}

i3Texture *	i3OptMipmapGen::_GenD3DMipmap( i3Texture * pTex, UINT32 filter, INT32 maxLevel)
{
	return nullptr;
}

i3Texture *	i3OptMipmapGen::_GenD3DXMipmap( i3Texture * pTex, UINT32 filter, INT32 maxLevel)
{
	i3Texture * pNewTex;

	pNewTex = i3Texture::new_object_ref();

	// Dx를 이용하여 Mipmap을 생성하므로 반드시 i3TextureDX이어야한다.
	I3ASSERT(pNewTex->meta() == i3TextureDX::static_meta());

	bool needMakeRunTimeForm = false;
	I3G_IMAGE_FORMAT newTexFmt = pTex->GetFormat();

	UINT32 sizSurfaceCpy = i3Gfx::CalcRasterBufferSize( 
		pTex->GetWidth(), pTex->GetHeight(), 1, newTexFmt);

	pNewTex->Create( pTex->GetWidth(), pTex->GetHeight(), maxLevel, newTexFmt, 0);

	// pTex가 i3TextureMem일경우 i3TextureDX에서 사용하는 서피스포맷과 틀릴수있다.
	if(pTex->meta() == i3TextureMem::static_meta())
	{
		if(pTex->GetFormat() != pNewTex->GetFormat())
			needMakeRunTimeForm = true;
	}
	
	if(needMakeRunTimeForm)
	{
		// 0번레벨의 서피스를이용해서 밉맵을 만들기때문에 maxLevel이 1이상이어도 0번만 작업해준다.
		char* srcSur = pTex->Lock(0, 0); // pTex->Lock : i3SurfaceMem		
		INT8* newTexPersist = (INT8*)i3MemAlloc(sizSurfaceCpy);

		if(srcSur)
			memcpy(newTexPersist, srcSur, sizSurfaceCpy);

		pTex->Unlock(0);

		I3ASSERT(!pNewTex->GetPersistData(0));
		pNewTex->SetPersistData(0, newTexPersist);
		pNewTex->SetPersistFormat(newTexFmt);

		// 현재 DXNative에서피스 포맷에 맞게 변형되어 복사된다.
		pNewTex->getSurface(0)->MakeRuntimeForm(false); 

		// 내부에서 I3MEM_SAFE_FREE_POINTER호출로 i3MemFree호출 삭제
		pNewTex->SetPersistData(0, nullptr);
	}
	else
	{
		// Level 0의 복사
		{
			pNewTex->Lock( 0, 0);
			pTex->Lock( 0, 0);

			if(pNewTex->GetLockedBuffer(0) && pTex->GetLockedBuffer(0))
				memcpy( pNewTex->GetLockedBuffer(0), pTex->GetLockedBuffer(0), sizSurfaceCpy);

			pNewTex->Unlock( 0);
			pTex->Unlock( 0);
		}
	}

	// Mipmap의 생성
	{
		IDirect3DTexture9 * pDXTex = ((i3TextureDX *)pNewTex)->GetDXImage();

		// D3DX Filtering
		if( m_bDither)
			filter |= D3DX_FILTER_DITHER;

		if( m_bSRGBIn)
			filter |= D3DX_FILTER_SRGB_IN;

		if( m_bSRGBOut)
			filter |= D3DX_FILTER_SRGB_OUT;

		if( m_bMirrorU)
			filter |= D3DX_FILTER_MIRROR_U;

		if( m_bMirrorV)
			filter |= D3DX_FILTER_MIRROR_V;

		if( m_bMirrorW)
			filter |= D3DX_FILTER_MIRROR_W;

		CHECKRESULT( ::D3DXFilterTexture( pDXTex, NULL, 0, filter));

		CHECKRESULT( pDXTex->AddDirtyRect(NULL));
	}

	return pNewTex;
}

i3Texture * i3OptMipmapGen::GenMipmap( i3Texture * pTex)
{
	i3Texture * pNewTex;
	INT32 mipcount, w, h, mincx = 1, mincy = 1;

	w = pTex->GetWidth();
	h = pTex->GetHeight();
	mipcount = 1;

	if( m_bLimitSize)
	{
		mincx = m_MinCX;
		mincy = m_MinCY;
	}

	while(( w >= mincx) && ( h >= mincy))
	{
		mipcount++;
		w = w >> 1;
		h = h >> 1;
	}

	if( m_bLimitLevel)
	{
		mipcount = min( m_MaxLevel, mipcount);
	}

	DWORD filter;
	bool bD3DX = false;

	switch( m_Filter)
	{
		case FILTER_D3D_POINT :				filter = D3DTEXF_POINT;	break;
		case FILTER_D3D_LINEAR :			filter = D3DTEXF_LINEAR;	break;
		case FILTER_D3D_ANISOTROPIC :		filter = D3DTEXF_ANISOTROPIC;	break;
		case FILTER_D3D_PYRAMIDALQUAD :		filter = D3DTEXF_PYRAMIDALQUAD; break;
		case FILTER_D3D_GAUSSIANQUAD :		filter = D3DTEXF_GAUSSIANQUAD;	break;
		case FILTER_D3DX_POINT :			filter = D3DX_FILTER_POINT;		break;
		case FILTER_D3DX_LINEAR :			filter = D3DX_FILTER_LINEAR;	break;
		case FILTER_D3DX_TRIANGLE :			filter = D3DX_FILTER_TRIANGLE; 	break;
		default :							filter = D3DX_FILTER_BOX;		break;
	}

	if( (m_Filter >= FILTER_D3DX_POINT) && (m_Filter <= FILTER_D3DX_BOX))
		bD3DX = true;
	
	if( bD3DX)
	{
		pNewTex = _GenD3DXMipmap( pTex, filter, mipcount);
	}
	else
	{
		return nullptr;
		//pNewTex = _GenD3DMipmap( pTex, filter, mipcount);
	}

	if( pTex->hasName())
	{
		pNewTex->SetResourcePath( pTex->GetName());
	}

	pNewTex->setTextureFlag( pTex->getTextureFlag());

	return pNewTex;
}

void i3OptMipmapGen::_RemoveAllTexBindInfo(void)
{
	TEX_REF_INFO * pInfo;

	for(size_t i = 0; i < m_TexBindList.size(); i++)
	{
		pInfo = m_TexBindList[i];

		delete pInfo;
	}
	
	m_TexBindList.clear();
}

INT32 i3OptMipmapGen::_FindTexRef( i3Texture * pTex)
{
	TEX_REF_INFO * pInfo;

	for( size_t i = 0; i < m_TexBindList.size(); i++)
	{
		pInfo = m_TexBindList[i];

		if( pInfo->m_pTex == pTex)
			return i;
	}

	return -1;
}

void i3OptMipmapGen::Trace( i3Node * pRoot)
{
	i3TextureBindAttr * pTexBind;
	i3Texture * pTex;
	i3::vector<i3RenderAttr*> List;

	i3Scene::FindAttrByType( pRoot, i3TextureBindAttr::static_meta(), List);

	// Texture를 중복되지 않도록 확인을 한다.
	for(size_t i = 0; i < List.size(); i++)
	{
		pTexBind = (i3TextureBindAttr *) List[i];
		I3ASSERT( pTexBind != nullptr);

		//if( pTexBind->GetTexture()->GetLevelCount() > 1)	
		//	continue;		//	이미 Mipmap이 있다.

		if( _FindTexRef( pTexBind->GetTexture()) == -1)
		{
			TEX_REF_INFO * pInfo = new TEX_REF_INFO;

			pInfo->m_pTex = pTexBind->GetTexture();
			pInfo->m_pNewTex = GenMipmap( pTexBind->GetTexture());

			if( pInfo->m_pNewTex == nullptr)
			{
				delete pInfo;
				goto ExitPart;
			}

			m_TexBindList.push_back( pInfo);
		}
	}

	for(size_t i = 0; i < List.size(); i++)
	{
		pTexBind = (i3TextureBindAttr *) List[i];
		I3ASSERT( pTexBind != nullptr);

		pTex = pTexBind->GetTexture();

		if( pTex != nullptr)
		{
			INT32 idx;
			TEX_REF_INFO * pInfo;

			idx = _FindTexRef( pTex);
			I3ASSERT( idx != -1);

			pInfo = m_TexBindList[idx];
			pTexBind->SetTexture( pInfo->m_pNewTex);
		}
	}

ExitPart:

	_RemoveAllTexBindInfo();
}

void i3OptMipmapGen::SetProperty( char * pszFieldName, char * pszValue)
{
	if( i3::generic_is_iequal( pszFieldName, "MinX") )
	{
		m_MinCX = atoi( pszValue);
	}
	else if( i3::generic_is_iequal( pszFieldName, "MinY") ) 
	{
		m_MinCY = atoi( pszValue);
	}
	else if( i3::generic_is_iequal( pszFieldName, "Filter") )
	{
		if( i3::generic_is_iequal( pszValue, "D3DX_Point") )
			m_Filter = FILTER_D3DX_POINT;
		else if( i3::generic_is_iequal( pszValue, "D3DX_Linear") )
			m_Filter = FILTER_D3DX_LINEAR;
		else if( i3::generic_is_iequal( pszValue, "D3DX_Triangle") )
			m_Filter = FILTER_D3DX_TRIANGLE;
		else if( i3::generic_is_iequal( pszValue, "D3DX_Box") )
			m_Filter = FILTER_D3DX_BOX;
	}
	else if( i3::generic_is_iequal( pszFieldName, "Dither") )
	{
		m_bDither = atoi( pszValue) != 0;
	}
	else if( i3::generic_is_iequal( pszFieldName, "SRGBIn") )
	{
		m_bSRGBIn = atoi( pszValue) != 0;
	}
	else if( i3::generic_is_iequal( pszFieldName, "SRGBOut") )
	{
		m_bSRGBOut = atoi( pszValue) != 0;
	}
	else if( i3::generic_is_iequal( pszFieldName, "MirrorU") )
	{
		m_bMirrorU = atoi( pszValue) != 0;
	}
	else if( i3::generic_is_iequal( pszFieldName, "MirrorV") )
	{
		m_bMirrorV = atoi( pszValue) != 0;
	}
	else if( i3::generic_is_iequal( pszFieldName, "MirrorW" ))
	{
		m_bMirrorW = atoi( pszValue) != 0;
	}
	else
	{
		I3TRACE("(%s) SetProperty : Unknown field name (%s = %s)\n", this->static_meta()->class_name(), pszFieldName, pszValue);	
	}
}

const char * i3OptMipmapGen::getDesc(void)
{
	return "MIPMAP 생성";
}
