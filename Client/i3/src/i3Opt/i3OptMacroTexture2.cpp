#include "i3OptPCH.h"
#include "i3OptMacroTexture2.h"
#include "i3Base/itl/vector_util.h"

//I3_CLASS_INSTANCE( i3OptMacroTexture2, i3SceneOptimizer);
I3_CLASS_INSTANCE( i3OptMacroTexture2);

#define		MAXIMUM_TEX_SIZE		4096

i3OptMacroTexture2::i3OptMacroTexture2(void) : m_TargetCX(MAXIMUM_TEX_SIZE), m_TargetCY(MAXIMUM_TEX_SIZE)
{
	m_Class = CLASS_TEXTURE;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

GEO_TEXBIND_INFO  *	i3OptMacroTexture2::_addGeoBind( i3Texture * pTex, i3Geometry * pGeo, INT32 texIdx, const i3::vector<i3Texture*>& RelList)
{
	INT32 i;
	GEO_TEXBIND_INFO * pNew, * pTemp;

	for( i = 0; i < _getGeoBindCount(); i++)
	{
		pTemp = _getGeoBind( i);

		if(	pTemp->getTexture() == pTex)
		{
			// Geometry 추가
			pTemp->addGeo( pGeo);
			// Relative Texture 추가.
			pTemp->addRelTex( RelList);
		}
	}

	// 동일한 Diffuse Map에 대한 것이 없다.	
	pNew = new GEO_TEXBIND_INFO;

	pNew->setTexCoordIndex( texIdx);
	pNew->setTexture( pTex);
	pNew->addGeo( pGeo);

	pNew->addRelTex( RelList);

	m_GeoList.push_back( pNew);

	return pNew;
}

GEO_TEXBIND_INFO  *	i3OptMacroTexture2::_addGeoBind( i3Texture * pTex, i3Geometry * pGeo, INT32 texIdx)
{
	INT32 i;
	GEO_TEXBIND_INFO * pNew, * pTemp;

	for( i = 0; i < _getGeoBindCount(); i++)
	{
		pTemp = _getGeoBind( i);

		if(	pTemp->getTexture() == pTex)
		{
			// Geometry 추가
			pTemp->addGeo( pGeo);
		}
	}

	// 동일한 Diffuse Map에 대한 것이 없다.	
	pNew = new GEO_TEXBIND_INFO;

	pNew->setTexCoordIndex( texIdx);
	pNew->setTexture( pTex);
	pNew->addGeo( pGeo);

	m_GeoList.push_back( pNew);

	return pNew;
}



void	i3OptMacroTexture2::_removeAllGeoBind(void)
{
	INT32 i;
	GEO_TEXBIND_INFO * pInfo;

	for( i = 0; i < _getGeoBindCount(); i++)
	{
		pInfo = (GEO_TEXBIND_INFO *) _getGeoBind( i);

		delete pInfo;
	}

	m_GeoList.clear();
}

// 해당 Node의 Parent를 따라가며, i3TextureBindAttr를 찾는다.
i3TextureBindAttr *	i3OptMacroTexture2::_findTexBind( i3Node * pNode, i3ClassMeta * pMeta)
{
	i3Node * pRoot = getTraceRoot()->GetParent();		// pRoot는 nullptr일 수 있다.
	INT32 i;
	i3RenderAttr * pAttr;

	while( (pNode != nullptr) && (pNode != pRoot))
	{
		if( i3::kind_of<i3AttrSet* >(pNode))
		{
			i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

			for( i = 0; i < pAttrSet->GetAttrCount(); i++)
			{
				pAttr = pAttrSet->GetAttr( i);

				if( pAttr->same_of( pMeta))
				{
					return (i3TextureBindAttr *) pAttr;
				}
			}
		}

		pNode = pNode->GetParent();
	}

	return nullptr;
}

bool	i3OptMacroTexture2::_isTilling( i3GeometryAttr * pGeoAttr, INT32 texCoord, INT32 texCX, INT32 texCY)
{
	// 해당 Geometry의 Vertex Data를 검색해,
	// Tilling이 사용되고 있는지를 확인.
	i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	INT32 i;
	VEC2D uv;
	bool bResult = false;
	REAL32 errX, errY;

	// 만약 Diffuse Texture Coordinates를 가지고 있지 않은 Vertex라면
	// Tiling이 되어 있는 Geometry라고 임시로 반환해
	// 처리가 되지 않도록 한다.
	if( pVA->GetFormat()->GetTextureCoordSetCount() <= 0)
		return true;

	pVA->Lock();

	errX = 5.0f / texCX;
	errY = 5.0f / texCY;

	for( i = 0; i < (INT32) pVA->GetCount(); i++)
	{
		pVA->GetTextureCoord( texCoord, i, &uv);

		if( (i3Math::abs( uv.x) > (1.0f + errX)) || (i3Math::abs( uv.y) > (1.0f + errY)))
		{
			bResult = true;
			break;
		}
	}

	pVA->Unlock();
	
	return bResult;
}

bool i3OptMacroTexture2::_checkTiledGeometry( GEO_TEXBIND_INFO * pInfo)
{
	INT32 j, k;
	i3Geometry * pGeo;
	i3RenderAttr * pAttr;
	INT32 cx, cy;
	i3Texture * pTex = pInfo->getTexture();

	cx = pTex->GetWidth();
	cy = pTex->GetHeight();

	for( j = 0; j < pInfo->getGeoCount(); j++)
	{
		pGeo = (i3Geometry *) pInfo->getGeo( j);

		for( k = 0; k < pGeo->GetGeometryAttrCount(); k++)
		{
			pAttr = pGeo->GetGeometryAttr( k);

			if( i3::kind_of<i3GeometryAttr*>(pAttr))
			{
				if( pTex->getTextureFlag() & I3I_TEXFLAG_LUX)
				{
					// LuxMap에 대한 처리이므로, TextureCoord Ch 1을 확인한다.
					if( _isTilling( (i3GeometryAttr *) pAttr, 1, cx, cy))
					{
						// LuxMap에 대해서 Tilling되어 있다.
						return true;
					}
				}
				else
				{
					// DiffuseMap애 대한 처리. TextureCoord Ch 0을 확인한다.
					if( _isTilling( (i3GeometryAttr *) pAttr, 0, cx, cy))
					{
						// Diffuse, Specular, Normal, ReflectMask Map들이 Tiling 되어 있기 때문에
						// 더 이상처리하지 않도록 한다.
						return true;
					}
				}
			}
		}
	}

	return false;
}

void i3OptMacroTexture2::_FilterTiledTextures(void)
{
	INT32 i;
	GEO_TEXBIND_INFO * pInfo;

	for( i = 0; i < _getGeoBindCount(); i++)
	{
		pInfo = _getGeoBind( i);

		if( _checkTiledGeometry( pInfo))
		{
			m_GeoList.erase(m_GeoList.begin() + i);

			delete pInfo;

			i--;
		}
	}
}


bool	i3OptMacroTexture2::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( i3::kind_of<i3Geometry* >(pNode))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;
		i3Texture * pTex, * pTargetTex;
		i3TextureBindAttr * pTexBind;
		i3::vector<i3Texture*>	relList;

		// Diffuse Map에 대한 처리.
		switch( m_Phase)
		{
			case 0 :		// Diffuse
				{
					pTexBind = _findTexBind( pNode, i3TextureBindAttr::static_meta());
					if( pTexBind != nullptr)
					{
						pTargetTex = pTexBind->GetTexture();

						if( pTargetTex != nullptr)
						{
							// Diffuse와 동일한 TexCoord를 공유하는 NormalMap, SpecularMap, ReflectMaskMap, EmissiveMap에 대해
							// 검색하여 Relative Texture로 등록한다.
							pTexBind = _findTexBind( pNode, i3NormalMapBindAttr::static_meta());
							if( pTexBind != nullptr)
							{
								pTex = pTexBind->GetTexture();
								if( pTex != nullptr)
									relList.push_back( pTex);
							}

							pTexBind = _findTexBind( pNode, i3SpecularMapBindAttr::static_meta());
							if( pTexBind != nullptr)
							{
								pTex = pTexBind->GetTexture();
								if( pTex != nullptr)
									relList.push_back( pTex);
							}

							pTexBind = _findTexBind( pNode, i3ReflectMaskMapBindAttr::static_meta());
							if( pTexBind != nullptr)
							{
								pTex = pTexBind->GetTexture();
								if( pTex != nullptr)
									relList.push_back( pTex);
							}

							pTexBind = _findTexBind( pNode, i3EmissiveMapBindAttr::static_meta());
							if( pTexBind != nullptr)
							{
								pTex = pTexBind->GetTexture();
								if( pTex != nullptr)
									relList.push_back( pTex);
							}

							_addGeoBind( pTargetTex, pGeo, 0, relList);
						}
					}

					relList.clear();
				}
				break;

			case 1 :
				// LuxMap에 대한 처리.
				{
					pTexBind = _findTexBind( pNode, i3LuxMapBindAttr::static_meta());
					if( pTexBind != nullptr)
					{
						pTargetTex = pTexBind->GetTexture();
						if( pTargetTex != nullptr)
						{
							_addGeoBind( pTargetTex, pGeo, 1);
						}
					}
				}
				break;
		}
	}

	return true;
}

