#include "i3OptPCH.h"
#include "i3OptConvertVertexFormat.h"
#include "i3Scene.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/atoi.h"

//I3_CLASS_INSTANCE( i3OptConvertVertexFormat, i3SceneOptimizer);
I3_CLASS_INSTANCE( i3OptConvertVertexFormat);

i3OptConvertVertexFormat::i3OptConvertVertexFormat(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;

	SetPositionConvertEnable(true);
	SetNormalConvertEnable(true);
	SetColorConvertEnable(true);
	SetWeightConvertEnable(true);
	SetTextureCoordConvertEnable(true);

	SetPositionType( I3VF_TYPE_REAL32);
	SetNormalType( I3VF_TYPE_REAL32);
	SetColorType( I3VF_TYPE_COLOR32_RGBA_8888);
	SetWeightType( I3VF_TYPE_REAL32);
	SetTextureCoordType( I3VF_TYPE_REAL32);
}

void i3OptConvertVertexFormat::CopyVertexArray( i3VertexArray * pDest, i3VertexArray * pSrc, REAL32 sx, REAL32 sy, REAL32 sz, VEC3D * pOffset)
{
	INT32 i, j;
	i3VertexFormat format;
	VEC3D scale;
	i3VertexArrayMem * pMemVA = nullptr;

	m_rTestMin_U = 1000.0f;
	m_rTestMax_U = -1000.0f;
	m_rTestMin_V = 1000.0f;
	m_rTestMax_V = -1000.0f;

	i3Vector::Set( &scale, 1.0f / sx, 1.0f / sy, 1.0f / sz);
	
	pSrc->Lock();
	pDest->Lock();

	format = * pDest->GetFormat();

	if( format.GetPlatform() != I3VF_PLATFORM_NA)
	{
		// 특정 Platform으로 지정된 Format을 생성하는 경우.
		// Persistant Data를 만들기 위해 i3VertexArrayMem class를 이용한다.
		pMemVA = i3VertexArrayMem::new_object();

		pMemVA->Create( &format, pDest->GetCount(), 0);
		pMemVA->Lock();
	}

	for( i = 0; i < (INT32) pSrc->GetCount(); i++)
	{
		for( j = 0; j < format.GetTextureCoordSetCount(); j++)
		{
			VEC2D vec;

			pSrc->GetTextureCoord( j, i, &vec);

			m_rTestMin_U = MIN(m_rTestMin_U, i3Vector::GetX(&vec));
			m_rTestMax_U = MAX(m_rTestMax_U, i3Vector::GetX(&vec));

			m_rTestMin_V = MIN(m_rTestMin_V, i3Vector::GetY(&vec));
			m_rTestMax_V = MAX(m_rTestMax_V, i3Vector::GetY(&vec));
		}
	}


	for( i =  0; i < (INT32) pSrc->GetCount(); i++)
	{
		if( format.GetHasPosition())
		{
			VEC3D vec;

			pSrc->GetPosition( i, &vec);

			i3Vector::Sub( &vec, &vec, pOffset);
			i3Vector::Mul( &vec, &vec, &scale);

			pDest->SetPosition( i, &vec);
				
			if( pMemVA != nullptr)
			{
				pMemVA->SetPosition( i, &vec);
			}
		}

		if( format.GetHasNormal())
		{
			VEC3D vec;

			pSrc->GetNormal( i, &vec);
			pDest->SetNormal( i, &vec);	

			if( pMemVA != nullptr)
				pMemVA->SetNormal( i, &vec);
		}

		if( format.GetHasColor())
		{
			COLORREAL col;

			pSrc->GetColor( i, &col);
			pDest->SetColor( i, &col);

			if( pMemVA != nullptr)
				pMemVA->SetColor( i, &col);
		}

		for( j = 0; j < format.GetTextureCoordSetCount(); j++)
		{
			VEC2D vec;

			pSrc->GetTextureCoord( j, i, &vec);
			
			//if( i3Vector::GetX(&vec) > 1.0f || i3Vector::GetX(&vec) < 0.0f || i3Vector::GetY(&vec) > 1.0f || i3Vector::GetY(&vec) < 0.0f) 
			if(m_rTestMax_U > 1.0f || m_rTestMax_V > 1.0f || m_rTestMin_U < 0.0f || m_rTestMin_V < 0.0f) 
			{
				i3Vector::SetX(&vec, abs((i3Vector::GetX(&vec) - m_rTestMin_U) / (m_rTestMax_U - m_rTestMin_U)) );
				i3Vector::SetY(&vec, abs((i3Vector::GetY(&vec) - m_rTestMin_V) / (m_rTestMax_V - m_rTestMin_V)) );

				if( i3Vector::GetX( &vec) <= 0.0f)
					i3Vector::SetX( &vec, 0.0f);

				if( i3Vector::GetX( &vec) >= 1.0f)
					i3Vector::SetX( &vec, 1.0f);

				if( i3Vector::GetY( &vec) <= 0.0f)
					i3Vector::SetY( &vec, 0.0f);

				if( i3Vector::GetY( &vec) >= 1.0f)
					i3Vector::SetY( &vec, 1.0f);
			}
						
			pDest->SetTextureCoord( j, i, &vec);

			if( pMemVA != nullptr)
				pMemVA->SetTextureCoord( j, i, &vec);
		}

		for( j = 0; j < format.GetBlendIndexCount(); j++)
		{
			pDest->SetBlendIndex( j, i, pSrc->GetBlendIndex( j, i));

			if( pMemVA != nullptr)
				pMemVA->SetBlendIndex( j, i, pSrc->GetBlendIndex( j, i));
		}

		for( j = 0; j < format.GetBlendWeightCount(); j++)
		{
			pDest->SetBlendWeight( j, i, pSrc->GetBlendWeight( j, i));

			if( pMemVA != nullptr)
				pMemVA->SetBlendWeight( j, i, pSrc->GetBlendWeight( j, i));
		}
	}

	pSrc->Unlock();
	pDest->Unlock();

	if( pMemVA != nullptr)
	{
		// 특정 Platform용의 Persistant Data가 있는 경우.
		UINT32 size = pMemVA->GetStride() * pMemVA->GetCount();
		char * pBuf = (char *) i3MemAlloc( size);
	
		memcpy( pBuf, pMemVA->GetLockedBuffer(), size);

		pDest->SetPersistData( pBuf );

		// Persist Format과 Runtime Format이 다르다는 것을
		// 알리기 위한 조치.
		*pDest->GetPersistFormat() = *pDest->GetFormat();		
		pDest->GetFormat()->SetPlatform( I3VF_PLATFORM_NA);

		pDest->SetModifiedFlag(false);

		pMemVA->Unlock();
		I3_MUST_RELEASE( pMemVA);
	}
}

