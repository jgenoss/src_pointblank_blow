#include "i3OptPCH.h"
#include "i3OptResizeTexture.h"

I3_CLASS_INSTANCE( i3OptResizeTexture);

i3OptResizeTexture::i3OptResizeTexture(void)
{
	m_Class = CLASS_TEXTURE;
}

void i3OptResizeTexture::_GetAvgPixel(	i3Texture * pTex, 
										INT32 x, INT32 y, INT32 cx, INT32 cy, COLORREAL * pCol)
{
	INT32 i, j;
	COLORREAL cTemp;

	if( x < 0)	
	{	
		cx -= -x;
		x = 0;
	}

	if( y < 0)	
	{
		cy -= -y;
		y = 0;
	}

	i3Color::Set( pCol, 0.0f, 0.0f, 0.0f, 0.0f);

	if((cx <= 0) || (cy <= 0))
		return;

	for( i = 0; i < cy; i++)
	{
		for( j = 0; j < cx; j++)
		{
			pTex->GetPixel( x + j, y + i, &cTemp);

			i3Vector::Add( (VEC4D *)pCol, (VEC4D *)pCol, (VEC4D *) &cTemp);
		}
	}

	REAL32 rt = 1.0f / (cx * cy);
	i3Vector::Scale( (VEC4D *)pCol, (VEC4D *)pCol, rt);
}

void i3OptResizeTexture::Scale( i3Texture * pSrc, i3Texture * pDest)
{
	REAL32 dx, dy, cx, cy;
	INT32 x, y, lv, srcW, srcH, dstW, dstH, srcLv;
	COLORREAL col;

	srcW = pSrc->GetWidth();
	srcH = pSrc->GetHeight();
	dstW = pDest->GetWidth();
	dstH = pDest->GetHeight();

	// Src > Dest
	for( lv = 0; lv < (INT32) pDest->GetLevelCount(); lv++)
	{
		pDest->Lock( lv);

		if( (INT32) pSrc->GetLevelCount() > lv)
		{
			srcLv = lv;
		}
		else
		{
			srcLv = 0;
			srcW = pSrc->GetWidth();
			srcH = pSrc->GetHeight();
		}

		pSrc->Lock( srcLv, I3G_LOCK_READONLY);

		cx = (REAL32)srcW / dstW;
		cy = (REAL32)srcH / dstH;
		dx = cx * 0.5f;
		dy = cy * 0.5f;

		for( y = 0; y < (INT32) dstH; y++)
		{
			for( x = 0; x < (INT32) dstW; x++)
			{
				// Dest의 각 Pixel을 기준으로 Src에서 평균값을 읽어와
				// 대입한다.
				_GetAvgPixel( pSrc, (INT32)((x * cx) - dx), (INT32)((y * cy) - dy), (INT32)cx, (INT32)cy, &col);

				pDest->SetPixel( x, y, &col);
			}
		}

		pDest->Unlock( pDest->getLockedLevel());
		pSrc->Unlock( pSrc->getLockedLevel());

		srcW = srcW >> 1;
		srcH = srcH >> 1;
		dstW = dstW >> 1;
		dstH = dstH >> 1;
	}
	
}

i3Texture *		i3OptResizeTexture::Resize( i3Texture * pSrc, INT32 targetW, INT32 targetH)
{
	i3Texture * pDest;
	I3G_IMAGE_FORMAT fmt;
	INT32 maxLv;

	fmt = pSrc->GetFormat();

	if( pSrc->GetFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED)
		fmt = I3G_IMAGE_FORMAT_RGBA_8888;

	pDest = i3Texture::new_object();

	// 가능한 최대 Mipmap Level을 구한다.
	{
		INT32 sz = MAX( targetW, targetH);

		for( maxLv = 0; sz > 16; maxLv++)
		{
			sz = sz >> 1;
		}

		maxLv = MIN( maxLv, (INT32) pSrc->GetLevelCount());
	}

	if( pDest->Create( targetW, targetH, maxLv, fmt, 0) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not create target texture(%dx%dx%d)", targetW, targetH, i3Gfx::GetImageFormatBitCount( fmt));
		I3_MUST_RELEASE(pDest);
		return nullptr;
	}

	Scale( pSrc, pDest);

	return pDest;
}

void i3OptResizeTexture::Trace( i3Node * pRoot)
{
	i3::vector<i3RenderAttr*> BindList;
	i3Texture * pSrc, * pDest;

	i3Scene::FindAttrByType( pRoot, i3TextureBindAttr::static_meta(), BindList);

	for(size_t i = 0; i < BindList.size(); i++)
	{
		INT32 tw, th;
		i3TextureBindAttr * pAttr = (i3TextureBindAttr *) BindList[i];

		if( pAttr->GetTexture() == nullptr)
			continue;

		pSrc = pAttr->GetTexture();

		pDest = nullptr;
		tw = pSrc->GetWidth();
		th = pSrc->GetHeight();

		switch( getType())
		{
			case TYPE_SCALE :		// 항상 Scale한다.
				tw = (INT32)(tw * m_fScale);
				th = (INT32)(th * m_fScale);
				break;

			case TYPE_LIMIT :		// 조건 검사
				if( m_UpperWidth != 0)
				{
					tw = MIN( tw, m_UpperWidth);
				}

				if( m_LowerWidth != 0)
				{
					tw = MAX( tw, m_LowerWidth);
				}

				if( m_UpperHeight != 0)
				{
					th = MIN( th, m_UpperHeight);
				}

				if( m_LowerHeight != 0)
				{
					th = MAX( th, m_LowerHeight);
				}
				break;
		}

		if( (tw != (INT32) pSrc->GetWidth()) || (th != (INT32) pSrc->GetHeight()))
		{
			pDest = Resize( pSrc, tw, th);

			if( pDest != nullptr)
			{
				pAttr->SetTexture( pDest);
				I3_MUST_RELEASE(pDest);
			}
		}
	}
}

void i3OptResizeTexture::SetProperty( char * pszFieldName, char * pszValue)	//	add by KOMET
{
	if( i3::generic_is_iequal( pszFieldName, "Type") )
	{
		if( i3::generic_is_iequal( pszValue, "Scale") )
			setType( TYPE_SCALE);
		else
			setType( TYPE_LIMIT);
	}
	else if( i3::generic_is_iequal( pszFieldName, "UpperWidth") )
	{
		m_UpperWidth = atoi( pszValue);
	}
	else if( i3::generic_is_iequal( pszFieldName, "LowerWidth") )
	{
		m_LowerWidth = atoi( pszValue);
	}
	else if( i3::generic_is_iequal( pszFieldName, "UpperHeight") )
	{
		m_UpperHeight = atoi( pszValue);
	}
	else if( i3::generic_is_iequal( pszFieldName, "LowerHeight") )
	{
		m_LowerHeight = atoi( pszValue);
	}
	else if( i3::generic_is_iequal( pszFieldName, "ScaleFactor") )
	{
		m_fScale = (REAL32) atof( pszValue);
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "(%s) SetProperty : Unknown field name (%s = %s)", meta()->class_name(), pszFieldName, pszValue);	
	}
}