bool i3OptMacroTexture2::_canBePlace( i3::pack::RECT * pRect)
{

	TEX_PLACE_INFO * pTemp;
	i3::pack::RECT rt;

	// 오른쪽으로 영역이 벗어났는지 확인
	if( pRect->right > m_TargetCX)
		return false;

	// 아래쪽으로 영역이 벗어났는지 확인
	if( pRect->bottom > m_TargetCY)
		return false;

	for(size_t i = 0; i < m_PlaceTexList.size(); i++)
	{
		pTemp = m_PlaceTexList[i];

		if( pTemp->m_idxMacro == m_idxMacro)
		{
			// 동일 Macro Texture에 배치된 Texture들 중에 영역이 겹치는 경우가 있다면 Out!
			rt.left		= (REAL32) pTemp->m_X;
			rt.top		= (REAL32) pTemp->m_Y;
			rt.right	= (REAL32) (pTemp->m_X + pTemp->m_Width - 1);
			rt.bottom	= (REAL32) (pTemp->m_Y + pTemp->m_Height - 1);

			if( i3Math::RectInRect( &rt, pRect) != TOUCH_NONE)
				return false;
		}
	}

	return true;
}

bool i3OptMacroTexture2::_Rec_TryToPlace( RECT * pRect)
{
	TEX_PLACE_INFO * pInfo, * pFindInfo = nullptr;
	INT32 srcSpan, destSpan;
	INT32 srcCX, srcCY;
	INT32 minDiff = 0x7FFFFFFF;
	RECT rt;

	if( pRect == nullptr)
	{
		rt.left = 0;	rt.top = 0;
		rt.right = m_TargetCX;
		rt.bottom = m_TargetCY;

		pRect = &rt;
	}

	srcCX = pRect->right - pRect->left;
	srcCY = pRect->bottom - pRect->top;
	srcSpan = srcCX * srcCY;

	// 배치되지 않은 Merge Tex들 중에 해당 Rect에 가장 적합하게 들어갈 수 있는
	// 놈을 찾는다. 면적의 차이를 비교
	for( size_t i = 0; i < m_PlaceTexList.size(); i++)
	{
		pInfo = m_PlaceTexList[i];

		if( pInfo->m_idxMacro != -1)
			continue;						// 이미 배치가 끝난 놈은 제외

		// 가로든 세로든 지정된 범위를 벗어난다면 제외
		if((pInfo->m_Width > srcCX) || (pInfo->m_Height > srcCY))
			continue;

		destSpan = pInfo->m_Width * pInfo->m_Height;

		INT32 diff = abs( srcSpan - destSpan);

		if( minDiff > diff)
		{
			pFindInfo = pInfo;
			minDiff = diff;
		}
	}

	if( pFindInfo == nullptr)
	{
		// 더 이상 배치 가능한 것이 없다면 배치를 종료한다.
		// 그러나, 이 상황이 항상 문제인 것은 아니며
		// 실제로 배치가 정상적으로 종료된 경우에도 발생할 수 있다.
		return true;
	}

	// 배치!!!!
	{
		pFindInfo->m_idxMacro = m_idxMacro;
		pFindInfo->m_X = pRect->left;
		pFindInfo->m_Y = pRect->top;
	}

	// 남는 Right, Bottom 영역에 대해 배치 시도
	{
		RECT remnant;
		i3::pack::RECT temp;

		// Right영역
		remnant.left	= pFindInfo->m_X + pFindInfo->m_Width;
		remnant.top		= pFindInfo->m_Y;
		remnant.right	= pRect->right;
		remnant.bottom	= pFindInfo->m_Y + pFindInfo->m_Height;

		temp.left		= (REAL32) remnant.left;
		temp.top		= (REAL32) remnant.top;
		temp.right		= (REAL32) remnant.right;
		temp.bottom		= (REAL32) remnant.bottom;

		if( _canBePlace( &temp))
		{
			_Rec_TryToPlace( &remnant);
		}

		// bottom 영역
		//
		// 우선 Right영역과 중복이 될 수 있는 4-4분면도 포함시킨 공간을 시도한다.
		remnant.left	= pFindInfo->m_X;
		remnant.top		= pFindInfo->m_Y + pFindInfo->m_Height;
		remnant.right	= pRect->right;
		remnant.bottom	= pRect->bottom;

		temp.left		= (REAL32) remnant.left;
		temp.top		= (REAL32) remnant.top;
		temp.right		= (REAL32) remnant.right;
		temp.bottom		= (REAL32) remnant.bottom;

		if( _canBePlace( &temp))
		{
			_Rec_TryToPlace( &remnant);
		}
	}

	return true;
}