void i3OptConvertVertexFormat::Trace( i3Node * pRoot)
{
	i3Geometry * pGeo;
	i3RenderAttr * pAttr;
	INT32 j;
	INT32 count;
	VEC3D tempmin, tempmax, min, max;
	i3GeometryAttr * pGeoAttr;
	VEC3D mid;
	REAL32 sx, sy, sz, cx, cy, cz;
	i3VertexArray * pOldVA, * pNewVA;
	i3VertexFormat format;
	i3BoundBox box;
	i3::vector<i3Node*> List;
	i3Node* pParentNode;

	i3Scene::FindNodeByExactType( pRoot, i3Geometry::static_meta(), List);

	for(size_t i = 0; i < List.size(); i++)
	{
		pGeo = (i3Geometry *) List[i];

		// i3GeometryAttr만 존재하는 Geometry인가를 확인한다.
		for( count = 0, j = 0; j < pGeo->GetGeometryAttrCount(); j++)
		{
			pAttr = (i3RenderAttr *) pGeo->GetGeometryAttr(j);

			if( i3::kind_of<i3GeometryAttr* >(pAttr))
			{
				pGeoAttr = (i3GeometryAttr *) pAttr;

				// i3Geometry Node 안에 포함되어 있는 i3GeometryAttr들의 전체 BoundBox를 구한다.
				pGeoAttr->CalcBoundBox( &tempmin, &tempmax);
				if( count == 0)
				{
					i3Vector::Copy( &min, &tempmin);
					i3Vector::Copy( &max, &tempmax);
				}
				else
				{
					i3Vector::Minimize( &min, &min, &tempmin);
					i3Vector::Maximize( &max, &max, &tempmax);
				}
				count ++;
			}
		}

		if( count != pGeo->GetGeometryAttrCount())
			continue;

		// Fixed-Point로 바뀌는 경우, 표현 가능한 수의 범위가 -1~1 사이의 값이
		// 되기 때문에 Scale 및 Translation에 대한 값을 표현해 주기 위한 i3Transform Node를
		// i3Geometry Node의 상위에 넣어준다.
		if( m_PosType != I3VF_TYPE_REAL32)
		{
			i3Node * pParent;
			i3Transform * pTrans;
			MATRIX tmx;
			VEC3D vec1, vec2;

			// 중앙점을 구한다.
			i3Vector::Add( &mid, &min, &max);
			i3Vector::Scale( &mid, &mid, 0.5f);

			i3Vector::Sub( &vec1, &min, &mid);
			i3Vector::Sub( &vec2, &max, &mid);

			sx = MAX( abs( i3Vector::GetX(&vec1)), abs(i3Vector::GetX(&vec2)));
			sy = MAX( abs( i3Vector::GetY(&vec1)), abs(i3Vector::GetY(&vec2)));
			sz = MAX( abs( i3Vector::GetZ(&vec1)), abs(i3Vector::GetZ(&vec2)));

			cx = MAX( abs( i3Vector::GetX(&min)), abs(i3Vector::GetX(&max)));
			cy = MAX( abs( i3Vector::GetY(&min)), abs(i3Vector::GetY(&max)));
			cz = MAX( abs( i3Vector::GetZ(&min)), abs(i3Vector::GetZ(&max)));

			if((cx > 1.0f) || (cy > 1.0f) || (cz > 1.0f))
			{
				pTrans = i3Transform::new_object_ref();

				i3Matrix::SetScale( &tmx, sx, sy, sz);
				i3Matrix::PostTranslate( &tmx, &mid);

				pTrans->SetMatrix( &tmx);

				I3_MUST_ADDREF( pGeo);

				pParent = pGeo->GetParent();

				if( pParent != nullptr)
				{
					pParent->RemoveChild( pGeo);
					pParent->AddChild( pTrans);
				}

				pTrans->AddChild( pGeo);
				pTrans->SetBound( pGeo->GetBound());

				I3_MUST_RELEASE( pGeo);
			}
			else
			{
				I3TRACE( "Skipped %f, %f, %f - %f, %f, %f\n", sx, sy, sz,
					i3Vector::GetX( &mid),
					i3Vector::GetY( &mid),
					i3Vector::GetZ( &mid));

				sx = 1.0f;
				sy = 1.0f;
				sz = 1.0f;
			}
		}
		else
		{
			sx = sy = sz = 1.0f;
			i3Vector::Set( &mid, 0.0f, 0.0f, 0.0f);
		}

		box.Reset();

		// i3GeometryAttr 들이 가지고 있는 i3VertexArray를
		// Converting하기 위해, 새로운 i3VertexArray를 생성한다.
		for( j = 0; j < pGeo->GetGeometryAttrCount(); j++)
		{
			pGeoAttr = (i3GeometryAttr *) pGeo->GetGeometryAttr( j);

			pOldVA = pGeoAttr->GetVertexArray();

			format = * pOldVA->GetFormat();

			// 새롭게 생성할 Vertex Format을 설정한다.
			if( GetPositionConvertEnable())
				format.SetPositionType( m_PosType);

			if( GetNormalConvertEnable())
				format.SetNormalType( m_NormalType);

			if( GetColorConvertEnable())
				format.SetColorType( m_ColorType);

			if( GetTextureCoordConvertEnable())
				format.SetTextureCoordType( m_TexCoordType);

			if( GetWeightConvertEnable())
				format.SetBlendWeightType( m_WeightType);

			format.SetPlatform( m_Platform);

			pNewVA = i3VertexArray::new_object_ref();
			//pNewVA->SetColorOrder( m_ColorOrder);
			pNewVA->Create( &format, pOldVA->GetCount(), pOldVA->GetUsageFlag());

			CopyVertexArray( pNewVA, pOldVA, sx, sy, sz, &mid);

			if(m_rTestMax_U > 1.0f || m_rTestMax_V > 1.0f || m_rTestMax_U < 0.0f || m_rTestMax_V < 0.0f) 
			{
				VEC3D vec;
				i3AttrSet* pTexAttrSet = i3AttrSet::new_object_ref();

				{
					i3TextureCoordScaleAttr *pTexAttr = i3TextureCoordScaleAttr::new_object_ref();
					i3Vector::Set(&vec, m_rTestMax_U - m_rTestMin_U, m_rTestMax_V - m_rTestMin_V, 1.0f);
					pTexAttr->SetScaleFactor( &vec);
					pTexAttr->SetOutputElementCount(2);
					pTexAttrSet->AddAttr( (i3RenderAttr*)pTexAttr );
				}

				{
					i3TextureCoordOffsetAttr * pAttr = i3TextureCoordOffsetAttr::new_object_ref();

					i3Vector::Set( &vec, m_rTestMin_U, m_rTestMin_V, 0.0f);
					pAttr->SetOffset( &vec);
					pAttr->SetOutputElementCount( 2);
					pTexAttrSet->AddAttr( pAttr);
				}
				
				pParentNode = pGeo->GetParent();

				pGeo->GetParent()->AddChild(pTexAttrSet);					
				pTexAttrSet->AddChild(pGeo);
				pParentNode->RemoveChild(pGeo);
			}

			// 새로운 i3VertexArray로 교체한다.
			pGeoAttr->SetVertexArray( pNewVA);

			pGeoAttr->CalcBoundBox( &min, &max);
			box.ExtendByVec( &min);
			box.ExtendByVec( &max);
		}

		if( pGeo->GetBound() != nullptr)
		{
			i3BoundBox * pNew = i3BoundBox::new_object_ref();

			pNew->SetMinMax( box.GetMin(), box.GetMax());
			pGeo->SetBound( pNew);
		}

	}
}

