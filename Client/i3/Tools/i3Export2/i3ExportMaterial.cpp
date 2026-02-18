#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"
#include "FalloffMtl.h"
#include "i3Base/string/ext/extract_filename.h"

void i3Export::_SetTextureSet( INT32 slot, Mtl * pMtl, int TexMapIndex)
{
	m_TexSetInfo[slot].m_pMtl = pMtl;
	m_TexSetInfo[slot].m_pBitmap = pMtl->GetSubTexmap( TexMapIndex);

	if( m_TexSetInfo[slot].m_pBitmap != NULL)
	{
		Matrix3 tmp;

		tmp.SetScale( Point3( 1.0f, -1.0f, 1.0f));

		m_TexSetInfo[slot].m_pBitmap->GetUVTransform( m_TexSetInfo[slot].m_Matrix);
		m_TexSetInfo[slot].m_Matrix = m_TexSetInfo[slot].m_Matrix * tmp;


		if( m_TexSetInfo[slot].m_pBitmap->ClassID().PartA() == BMTEX_CLASS_ID)
		{
			BitmapTex * pBmpTex = (BitmapTex *) m_TexSetInfo[slot].m_pBitmap;

			m_TexSetInfo[slot].m_pUVGen = pBmpTex->GetUVGen();

			//I3TRACE( "Tiling : %d\n", pBmpTex->GetUVGen()->GetTextureTiling());
		}
	}
	else
	{
		m_TexSetInfo[slot].m_pUVGen = NULL;
		m_TexSetInfo[slot].m_Matrix.IdentityMatrix();
	}
}

void i3Export::_OnLuxMapMaterial( INode * pNode, i3AttrSet * pAttrSet, Mtl * pMtl)
{
	i3Texture * pTex;
	Mtl * pLuxMtl = pMtl->GetSubMtl( 1);
	TimeValue time = getStartTime();
	BitmapTex * pBmpTex;

	_SetTextureSet( 1, pLuxMtl, ID_DI);

	if( m_TexSetInfo[1].m_pBitmap->ClassID().PartA() != BMTEX_CLASS_ID)
	{
		Log( pNode, "LuxMap 설정이 되어 있으나, 올바른 방법으로 된 것이 아닙니다.");
		return;
	}

	 pBmpTex = (BitmapTex *) m_TexSetInfo[1].m_pBitmap;

	 if( pBmpTex->GetBitmap( time) == NULL)
	 {
		 Log( pNode, "LuxMap 설정이 되어 있으나, 실제 Bitmap이 존재하지 않습니다. LuxMap Baking을 확인하세요.");
		 return;
	 }

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set( TRUE);

		pAttrSet->AddAttr( pAttr);
	}

	{
		i3LuxMapEnableAttr * pAttr = i3LuxMapEnableAttr::new_object_ref();
		pAttr->Set( TRUE);
		
		pAttrSet->AddAttr( pAttr);
	}

	I3G_TEXTURE_WRAP s = I3G_TEXTURE_WRAP_CLAMP, t = I3G_TEXTURE_WRAP_CLAMP;
	StdUVGen* stdUVGen = m_TexSetInfo[1].m_pUVGen;

	// Texture Wrap (기본 Repeat)
	if( stdUVGen != NULL)
	{
		int tilingType = stdUVGen->GetTextureTiling();

		if( tilingType & U_WRAP)
			s = I3G_TEXTURE_WRAP_WRAP;
		
		if( tilingType & V_WRAP)
			t = I3G_TEXTURE_WRAP_WRAP;

		if( (s != I3G_TEXTURE_WRAP_WRAP) || (t != I3G_TEXTURE_WRAP_WRAP))
		{
			i3LuxMapWrapAttr * pAttr = i3LuxMapWrapAttr::new_object_ref();

			pAttr->SetHorzWrap( s);
			pAttr->SetVertWrap( t);
			pAttrSet->AddAttr( pAttr);
		}
	}

	{
		i3LuxMapBindAttr * pAttr = i3LuxMapBindAttr::new_object_ref();
		
		pTex = _CreateTexture( pBmpTex->GetMapName(), pBmpTex->GetBitmap(time));
		
		pAttr->SetTexture( pTex);
		pAttrSet->AddAttr( pAttr);
	}
}

