#include "i3OptPCH.h"
#include "i3OptConvertDXTTexture.h"

#include "nvDXTLib/inc/dds/ddsTypes.h"
#include "nvDXTLib/inc/dds/ConvertColor.h"
#include "nvDXTLib/inc/dds/nvdxt_options.h"
#include "nvDXTLib/inc/dds/tPixel.h"
#include "nvDXTLib/inc/dds/tVector.h"
#include "nvDXTLib/inc/dxtlib/dxtlib.h"

#include "i3Base/itl/vector_util.h"
#include "i3Base/string/compare/generic_is_niequal.h"

namespace
{
	struct __DXT_CONVERT_INFO
	{
		UINT32 m_Accm = 0;
		i3Texture *		m_pTex = nullptr;
	};

	struct TEX_REF_INFO
	{
		i3Texture *		m_pTex = nullptr;
		i3Texture *		m_pNewTex = nullptr;
	};
}

I3_CLASS_INSTANCE(i3OptConvertDXTTexture);

i3OptConvertDXTTexture::i3OptConvertDXTTexture(void)
{
	m_Class = CLASS_TEXTURE;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;

	setQuality( QUALITY_HIGHEST);
	setBinaryAlphaEnable( false);
	setAlphaRef( 0x7F);
	setDitherEnable( true);
	setTargetImageFormat( I3G_IMAGE_FORMAT_DXT1);
}

I3G_IMAGE_FORMAT i3OptConvertDXTTexture::_findProperFormat( i3TextureBindAttr * pTexAttr)
{
	if( i3::same_of<i3SpecularMapBindAttr* >(pTexAttr))
	{
		// Normal Map 등의 경우, Pixel의 값이 최대한 손상되지 않도록
		// DXT5로 강제한다.
		return I3G_IMAGE_FORMAT_DXT5;
	}

	if( i3::same_of<i3NormalMapBindAttr*>(pTexAttr))
	{
		// Normal Map 등의 경우, Pixel의 값이 최대한 손상되지 않도록
		// DXT5로 강제한다.
		return I3G_IMAGE_FORMAT_DXT1;
	}

	if( i3::same_of<i3EmissiveMapBindAttr*>(pTexAttr))
	{
		return I3G_IMAGE_FORMAT_DXT1;
	}

	i3Texture * pTex = pTexAttr->GetTexture();

	if( !(pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_ALPHA))
	{
		// Alpha가 없다면 DXT1으로 충분하다.
		return I3G_IMAGE_FORMAT_DXT1;
	}

	// Alpha가 있다면, 그 값이 On/Off 수준인지, 또는 Blending을 해야하는 수준인지에
	// 따라 판단.
	UINT32 x, y;
	COLORREAL col;

	i3Surface * pSurface = pTex->getSurface( 0);

	pTex->Lock( 0);

	for( y = 0; y < pSurface->getHeight(); y++)
	{
		for( x = 0; x < pSurface->getWidth(); x++)
		{
			pTex->GetPixel( x, y, &col);

			if((col.a > I3_EPS) && (col.a < (1.0f - I3_EPS)))
			{
				pTex->Unlock( 0);
				return I3G_IMAGE_FORMAT_DXT5;
			}
		}
	}

	pTex->Unlock( 0);

	return I3G_IMAGE_FORMAT_DXT1;
}

i3Texture * i3OptConvertDXTTexture::MakeToDXT( i3Texture * pSrc)
{
	UINT32 lv, w, h;
	i3Texture * pDest;

	if( (pSrc->GetFormat() & ~I3G_IMAGE_FORMAT_MASK_PLATFORM) == (m_TargetFormat & ~I3G_IMAGE_FORMAT_MASK_PLATFORM))
	{
		return pSrc;
	}

	pDest = i3Texture::new_object_ref();

	w = pSrc->GetWidth();
	h = pSrc->GetHeight();
	pDest->Create( w, h, pSrc->GetLevelCount(), m_TargetFormat, 0);

	pDest->setTextureFlag( pSrc->getTextureFlag());

	I3TRACE( "DXT Converting (%s) : %s -> %s\n", pSrc->GetName(), i3Gfx::GetImageFormatName( pSrc->GetFormat()), i3Gfx::GetImageFormatName( m_TargetFormat));

	for( lv = 0; lv < pSrc->GetLevelCount(); lv++)
	{
		i3Surface * pSrcSurface = pSrc->getSurface( lv);
		i3Surface * pDestSurface = pDest->getSurface( lv);

		::D3DXLoadSurfaceFromSurface( pDestSurface->getSurface(), NULL, NULL,
									pSrcSurface->getSurface(), NULL, NULL,
									D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER,
									0);
		w = w >> 1;
		h = h >> 1;
	}

	I3ASSERT( i3::same_of<i3TextureDX* >(pDest));
	
	i3TextureDX * pTexDX = (i3TextureDX *) pDest;

	pTexDX->GetDXImage()->AddDirtyRect(nullptr);


	return pDest;
}

i3Texture * i3OptConvertDXTTexture::MakeFromDXT( i3Texture * pSrc)
{
	return MakeToDXT( pSrc);
}

inline void _RemoveAllTexBindInfo( i3::vector<TEX_REF_INFO*>& List)
{
	TEX_REF_INFO * pInfo;

	for(size_t i = 0; i < List.size(); i++)
	{
		pInfo = List[i];
		delete pInfo;
	}
	
	List.clear();
}

inline INT32 _FindTexRef( i3Texture * pTex, i3::vector<TEX_REF_INFO*>& List)
{
	TEX_REF_INFO * pInfo;

	for(size_t i = 0; i < List.size(); i++)
	{
		pInfo = List[i];

		if( pInfo->m_pTex == pTex)
			return i;
	}

	return -1;
}