void i3OptMacroTexture2::_RemoveBigTextures(void)
{
	INT32 i, j;
	GEO_TEXBIND_INFO * pInfo;
	i3Texture * pTex;
	bool bRemove;

	for( i = 0; i < _getGeoBindCount(); i++)
	{
		pInfo = _getGeoBind( i);

		bRemove = false;

		pTex = pInfo->getTexture();

		// 지정 해상도보다 큰 Texture들은 애초에 배치가 안되기 때문에 대상에서 제외
		if( ((INT32) pTex->GetWidth() > m_TargetCX) || ((INT32) pTex->GetHeight() > m_TargetCY))
			bRemove = true;

		for( j = 0; j < pInfo->getRelTexCount(); j++)
		{
			pTex = pInfo->getRelTex( j);

			if( ((INT32)pTex->GetWidth() > m_TargetCX) || ((INT32) pTex->GetHeight() > m_TargetCY))
				bRemove = true;

		}

		if( bRemove)
		{
			m_GeoList.erase(m_GeoList.begin() + i);

			delete pInfo;
			i--;
			continue;
		}
	}
}

TEX_PLACE_INFO *	i3OptMacroTexture2::_addTexPlace( i3Texture * pTex)
{
	TEX_PLACE_INFO * pInfo;

	pInfo = (TEX_PLACE_INFO *) new TEX_PLACE_INFO;
	
	pInfo->m_idxMacro = -1;
	pInfo->m_pTex = pTex;
	pInfo->m_X = 0;
	pInfo->m_Y = 0;
	pInfo->m_Width = pTex->GetWidth();
	pInfo->m_Height = pTex->GetHeight();

	m_PlaceTexList.push_back( pInfo);

	return pInfo;
}