I3VF_TYPES i3OptConvertVertexFormat::_getTypes( char * pszValue)
{
	//	Add by KOMET
	i3::iterator_range<const char*> strValue = i3::as_literal(pszValue);

	if( i3::generic_is_iequal( strValue, "I3VF_TYPE_REAL32") )						return I3VF_TYPE_REAL32;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED32_16") )				return I3VF_TYPE_FIXED32_16;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED16_8") )				return I3VF_TYPE_FIXED16_8;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED16_12") )				return I3VF_TYPE_FIXED16_12;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED16_15") )				return I3VF_TYPE_FIXED16_15;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED8_4") )				return I3VF_TYPE_FIXED8_4;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED8_7") )				return I3VF_TYPE_FIXED8_7;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_COLOR32_RGBA_8888") )		return I3VF_TYPE_COLOR32_RGBA_8888;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_COLOR16_RGBA_1555") )		return I3VF_TYPE_COLOR16_RGBA_1555;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_COLOR16_RGBX_565") )		return I3VF_TYPE_COLOR16_RGBX_565;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_COLOR16_RGBA_4444") )		return I3VF_TYPE_COLOR16_RGBA_4444;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED16_0"))				return I3VF_TYPE_FIXED16_0;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED8_0") )				return I3VF_TYPE_FIXED8_0;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED16_15_U"))			return I3VF_TYPE_FIXED16_15_U;
	else if( i3::generic_is_iequal( strValue, "I3VF_TYPE_FIXED8_7_U"))				return I3VF_TYPE_FIXED8_7_U;
	else if( i3::generic_is_iequal( strValue, "I3VF_COLORORDER_ARGB") )				return I3VF_COLORORDER_ARGB;
	else if( i3::generic_is_iequal( strValue, "I3VF_COLORORDER_RGBA") )				return I3VF_COLORORDER_RGBA;
	
	return 9999;
}