i3Texture * i3Export::GetTextureSetting( INode * pNode, StdMat * std, int Channel, I3G_TEXTURE_WRAP * pWrapU, I3G_TEXTURE_WRAP * pWrapV)
{
	TimeValue time = getStartTime();
	i3Texture * pTex = NULL;

	if( !std->MapEnabled( Channel))
		return NULL;

	Texmap* pMap = std->GetSubTexmap( Channel);

	if( pMap->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
	{
		BitmapTex * pBmpTex = (BitmapTex *) pMap;
		I3G_TEXTURE_WRAP s = I3G_TEXTURE_WRAP_CLAMP, t = I3G_TEXTURE_WRAP_CLAMP;
		StdUVGen* stdUVGen = pBmpTex->GetUVGen();

		// Texture Wrap (기본 Repeat)
		{
			int tilingType = stdUVGen->GetTextureTiling();
			if( tilingType & U_WRAP)
				s = I3G_TEXTURE_WRAP_WRAP;
			
			if( tilingType & V_WRAP)
				t = I3G_TEXTURE_WRAP_WRAP;

			if( pWrapU != NULL)
				*pWrapU = s;

			if( pWrapV != NULL)
				*pWrapV = t;
		}

		// Texture 
		if( pBmpTex->GetBitmap( time) != NULL)
		{
			pTex = _CreateTexture( pBmpTex->GetMapName(), pBmpTex->GetBitmap( time));
		}
		else
		{
			Log( pNode, "Empty Texture\n");
		}
	}

	return pTex;
}

bool i3Export::_OnExportMaterial( INode * pNode, MtlBase * pMtl, i3AttrSet * pAttrSet)
{
	bool bTexture = false;
	I3G_TEXTURE_WRAP	wrapU, wrapV;
	i3Texture * pTex;

	TimeValue time = getStartTime();

	if( pMtl == NULL)
		return false;
			
	// Color

	// Texture Bind

	// Texture Filter

	// Texture Wrap

	// Texture Function

	// See if it's a Standard material
	if (pMtl->ClassID() == Class_ID( DMTL_CLASS_ID, 0))
	{
		// It is -- Access the two sided property of the material
		StdMat* std = (StdMat *)pMtl;

		if( std->MapEnabled( ID_AM))
		{
			Texmap* ambientMap = std->GetSubTexmap( ID_AM);

			if( ambientMap != NULL)
			{
				UINT32 partA = ambientMap->ClassID().PartA();
				UINT32 partB = ambientMap->ClassID().PartB();

				if( ambientMap->ClassID() == Class_ID( FALLOFF_MTL_CLASS_ID, 0))
				{
					FalloffMtl * pFalloff = (FalloffMtl *) ambientMap;

					pFalloff->Bind();

					I3TRACE( "Index Of Refraction : %f\n", pFalloff->GetIndexOfRefract());
					//pFalloff->DumpParam();

					{
						i3FresnelAttr * pAttr = i3FresnelAttr::new_object_ref();

						pAttr->setEnable( true);
						pAttr->setIOR( pFalloff->GetIndexOfRefract());

						COLORREAL col;
						pFalloff->GetColor2( &col);
						pAttr->setColor( &col);

						pAttrSet->AddAttr( pAttr);
					}
				}
			}
		}

		// Texture Enable (기본 Off)
		{
			i3Texture * pTex = GetTextureSetting( pNode, std, ID_DI, &wrapU, &wrapV);

			if( pTex != NULL)
			{
				bTexture = true;

				{
					i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();

					pAttr->Set( TRUE);

					pAttrSet->AddAttr( pAttr);
				}

				if( (wrapU != I3G_TEXTURE_WRAP_WRAP) || (wrapV != I3G_TEXTURE_WRAP_WRAP))
				{
					i3TextureWrapAttr * pAttr = i3TextureWrapAttr::new_object_ref();

					pAttr->SetHorzWrap( wrapU);
					pAttr->SetVertWrap( wrapV);

					pAttrSet->AddAttr( pAttr);
				}

				{
					i3TextureBindAttr * pAttr = i3TextureBindAttr::new_object_ref();

					pAttr->SetTexture( pTex);
					pAttrSet->AddAttr( pAttr);

					pTex->Release(); 
				}
			}
		}
		
		// Normal Mapping
		if( std->MapEnabled( ID_BU))
		{
			Texmap * normalMap = std->GetSubTexmap( ID_BU);

			{
				i3NormalMapEnableAttr * pAttr = i3NormalMapEnableAttr::new_object_ref();

				pAttr->Set( TRUE);
				pAttrSet->AddAttr( pAttr);
			}

			// Normap Map은 기본 Texmap안에 다시 첫번째 SubTexmap으로 존재한다.
			{
				Texmap * pSubTexmap = normalMap->GetSubTexmap( 0);

				if( pSubTexmap != NULL)
				{
					if( pSubTexmap->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
					{
						BitmapTex * pBmpTex = (BitmapTex *) pSubTexmap;

						// Texture 
						if( pBmpTex->GetBitmap( time) != NULL)
						{
							i3Texture * pTex;

							pTex = _CreateTexture( pBmpTex->GetMapName(), pBmpTex->GetBitmap( time));

							if( pTex != NULL)
							{
								i3NormalMapBindAttr * pAttr = i3NormalMapBindAttr::new_object_ref();

								pAttr->SetTexture( pTex);
								pAttrSet->AddAttr( pAttr);

								pTex->Release(); 
							}

							bTexture = true;
						}
						else
						{
							Log( pNode, "Empty Normal Texture\n");
						}
						
					}
				}
			}
		}

		{
			// Specular Map
			pTex = GetTextureSetting( pNode, std, ID_SP, &wrapU, &wrapV);

			if( pTex != NULL)
			{
				bTexture = true;

				{
					i3SpecularMapEnableAttr * pAttr = i3SpecularMapEnableAttr::new_object_ref();

					pAttr->Set( TRUE);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3SpecularMapBindAttr * pAttr = i3SpecularMapBindAttr::new_object_ref();

					pAttr->SetTexture( pTex);
					pAttrSet->AddAttr( pAttr);

					pTex->Release(); 
				}
			}
		}

		{
			// Emissive Map
			pTex = GetTextureSetting( pNode, std, ID_SI, &wrapU, &wrapV);

			if( pTex != NULL)
			{
				bTexture = true;

				{
					i3EmissiveMapEnableAttr * pAttr = i3EmissiveMapEnableAttr::new_object_ref();

					pAttr->Set( TRUE);
					pAttrSet->AddAttr( pAttr);
				}

				{
					i3EmissiveMapBindAttr * pAttr = i3EmissiveMapBindAttr::new_object_ref();

					pAttr->SetTexture( pTex);
					pAttrSet->AddAttr( pAttr);

					pTex->Release(); 
				}
			}
		}

		// Reflect Mapping
		if( std->MapEnabled( ID_RL))
		{
			Texmap * reflectMap = std->GetSubTexmap( ID_RL);

			// Reflect Map은 다시 Reflect Cube Map과 Reflect Mask Map으로
			// 구분되어 설정된다.
			if( reflectMap->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
			{
				// 2D Texture인지, 3D Texture인지 확인 작업이 필요..
			}
			else if( reflectMap->ClassID() == Class_ID( MASK_CLASS_ID, 0))
			{
				// Reflect Map
				Texmap * pSubTexmap = reflectMap->GetSubTexmap( 0);
				bool bReflectEnableAdded = false;

				if( pSubTexmap != NULL)
				{
					if( pSubTexmap->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
					{
						// 3D Texture여야 한다.
						BitmapTex * pBmpTex = (BitmapTex *) pSubTexmap;

						// Texture 
						if( pBmpTex->GetBitmap( time) != NULL)
						{
							i3Texture * pTex;

							pTex = _CreateTexture( pBmpTex->GetMapName(), pBmpTex->GetBitmap( time));

							if( pTex != NULL)
							{
								{
									i3ReflectMapEnableAttr * pAttr = i3ReflectMapEnableAttr::new_object_ref();

									pAttr->Set( TRUE);
									pAttrSet->AddAttr( pAttr);

									bReflectEnableAdded = true;
								}

								{
									i3ReflectMapBindAttr * pAttr = i3ReflectMapBindAttr::new_object_ref();

									pAttr->SetTexture( pTex);
									pAttrSet->AddAttr( pAttr);
								}

								pTex->Release(); 
							}
						}
						else
						{
							Log( pNode, "Empty Reflect-Mask Texture\n");
						}
					}
					/*
					if( pSubTexmap->ClassID() == Class_ID( ACUBIC_CLASS_ID, 0))
					{
						StdCubic * pBmpTex = (StdCubic *) pSubTexmap;

						// Texture 
						IParamArray * pArray = pBmpTex->GetParamBlock();
					}
					*/
				}
				else
				{
					// Reflect Cube Map이 설정되지 않은 경우.
					// 문제삼지 않는다.
				}

				// Reflect-Mask Map
				pSubTexmap = reflectMap->GetSubTexmap( 1);
				if( pSubTexmap != NULL)
				{
					if( pSubTexmap->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
					{
						BitmapTex * pBmpTex = (BitmapTex *) pSubTexmap;

						// Texture 
						if( pBmpTex->GetBitmap( time) != NULL)
						{
							i3Texture * pTex;

							pTex = _CreateTexture( pBmpTex->GetMapName(), pBmpTex->GetBitmap( time));

							if( pTex != NULL)
							{
								if( bReflectEnableAdded == false)
								{
									i3ReflectMapEnableAttr * pAttr = i3ReflectMapEnableAttr::new_object_ref();

									pAttr->Set( TRUE);
									pAttrSet->AddAttr( pAttr);
								}

								{
									i3ReflectMaskMapEnableAttr * pAttr = i3ReflectMaskMapEnableAttr::new_object_ref();

									pAttr->Set( TRUE);
									pAttrSet->AddAttr( pAttr);
								}

								{
									i3ReflectMaskMapBindAttr * pAttr = i3ReflectMaskMapBindAttr::new_object_ref();

									pAttr->SetTexture( pTex);
									pAttrSet->AddAttr( pAttr);
								}

								pTex->Release(); 
							}
						}
						else
						{
							Log( pNode, "Empty Reflect-Mask Texture\n");
						}
					}
				}
				else
				{
					// Reflect-Mask Map이 설정되지 않은 경우
					// 문제삼지 않는다.
				}
			}
		}

		// Lighting Enable (기본 On)
		if( std->GetShading() == SHADE_CONST)
		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set( FALSE);
			pAttrSet->AddAttr( pAttr);
		}

		// Material Attribute
		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();
			Color clr1, clr2, clr3; //, clr4;
			REAL32 opq = std->GetOpacity( time);
			REAL32 ss = std->GetShinStr( time);

			clr1 = std->GetAmbient( time);
			clr2 = std->GetDiffuse( time);
			clr3 = std->GetSpecular( time);

			pAttr->SetDiffuse( clr2.r, clr2.g, clr2.b, opq);
			pAttr->SetSpecular( clr3.r * ss, clr3.g * ss, clr3.b * ss, opq);
			pAttr->SetShininess( std->GetShininess( time) * 128.0f);

			if( std->GetSelfIllumColorOn())
			{
				Color c = std->GetSelfIllumColor();

				pAttr->SetEmissive( c.r, c.g, c.b, 1.0f);
			}

			pAttrSet->AddAttr( pAttr);
		}

		// Blending Enable (기본 Off)
		if( std->MapEnabled( ID_OP))
		{
			// Alpha Test Mode
			{
				i3AlphaTestEnableAttr * pAttr2 = i3AlphaTestEnableAttr::new_object_ref();

				pAttr2->Set( TRUE);

				pAttrSet->AddAttr( pAttr2);
			}

			// Alpha Func
			{
				i3AlphaFuncAttr * pAttr = i3AlphaFuncAttr::new_object_ref();

				pAttr->SetRefValue( 0);

				pAttrSet->AddAttr( pAttr);
			}

			Texmap* pOpaqueMap = std->GetSubTexmap( ID_OP);

			// Blending 여부 판별
			//
			// "Premultiplied Alpha" 설정을 사용한다.
			//
			if( pOpaqueMap->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
			{
				BitmapTex * pBmpTex = (BitmapTex *) pOpaqueMap;
				Bitmap * pBitmap = pBmpTex->GetBitmap( time);

				if( pBmpTex->GetPremultAlpha( TRUE) == FALSE)
				{
					// Blend Enable
					{
						i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

						pAttr->Set( TRUE);
						pAttrSet->AddAttr( pAttr);
					}

					// Blending Mode
					{
						i3BlendModeAttr * pAttr2 = i3BlendModeAttr::new_object_ref();

						pAttr2->SetSource( I3G_BLEND_SRCALPHA);

						switch( std->GetTransparencyType())
						{
							case TRANSP_ADDITIVE :		pAttr2->SetDestination( I3G_BLEND_ONE);	break;
							default :					pAttr2->SetDestination( I3G_BLEND_INVSRCALPHA);	break;
						}

						pAttrSet->AddAttr( pAttr2);
					}

					pAttrSet->AddFlag( I3_NODEFLAG_TRANSPARENT);
				}
			}
		}

		// Face-Cull
		if( std->GetTwoSided())
		{
			i3FaceCullModeAttr * pAttr = i3FaceCullModeAttr::new_object_ref();

			pAttr->Set( I3G_FACE_CULL_NONE);
			pAttrSet->AddAttr( pAttr);
		}
	}

	return bTexture;
}

static void _Rec_MatchMaterialNameByTex( Mtl * pMtl)
{
	INT32 i;
	Mtl * pSubMtl;

	if( pMtl->ClassID() == Class_ID( DMTL_CLASS_ID, 0))
	{
		StdMat * pStd = (StdMat *) pMtl;

		if( pStd->NumSubMtls() == 0)
		{
			if( pStd->MapEnabled( ID_DI))
			{
				Texmap * pMap = pMtl->GetSubTexmap( ID_DI);

				if( pMap->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
				{
					BitmapTex * pBmpTex = (BitmapTex *) pMap;

					if( (pBmpTex != NULL) && (pBmpTex->GetMapName() != NULL))
					{
						char szName[MAX_PATH], szFull[MAX_PATH];

						strcpy( szFull, pBmpTex->GetMapName());

						if( szFull[0] != 0)
						{
							i3::extract_filetitle( szFull, szName);

							TSTR s( szName);

							I3TRACE( "Name Adjust : %s -> %s\n", pMtl->GetName(), szName);

							pMtl->SetName(s);
						}
					}
				}
			}
		}
	}

	for( i = 0; i < pMtl->NumSubMtls(); i++)
	{
		pSubMtl = pMtl->GetSubMtl( i);

		if( pSubMtl == NULL)
			continue;

		_Rec_MatchMaterialNameByTex( pSubMtl);
	}
}

static void _Rec_MatchMaterialName( INode * pNode)
{
	Mtl * pMtl = pNode->GetMtl();

	if( pMtl != NULL)
	{
		_Rec_MatchMaterialNameByTex( pMtl);
	}

	INT32 i;
	INode * pChild;

	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		pChild = pNode->GetChildNode( i);

		_Rec_MatchMaterialName( pChild);
	}
}

void i3Export::MatchMaterialNamesWithTexture(void)
{
	Interface * ip = GetCOREInterface();

	_Rec_MatchMaterialName( ip->GetRootNode());
}