void i3OptConvertDXTTexture::Trace( i3Node * pRoot)
{
	i3TextureBindAttr * pTexBind;
	i3Texture * pTex;
	i3::vector<i3RenderAttr*> List;
	i3::vector<TEX_REF_INFO*> TexBindList;
	bool bCompress;

	bCompress = (getTargetImageFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED) != 0;		

	i3Scene::FindAttrByType( pRoot, i3TextureBindAttr::static_meta(), List);
	i3::vu::remove_duplicates(List);

	ReportStart( INT32(List.size()) );

	// Texture를 중복되지 않도록 확인을 한다.
	for(size_t i = 0; i < List.size(); i++)
	{
		pTexBind = (i3TextureBindAttr *) List[i];

		{
			i3Texture * pTex = pTexBind->GetTexture();

			if( pTex != nullptr)
				ReportProgress( i, pTex->GetName());
			else
				ReportProgress( i);
		}

		if( _FindTexRef( pTexBind->GetTexture(), TexBindList) == -1)
		{
			TEX_REF_INFO * pInfo = new TEX_REF_INFO;
			i3mem::FillZero( pInfo, sizeof( TEX_REF_INFO));

			pInfo->m_pTex = pTexBind->GetTexture();

			if( pTexBind->GetTexture() != nullptr)
			{
				if( bCompress)
				{
					m_TargetFormat = _findProperFormat( pTexBind);

					pInfo->m_pNewTex = MakeToDXT( pTexBind->GetTexture());
				}
				else
				{
					pInfo->m_pNewTex = pTexBind->GetTexture();
				}

				if( pInfo->m_pTex->hasName())
				{
					pInfo->m_pNewTex->SetName( pInfo->m_pTex->GetName());
				}
			}

			TexBindList.push_back( pInfo);
		}
	}

	for(size_t i = 0; i < List.size(); i++)
	{
		pTexBind = (i3TextureBindAttr *) List[i];

		pTex = pTexBind->GetTexture();

		if( pTex != nullptr)
		{
			INT32 idx;
			TEX_REF_INFO * pInfo;

			idx = _FindTexRef( pTex, TexBindList);
			I3ASSERT( idx != -1);

			pInfo = TexBindList[idx];

			pTexBind->SetTexture( pInfo->m_pNewTex);

		}
	}
	
	ReportEnd();
}

void i3OptConvertDXTTexture::SetProperty( char * pszFieldName, char * pszValue)
{	
	if( i3::generic_is_iequal( pszFieldName, "QUALITY") )
	{		
		if( i3::generic_is_iequal( pszValue, "FASTEST") )
			setQuality( QUALITY_FASTEST);
		else if( i3::generic_is_iequal( pszValue, "NORMAL") )
			setQuality( QUALITY_NORMAL);
		else if( i3::generic_is_iequal( pszValue, "HIGHEST") )
			setQuality( QUALITY_HIGHEST);
		else 
			goto LABEL_INVALID;
		
	}
	else if( i3::generic_is_iequal( pszFieldName, "BinaryAlpha") ) 
	{
		if( i3::generic_is_niequal(pszValue, "T", 1) ||
			 i3::generic_is_niequal(pszValue, "Y", 1) )
			setBinaryAlphaEnable( true);
		else if( i3::generic_is_niequal(pszValue, "F", 1) ||
			 i3::generic_is_niequal(pszValue, "N", 1) )
			setBinaryAlphaEnable( false);	
		else 
			goto LABEL_INVALID;
	}
	else if( i3::generic_is_iequal( pszFieldName, "AlphaRef") ) 
	{
		INT32 nAlphaRef = atoi( pszValue);

		if( nAlphaRef < 0 || nAlphaRef > 255)
			goto LABEL_INVALID;

		setAlphaRef( (UINT8) nAlphaRef);
	}
	else if( i3::generic_is_iequal( pszFieldName, "Dither") ) 
	{
		if( i3::generic_is_niequal(pszValue, "T", 1) ||
			 i3::generic_is_niequal(pszValue, "Y", 1) )
			setDitherEnable( true);
		else if( i3::generic_is_niequal(pszValue, "F", 1) ||
			 i3::generic_is_niequal(pszValue, "N", 1) )
			setDitherEnable( false);	
		else 
			goto LABEL_INVALID;
	}
	else if( i3::generic_is_iequal( pszFieldName, "TargetFormat") ) 
	{
		if( i3::generic_is_iequal( pszValue, "DXT1_X") )
			setTargetImageFormat( I3G_IMAGE_FORMAT_DXT1_X);
		else if( i3::generic_is_iequal( pszValue, "DXT1") )
			setTargetImageFormat( I3G_IMAGE_FORMAT_DXT1);
		else if(i3::generic_is_iequal( pszValue, "DXT2") )
			setTargetImageFormat( I3G_IMAGE_FORMAT_DXT2);
		else if( i3::generic_is_iequal( pszValue, "DXT3") )
			setTargetImageFormat( I3G_IMAGE_FORMAT_DXT3);
		else if( i3::generic_is_iequal( pszValue, "DXT4") )
			setTargetImageFormat( I3G_IMAGE_FORMAT_DXT4);
		else if( i3::generic_is_iequal( pszValue, "DXT5") )
			setTargetImageFormat( I3G_IMAGE_FORMAT_DXT5);
		else 
			goto LABEL_INVALID;
	}

	return;

	//	invalid field or value
LABEL_INVALID:

	I3TRACE("(%s) SetProperty : Unknown field name (%s = %s)\n", this->static_meta()->class_name(), pszFieldName, pszValue);	
}

const char *	i3OptConvertDXTTexture::getDesc(void)
{
	return "DXT Texture 변환";
}

bool i3OptConvertDXTTexture::isSupportProgress(void)
{
	return true;
}