void i3OptConvertVertexFormat::SetProperty( char * pszFieldName, char * pszValue)	
{
	//	Add by KOMET
	bool nFlag = false;

	if( i3::generic_is_iequal( pszFieldName, "PositionConvertEnable") )
	{		
		if ( i3::generic_is_niequal(pszValue, "T", 1) ||
			i3::generic_is_niequal(pszValue, "Y", 1) )
			nFlag = true;	
		else if ( i3::generic_is_niequal(pszValue, "F", 1) ||
				i3::generic_is_niequal(pszValue, "N", 1) )
			nFlag = false;
		else
		{
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);
			return;
		}

		SetPositionConvertEnable( nFlag);		
		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, "PositionType") )
	{
		I3VF_TYPES nTypes = _getTypes( pszValue);

		if( nTypes != 9999)
		{
			SetPositionType( nTypes);
			return;
		}
	}
	else if( i3::generic_is_iequal( pszFieldName, "NormalConvertEnable") )
	{
		if (i3::generic_is_niequal(pszValue, "T", 1) ||
			i3::generic_is_niequal(pszValue, "Y", 1) )
			nFlag = true;
		else if (i3::generic_is_niequal(pszValue, "F", 1) ||
				i3::generic_is_niequal(pszValue, "N", 1) )
			nFlag = false;
		else
		{
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);
			return;
		}

		SetNormalConvertEnable( nFlag);		
		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, "NormalType") )
	{
		I3VF_TYPES nTypes = _getTypes( pszValue);

		if( nTypes != 9999)
		{
			SetNormalType( nTypes);
			return;
		}
	}
	else if( i3::generic_is_iequal( pszFieldName, "ColorConvertEnable") )
	{
		if ( i3::generic_is_niequal(pszValue, "T", 1) ||
			i3::generic_is_niequal(pszValue, "Y", 1) )
			nFlag = true;
		else if(	i3::generic_is_niequal(pszValue, "F", 1) ||
					i3::generic_is_niequal(pszValue, "N", 1) )
			nFlag = false;
		else
		{
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);
			return;
		}

		SetColorConvertEnable( nFlag);		
		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, "ColorType") )
	{
		I3VF_TYPES nTypes = _getTypes( pszValue);

		if( nTypes != 9999)
		{
			SetColorType( nTypes);
			return;
		}
	}
	else if( i3::generic_is_iequal( pszFieldName, "WeightConvertEnable") )
	{
		if (i3::generic_is_niequal( pszValue, "T", 1) ||
			i3::generic_is_niequal( pszValue, "Y", 1) )
			nFlag = true;
		else if (i3::generic_is_niequal( pszValue, "F", 1) ||
			i3::generic_is_niequal( pszValue, "N", 1) ) 
			nFlag = false;
		else
		{
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);
			return;
		}

		SetWeightConvertEnable( nFlag);		
		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, "WeightType") )
	{
		I3VF_TYPES nTypes = _getTypes( pszValue);

		if( nTypes != 9999)
		{
			SetWeightType( nTypes);
			return;
		}
	}
	else if( i3::generic_is_iequal( pszFieldName, "TextureCoordConvertEnable") )
	{
		if (i3::generic_is_niequal(pszValue, "T", 1) ||
			i3::generic_is_niequal(pszValue, "Y", 1) )
			nFlag = true;
		else if(i3::generic_is_niequal(pszValue, "F", 1) ||
				i3::generic_is_niequal(pszValue, "N", 1) )
			nFlag = false;
		else
		{
			I3TRACE("SetProperty : Invalid Value (%s)\n", pszValue);
			return;
		}

		SetTextureCoordConvertEnable( nFlag);		
		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, "TextureCoordType") )
	{
		I3VF_TYPES nTypes = _getTypes( pszValue);

		if( nTypes != 9999)
		{
			SetTextureCoordType( nTypes);
			return;
		}
	}
	else if( i3::generic_is_iequal( pszFieldName, "ColorOrder") )
	{
		INT32 nOrder = i3::atoi( pszValue);
		SetColorOrder( nOrder);

		return;
	}
	else if( i3::generic_is_iequal( pszFieldName, "TargetPlatform") )
	{
		INT32 nPlatform = i3::atoi( pszValue);
		SetTargetPlatform( nPlatform);

		return;
	}

	I3TRACE("(%s) SetProperty : Unknown field name (%s = %s)\n", 
		static_meta()->class_name(),   // 좀 이상해보이지만 그대로 치환.(2012.08.07.수빈)
		pszFieldName, pszValue);	
}