void i3OptMacroTexture2::_removeAllTexPlace(void)
{
	TEX_PLACE_INFO * pInfo;

	for(size_t i = 0; i < m_PlaceTexList.size(); i++)
	{
		pInfo = m_PlaceTexList[i];

		delete pInfo;
	}

	m_PlaceTexList.clear();
}

void i3OptMacroTexture2::_BuildPlacementList(void)
{
	INT32 i, j;
	GEO_TEXBIND_INFO * pInfo;
	i3Texture * pBiggest = nullptr, * pTex;
	INT32 maxSz = 0;
	UINT32 texFlag;

	// 가장 큰 유형의 Texture 위주로 배치를 해야하기 때문에
	// 가장 큰 Texture를 찾는다.
	for( i = 0; i < _getGeoBindCount(); i++)
	{
		pInfo = _getGeoBind( i);

		pTex = pInfo->getTexture();
		if(((INT32) pTex->GetWidth() > maxSz) || ((INT32) pTex->GetHeight() > maxSz))
		{
			pBiggest = pTex;
			maxSz = max( pTex->GetWidth(), pTex->GetHeight());
		}

		for( j = 0; j < pInfo->getRelTexCount(); j++)
		{
			pTex = pInfo->getTexture();
			if(((INT32) pTex->GetWidth() > maxSz) || ((INT32) pTex->GetHeight() > maxSz))
			{
				pBiggest = pTex;
				maxSz = max( pTex->GetWidth(), pTex->GetHeight());
			}
		}
	}

	// 처리할 것이 없다는 뜻..
	if( pBiggest == nullptr)
		return;

	// 가장 큰 Texture의 유형(Diffuse, Normal, ...)
	texFlag = pBiggest->getTextureFlag();

	// 해당 유형의 Texture에 대해서만 Placement List 정보를 만들고 배치한다.
	// Placement List는 실제 배치 위치에 대한 기준을 제시하는 것으로,
	// Texture Coordinates가 공유되는 Texture들을 재배치할 때, 이 정보를
	// 바탕으로 처리한다.
	{
		TEX_PLACE_INFO * pPlace;

		for( i = 0; i < _getGeoBindCount(); i++)
		{
			pInfo = _getGeoBind( i);

			pTex = pInfo->getTexture();
			if( (pTex->getTextureFlag() & I3I_TEXFLAG_MASK_USAGE) == texFlag)
			{
				pPlace = _addTexPlace( pTex);
				pPlace->m_pTargetTex = pInfo->getTexture();
			}

			for( j = 0; j < pInfo->getRelTexCount(); j++)
			{
				pTex = pInfo->getRelTex( j);

				if( (pTex->getTextureFlag() & I3I_TEXFLAG_MASK_USAGE) == texFlag)
				{
					pPlace = _addTexPlace( pTex);
					pPlace->m_pTargetTex = pInfo->getTexture();
				}
			}
		}
	}

	// 배치시도.
	// Diffuse든, NormalMap이든, 유형 별로 걸리진 List를 MacroTexture에 가상적으로 배치해본다.
	{
		m_idxMacro = 0;

		do
		{
			_Rec_TryToPlace(nullptr);

			m_idxMacro++;							// Macro Texture 추가.

		} while( isAllTexPlaced() == false);		// 모든 Placement 후보가 처리될 때까지 반복.
	}
}

bool i3OptMacroTexture2::isAllTexPlaced(void)
{
	TEX_PLACE_INFO * pInfo;

	for(size_t i = 0; i < m_PlaceTexList.size(); i++)
	{
		pInfo = m_PlaceTexList[i];

		if( pInfo->m_idxMacro == -1)
			return false;
	}

	return true;
}

TEX_PLACE_INFO * i3OptMacroTexture2::_FindTexPlace( i3Texture * pTargetTex)
{
	
	TEX_PLACE_INFO * pInfo;

	for(size_t i = 0; i < m_PlaceTexList.size(); i++)
	{
		pInfo = m_PlaceTexList[i];

		if( pInfo->m_pTargetTex == pTargetTex)
			return pInfo;
	}

	return nullptr;
}

TEX_EXCHANGE_INFO *	i3OptMacroTexture2::findExchange( i3Texture * pTex)
{
	INT32 i;
	TEX_EXCHANGE_INFO * pInfo;

	for( i = 0; i < getExchangeCount(); i++)
	{
		pInfo = (TEX_EXCHANGE_INFO *) getExchange( i);

		if( pInfo->m_pSrcTex == pTex)
			return pInfo;
	}

	return nullptr;
}

TEX_EXCHANGE_INFO *	i3OptMacroTexture2::addExchange( i3Texture * pTex, REAL32 u, REAL32 v, INT32 idxMacro)
{
	TEX_EXCHANGE_INFO * pInfo;

	pInfo = findExchange( pTex);
	if( pInfo == nullptr)
	{
		pInfo = new TEX_EXCHANGE_INFO;
		pInfo->m_idxMacro = idxMacro;
		pInfo->m_U = u;
		pInfo->m_V = v;

		m_ExchangeList.push_back( pInfo);
	}

	I3ASSERT( pInfo->m_U == u);
	I3ASSERT( pInfo->m_V == v);
	I3ASSERT( pInfo->m_idxMacro == idxMacro);

	return pInfo;
}

void	i3OptMacroTexture2::removeAllExchange(void)
{
	INT32 i;
	TEX_EXCHANGE_INFO * pInfo;

	for( i = 0; i < getExchangeCount(); i++)
	{
		pInfo = getExchange( i);

		delete pInfo;
	}

	m_ExchangeList.clear();
}

MACRO_TEX_INFO2 *	i3OptMacroTexture2::addMacroTex( INT32 idx, INT32 maxx, INT32 maxy)
{
	if( (INT32)m_MacroTexList.size() <= idx)
	{
		size_t oldCnt = m_MacroTexList.size();

		m_MacroTexList.resize( idx + 1);

		for( size_t i = oldCnt; i < m_MacroTexList.size(); i++)
		{
			m_MacroTexList[ i] = nullptr;
		}
	}

	// Macro Tex 정보를 갱신
	MACRO_TEX_INFO2 * pMacro = m_MacroTexList[ idx];

	if( pMacro == nullptr)
	{
		// 새로 생성
		pMacro = new MACRO_TEX_INFO2;
		pMacro->m_pMacro = nullptr;
		pMacro->m_Width = 0;
		pMacro->m_Height = 0;

		m_MacroTexList[ idx] = pMacro;
	}

	pMacro->m_Width = max( maxx, pMacro->m_Width);
	pMacro->m_Height = max( maxy, pMacro->m_Height);	

	return pMacro;
}

void	i3OptMacroTexture2::_MakeExchangeList(void)
{
	INT32 i, j, cntMacro;
	GEO_TEXBIND_INFO * pBind;
	TEX_PLACE_INFO * pPlace;
	REAL32 u, v, sx, sy;
	INT32 pcx, pcy, mx, my;
	i3Texture * pTex;

	// 실제 생성되어야 할 MacroTexture의 개수를 구한다.
	cntMacro = m_idxMacro;

	// 각 Texture들마다 변환 Table을 생성해준다.
	for( i = 0; i < _getGeoBindCount(); i++)
	{
		pBind = (GEO_TEXBIND_INFO *) _getGeoBind( i);

		pPlace = _FindTexPlace( pBind->getTexture());
		I3ASSERT( pPlace != nullptr);

		pcx = pPlace->m_pTex->GetWidth();
		pcy = pPlace->m_pTex->GetHeight();

		u = (REAL32) pPlace->m_X / pcx;
		v = (REAL32) pPlace->m_Y / pcy;

		// TargetTex (Diffuse or Lux)
		{
			pTex = pBind->getTexture();

			sx = (REAL32) pTex->GetWidth() / pcx;
			sy = (REAL32) pTex->GetHeight() / pcy;

			mx = (INT32)((pPlace->m_X + pPlace->m_Width) * sx);
			my = (INT32)((pPlace->m_Y + pPlace->m_Height) * sy);

			addMacroTex( pPlace->m_idxMacro, mx, my);

			I3TRACE( "EX : [%d] %s -> (%d, %d)\n", pPlace->m_idxMacro, pTex->GetName(), mx, my);

			addExchange( pBind->getTexture(), u, v, pPlace->m_idxMacro);
		}

		// Related Tex
		for( j = 0; j < pBind->getRelTexCount(); j++)
		{
			pTex = pBind->getRelTex( j);
			INT32 idxMacro;

			switch( pTex->getTextureFlag() & I3I_TEXFLAG_MASK_USAGE)
			{
				case I3I_TEXFLAG_NORMAL :			idxMacro = (cntMacro * 1) + pPlace->m_idxMacro;	break;
				case I3I_TEXFLAG_SPECULAR :			idxMacro = (cntMacro * 2) + pPlace->m_idxMacro; break;
				case I3I_TEXFLAG_REFLECT_MASK :		idxMacro = (cntMacro * 3) + pPlace->m_idxMacro; break;
				default :							idxMacro = pPlace->m_idxMacro;	break;
			}

			sx = (REAL32) pTex->GetWidth() / pcx;
			sy = (REAL32) pTex->GetHeight() / pcy;

			mx = (INT32)((pPlace->m_X + pPlace->m_Width) * sx);
			my = (INT32)((pPlace->m_Y + pPlace->m_Height) * sy);

			addMacroTex( idxMacro, mx, my);

			I3TRACE( "EX : [%d] %s -> (%d, %d)\n", idxMacro, pTex->GetName(), mx, my);

			addExchange( pTex, u, v, idxMacro);
		}
	}
}

void	i3OptMacroTexture2::_CreateMacroTex(void)
{
	MACRO_TEX_INFO2 * pMacro;

	for(size_t i = 0; i < m_MacroTexList.size(); i++)
	{
		pMacro = m_MacroTexList[i];

		I3TRACE( "MACRO[%d] : %dx%d\n", i, pMacro->m_Width, pMacro->m_Height);
	}
}

void	i3OptMacroTexture2::Optimize( i3SceneGraphInfo * pSg)
{
	setSg( pSg);

	// Texture Merge Information을 만든다.
	// Common Sg.를 포함한 전체 Sg.를 임시로 만들어 생성한다.
	i3Node * pRoot;

	if( pSg->getCommonSg() != nullptr)
	{
		i3Node * pTemp;

		pRoot = pSg->getCommonSg();
		
		// 가장 말단을 찾는다.
		for( pTemp = pRoot; pTemp->GetChildCount() > 0;)
		{
			pTemp = pTemp->GetChild( 0);
		}

		// 임시로 Common Sg.와 Instance Sg.를 붙인다.
		if( pTemp->IsChild( pSg->getInstanceSg()) == false)
		{
			pTemp->AddChild( pSg->getInstanceSg());
		}
	}
	else
	{
		// Instance Sg.만을 대상으로 처리
		pRoot = pSg->getInstanceSg();
	}

	// Diffuse에 대한 처리.
	{
		m_Phase = 0;

		// Merge Info를 Build한다.
		Trace( pRoot);

		// Tiling된 Texture에 대해서는 더 이상처리하지 않도록...
		_FilterTiledTextures();

		// Diffuse, Normal, 등의 종류 중 어느 종류가 가장 큰 Texture Size 비중을
		// 차지하지는지 확인하고, 지정 크기(m_TargetCX, m_TargetCY)에 부적합여부를 판별한다
		// 만약 이미 지정 크기보다 크거나 같은 크기라면 이동시키지 못하기 때문에
		// 대상에서 제외한다.
		_RemoveBigTextures();

		// 가장 면적이 큰 종류를 찾는다. (Diffuse, Normal, Specular 등의 Texture 종류들 중에 가장
		// 크기가 큰 Texture를 위주로 배치한다.)
		_BuildPlacementList();

		// Exchange Table을 만든다.
		_MakeExchangeList();

		// 배치된 Texture를 보고 최종적으로 MacroTexture들을 생성한다.
		_CreateMacroTex();

		//_RemoveAllMacroTex();
	}
}


//////////////////////////////////////////////////////////////////////////////////////
GEO_TEXBIND_INFO::GEO_TEXBIND_INFO(void)
{
	m_TexCoordIndex = 0;
	m_pMap = nullptr;
}

GEO_TEXBIND_INFO::~GEO_TEXBIND_INFO(void)
{
	removeAllRelTex();
	removeAllGeo();
	I3_SAFE_RELEASE( m_pMap);
}

void	GEO_TEXBIND_INFO::addRelTex( const i3::vector<i3Texture*>& List)
{
	for( size_t i = 0; i < List.size(); i++)
	{
		i3Texture * pTex = List[i];
		I3_SAFE_ADDREF( pTex);
		i3::vu::push_back_once(m_RelList, pTex);
	}
}

void	GEO_TEXBIND_INFO::removeAllRelTex(void)
{
	

	for(size_t i = 0; i < m_RelList.size(); i++)
	{
		i3Texture * pTex = m_RelList[i];

		I3_MUST_RELEASE(pTex);
	}

	m_RelList.clear();
}

void	GEO_TEXBIND_INFO::addGeo( i3Geometry * pGeo)
{
	I3_MUST_ADDREF( pGeo);

	i3::vu::push_back_once(m_GeoList, pGeo);
}

void	GEO_TEXBIND_INFO::removeAllGeo(void)
{
	
	i3Geometry * pGeo;

	for(size_t i = 0; i < m_GeoList.size(); i++)
	{
		pGeo = getGeo( i);

		I3_MUST_RELEASE(pGeo);
	}

	m_GeoList.clear();
}

void	GEO_TEXBIND_INFO::reset(void)
{
	removeAllGeo();
	removeAllRelTex();
	I3_SAFE_RELEASE( m_pMap);
}

i3Texture *	GEO_TEXBIND_INFO::getRelTexByFlag( UINT32 flag)
{
	INT32 i;

	for( i = 0; i < getRelTexCount(); i++)
	{
		i3Texture * pTex = (i3Texture *) getRelTex( i);

		if( (pTex->getTextureFlag() & I3I_TEXFLAG_MASK_USAGE) == flag)
		{
			return pTex;
		}
	}

	return nullptr;
}
