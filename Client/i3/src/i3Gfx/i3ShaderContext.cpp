#include "i3GfxType.h"
#include "i3ShaderContext.h"
#include "i3RenderContext.h"
#include "i3ShaderParam.h"

//I3_CLASS_INSTANCE( i3ShaderContext, i3ElementBase);
I3_CLASS_INSTANCE( i3ShaderContext);

i3ShaderContext::i3ShaderContext(void)
{
	for (INT32 i = 0; i < I3G_SHADER_TYPE_MAX; i++)
	{
		m_pShaderBin[i] = nullptr;
		m_ParamListArray[i].reserve( 16);
	}
}

i3ShaderContext::~i3ShaderContext(void)
{


	// Shader는 Referencing하지 않는다.
	RemoveAllParams();
	_ReleaseShaders();

	I3MEM_SAFE_FREE( m_pErrorMsg);


}

void i3ShaderContext::SetErrorMsg( char * pszMsg, INT32 len)
{
	I3MEM_SAFE_FREE( m_pErrorMsg);

	if( len > 0)
	{
		m_pErrorMsg = (char *) i3MemAlloc( len + 1);
		I3ASSERT( m_pErrorMsg != nullptr);

		memcpy( m_pErrorMsg, pszMsg, len);
		m_pErrorMsg[len] = 0;
	}
}

void i3ShaderContext::ReportErrorMsg(void)
{
	if( m_pErrorMsg == nullptr)
		return;

	I3TRACE( "SHADER : %s\n", m_pErrorMsg);

	//i3Error::SetEnv( (char *) m_pShader->GetName(), 0, "", I3LOG_WARN);
	//i3Error::Log( m_pErrorMsg);
}

void i3ShaderContext::AddParam( I3G_SHADER_TYPE type, i3ShaderParam * pParam)
{
	I3_MUST_ADDREF( pParam);

	i3::vector<i3ShaderParam*>& List = m_ParamListArray[ type];

	List.push_back( pParam);

	if( type == I3G_SHADER_TYPE_PIXEL)
	{
		pParam->setTarget( i3ShaderParam::TARGET_PIXELSHADER);
	}
	else
	{
		pParam->setTarget( i3ShaderParam::TARGET_VERTEXSHADER);
	}
}

void i3ShaderContext::RemoveParam( I3G_SHADER_TYPE type, i3ShaderParam * pParam)
{
	I3ASSERT( pParam != nullptr);

	I3_MUST_RELEASE( pParam);

	/*
	switch( type)
	{
		case I3G_SHADER_TYPE_VERTEX :			m_VS0ParamList.Remove( pParam);	break;
		case I3G_SHADER_TYPE_BLENDED_VERTEX :	m_VS1ParamList.Remove( pParam);	break;
		default :								m_PSParamList.Remove( pParam);	break;
	}
	*/
	I3ASSERT_0;
}

void i3ShaderContext::RemoveAllParams(void)
{
	INT32  j;
	i3ShaderParam * pParam;

	for( j = 0; j < I3G_SHADER_TYPE_MAX; j++)
	{
		i3::vector<i3ShaderParam*>& List = m_ParamListArray[j];

		for( size_t i = 0; i < List.size(); i++)
		{
			pParam = List[i];
			I3_MUST_RELEASE( pParam);
		}
		List.clear();
	}
}

UINT32 i3ShaderContext::getParamCount( I3G_SHADER_TYPE type)
{
	return m_ParamListArray[ type].size();
}

i3ShaderParam *	i3ShaderContext::getParam( I3G_SHADER_TYPE type, INT32 idx)
{
	return m_ParamListArray[type][ idx];
}

i3ShaderParam *	i3ShaderContext::FindParam( I3G_SHADER_TYPE type, const char * pszName)
{
		
	i3ShaderParam * pParam;

	i3::vector<i3ShaderParam*>& List = m_ParamListArray[ type];
	
	for(size_t i = 0; i < List.size(); i++)
	{
		pParam = List[i];

		if( i3::generic_is_iequal( pParam->GetNameString(), pszName) )
			return pParam;
	}

	return nullptr;
}

void i3ShaderContext::Bind( i3Shader * pShader)
{
	// Renference Counting 하지 않음.
	m_pShader = pShader;
}

bool i3ShaderContext::Apply( i3RenderContext * pContext)
{
	setModifiedFlag( 0xFFFFFFFF);
	removeState( I3G_SHADER_CTX_STATE_SKINNING);

	return true;
}

void i3ShaderContext::setLight( i3Light * pLight1, i3Light * pLight2)
{
	// Light가 들어가는 순서는
	// 1. Directional Light
	// 2. Point Light
	// 3. Spot Light

	m_LightCount = 0;

	if( pLight1 != nullptr)
	{
		m_pLights[0] = pLight1;
		pLight1->addState( I3G_LIGHT_STATE_APPLIED);
		addModifiedFlag( I3G_SHADER_PMASK_LIGHT0);
		m_LightCount++;
	}

	if( pLight2 != nullptr)
	{
		m_pLights[1] = pLight2;
		pLight2->addState( I3G_LIGHT_STATE_APPLIED);
		addModifiedFlag( I3G_SHADER_PMASK_LIGHT1);
		m_LightCount++;
	}
}


void i3ShaderContext::UpdateParams( I3G_SHADER_TYPE type, i3RenderContext * pCtx, bool bDump)
{

	i3ShaderParam * pParam;
	MATRIX mtx;
	i3Light * pLight;
	UINT32 Modified = getModifiedFlag();
	COLORREAL mtl;

	// Modified = 0xFFFFFFFF;

	if( Modified == 0)
		return;

	i3::vector<i3ShaderParam*>& List = m_ParamListArray[ type];

	for(size_t i = 0; i < List.size(); i++)
	{
		pParam = List[i];

		if( (Modified & pParam->getUpdateMask()) == 0)
			continue;

		//__SP( pParam->getSemantic());

		switch( pParam->getSemantic())
		{
			case i3ShaderParam::SEMANTIC_CONSTANT:			pParam->Update( pCtx);	break;

			case i3ShaderParam::SEMANTIC_MATRIX_WORLD :		pParam->Update( pCtx->GetWorldMatrix());		break;

			case i3ShaderParam::SEMANTIC_MATRIX_BONE:
				pParam->Update( pCtx->GetVertexBlendMatrixArray(), pCtx->getVertexBlendMatrixCount());	
				break;

			case i3ShaderParam::SEMANTIC_MATRIX_VIEW:		pParam->Update( pCtx->GetViewMatrix());		break;

			case i3ShaderParam::SEMANTIC_MATRIX_INV_VIEW :	
				{
					MATRIX tmp;

					i3Matrix::Transpose3x3( &tmp, pCtx->GetViewMatrix());
					pParam->Update( &tmp);
				}
				break;

			case i3ShaderParam::SEMANTIC_MATRIX_INV_WORLD:
				{
					MATRIX tmp;

					i3Matrix::Inverse(&tmp, nullptr, pCtx->GetWorldMatrix());
					pParam->Update( &tmp);
				}
				break;

			case i3ShaderParam::SEMANTIC_MATRIX_WORLDVIEW:
				{
					i3Matrix::Mul( &mtx, pCtx->GetWorldMatrix(), pCtx->GetViewMatrix());
					pParam->Update( &mtx);
				}
				break;
				
			case i3ShaderParam::SEMANTIC_MATRIX_VIEWPROJECT:
				{
					i3Matrix::Mul( &mtx, pCtx->GetViewMatrix(), pCtx->GetProjectMatrix());
					pParam->Update( &mtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_MATRIX_PROJECT:		pParam->Update( pCtx->GetProjectMatrix());			break;

			case i3ShaderParam::SEMANTIC_MATRIX_FULL:
				{
					i3Matrix::Mul( &mtx, pCtx->GetWorldMatrix(), pCtx->GetViewMatrix());
					i3Matrix::Mul( &mtx, &mtx, pCtx->GetProjectMatrix());
					pParam->Update( &mtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_MATRIX_TEX :
				pParam->Update( pCtx->GetTextureMatrix( I3G_TEXTURE_BIND_DIFFUSE));
				break;

			case i3ShaderParam::SEMANTIC_AMBIENT :
				pParam->Update( pCtx->getAmbient());
				break;

			case i3ShaderParam::SEMANTIC_COLOR :
				pParam->Update( pCtx->GetColor());
				break;

			//case i3ShaderParam::SEMANTIC_MATRIX_TEXTURE:		pParam->Update( pCtx->GetTextureMatrix( pParam->getIndex()));		break;

			////////////////////////////////////////////////////////////////////////
			// Light 0
			case i3ShaderParam::SEMANTIC_LIGHT_DIRECTION_0:
				if( m_LightCount > 0)
				{
					pLight = m_pLights[0];

					VEC3D dir;
					i3Vector::TransformNormal( &dir, pLight->GetWorldSpaceDirection(), pCtx->GetViewMatrix());
					i3Vector::Normalize( &dir, &dir);

					//Shader에서 사용하는 변수는 float4인데 넘겨주는데는 float3이라서 뒤에 쓰레기 값이 붙는
					//현상이 발견되어 코드수정
					VEC4D temp;
					temp.x = dir.x;
					temp.y = dir.y;
					temp.z = dir.z;
					temp.w = 0;

					pParam->Update( &temp);
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_POSITION_0:
				if( m_LightCount > 0)
				{
					pLight = m_pLights[0];

					VEC3D pos;
					i3Vector::TransformCoord( &pos, pLight->GetWorldSpacePosition(), pCtx->GetViewMatrix());

					//Shader에서 사용하는 변수는 float4인데 넘겨주는데는 float3이라서 뒤에 쓰레기 값이 붙는
					//현상이 발견되어 코드수정
					VEC4D temp;
					temp.x = pos.x;
					temp.y = pos.y;
					temp.z = pos.z;
					temp.w = 0;

					pParam->Update(&temp);

				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_DIFFUSE_0:	
				if( m_LightCount > 0)
				{
					COLORREAL col; 

					pLight = m_pLights[0];

					COLORREAL * pLitDiff = pLight->GetDiffuse();
					COLORREAL * pMtlDiff = pCtx->getMaterialDiffuse();

					i3Color::Mul( &col, pLitDiff, pMtlDiff);
					col.a = pCtx->getMaterialDiffuse()->a;

					pParam->Update( &col);
				}
				else
				{
					pParam->Update( pCtx->getMaterialDiffuse());
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_SPECULAR_0:	
				if( m_LightCount > 0)
				{
					pLight = m_pLights[0];

					i3Color::Mul( &mtl, pLight->GetSpecular(), pCtx->getMaterialSpecular());
					mtl.a = pCtx->getMaterialShininess();

					pParam->Update( &mtl);
				}
				else
				{
					pParam->Update( pCtx->getMaterialSpecular());
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_MAP_0:
				if( m_LightCount > 0)
				{
					pLight = m_pLights[0];
					pParam->Update( pLight->getPatternMap());
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_FACTOR_0 :
				if( m_LightCount > 0)
				{
					pLight = m_pLights[0];

					VEC4D vec;

					vec.x = 1.0f / (pLight->getRange() * pLight->getRange());
					vec.y = i3Math::cos( pLight->getOuterAngle() * 0.5f);
					vec.z = 1.0f / ( i3Math::cos( pLight->getInnerAngle() * 0.5f) - vec.y);
					vec.w = 0.0f;

					pParam->Update( &vec);
				}
				break;

			case i3ShaderParam::SEMANTIC_SHADOWMAP_0 :
				if( m_LightCount > 0)
				{
					pLight = m_pLights[0];

					I3G_TEXTURE_BIND_CONTEXT * pTexCtx = pCtx->getTextureBindContext( I3G_TEXTURE_BIND_SHADOW);

					pTexCtx->m_pTexture = pLight->getActualShadowMap();

					pParam->Update( pTexCtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_SHADOWMAP_MATRIX_0 :
				if( m_LightCount > 0)
				{
					pLight = m_pLights[0];

					pParam->Update( pLight->getShadowMapMatrix());
				}
				break;

			case i3ShaderParam::SEMANTIC_SHADOWMAP_FACTOR_0 :
				if( m_LightCount > 0)
				{
					pLight = m_pLights[0];

					VEC4D vec;

					vec.x = (REAL32) pLight->getShadowMapSize();
					vec.y = i3Math::Reciprocal( vec.x);
					vec.z = i3Math::cos( pLight->getOuterAngle());
					vec.w = pLight->getLightSpaceZRange();
					pParam->Update( &vec);
				}
				break;


			////////////////////////////////////////////////////////////////////////
			// Light 1
			case i3ShaderParam::SEMANTIC_LIGHT_DIRECTION_1:
				if( m_LightCount > 1)
				{
					pLight = m_pLights[1];

					VEC3D dir;
					i3Vector::TransformNormal( &dir, pLight->GetWorldSpaceDirection(), pCtx->GetViewMatrix());
					i3Vector::Normalize( &dir, &dir);

					//Shader에서 사용하는 변수는 float4인데 넘겨주는데는 float3이라서 뒤에 쓰레기 값이 붙는
					//현상이 발견되어 코드수정
					VEC4D temp;
					temp.x = dir.x;
					temp.y = dir.y;
					temp.z = dir.z;
					temp.w = 0;

					pParam->Update(&temp);
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_POSITION_1:
				if( m_LightCount > 1)
				{
					pLight = m_pLights[1];

					VEC3D pos;
					i3Vector::TransformCoord( &pos, pLight->GetWorldSpacePosition(), pCtx->GetViewMatrix());

					//Shader에서 사용하는 변수는 float4인데 넘겨주는데는 float3이라서 뒤에 쓰레기 값이 붙는
					//현상이 발견되어 코드수정
					VEC4D temp;
					temp.x = pos.x;
					temp.y = pos.y;
					temp.z = pos.z;
					temp.w = 0;

					pParam->Update(&temp);
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_DIFFUSE_1:	
				if( m_LightCount > 1)
				{
					pLight = m_pLights[1];
					i3Color::Mul( &mtl, pLight->GetDiffuse(), pCtx->getMaterialDiffuse());
					mtl.a = pCtx->getMaterialDiffuse()->a;
					pParam->Update( &mtl);
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_SPECULAR_1:	
				if( m_LightCount > 1)
				{
					pLight = m_pLights[1];
					i3Color::Mul( &mtl, pLight->GetSpecular(), pCtx->getMaterialSpecular());
					mtl.a = pCtx->getMaterialShininess();
					pParam->Update( &mtl);
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_MAP_1:
				if( m_LightCount > 1)
				{
					pLight = m_pLights[1];
					pParam->Update( pLight->getPatternMap());
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_FACTOR_1 :
				if( m_LightCount > 1)
				{
					pLight = m_pLights[1];

					VEC4D vec;

					vec.x = 1.0f / (pLight->getRange() * pLight->getRange());
					vec.y = i3Math::cos( pLight->getOuterAngle() * 0.5f);
					vec.z = 1.0f / ( i3Math::cos( pLight->getInnerAngle() * 0.5f) - vec.y);
					vec.w = 0.0f;

					pParam->Update( &vec);
				}
				break;

			case i3ShaderParam::SEMANTIC_SHADOWMAP_1 :
				if( m_LightCount > 1)
				{
					pLight = m_pLights[1];

					I3G_TEXTURE_BIND_CONTEXT * pTexCtx = pCtx->getTextureBindContext( I3G_TEXTURE_BIND_SHADOW);

					pTexCtx->m_pTexture = pLight->getActualShadowMap();

					pParam->Update( pTexCtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_SHADOWMAP_MATRIX_1 :
				if( m_LightCount > 1)
				{
					pLight = m_pLights[1];

					pParam->Update( pLight->getShadowMapMatrix());
				}
				break;

			case i3ShaderParam::SEMANTIC_SHADOWMAP_FACTOR_1 :
				if( m_LightCount > 1)
				{
					pLight = m_pLights[1];

					VEC4D vec;

					vec.x = (REAL32) pLight->getShadowMapSize();
					vec.y = i3Math::Reciprocal( vec.x);
					vec.z = i3Math::cos( pLight->getOuterAngle());
					vec.w = pLight->getLightSpaceZRange();
					pParam->Update( &vec);
				}
				break;

				/////////////////////////////////////
				//////////////
				//////////////
				//////////////

			case i3ShaderParam::SEMANTIC_MATERIAL_EMISSIVE:	
				pParam->Update( pCtx->getMaterialEmissive());	
				break;

			case i3ShaderParam::SEMANTIC_BONE_COUNT:
				{
					i3VertexFormat * pFormat = pCtx->GetVertexArray()->GetFormat();

					if( pFormat->GetBlendIndexCount() > 0)
					{
						pParam->Update( (REAL32) pFormat->GetBlendWeightCount() + 1.0f);
					}
					else						
						pParam->Update( (REAL32) 0.0f);
				}
				break;

			case i3ShaderParam::SEMANTIC_CAMERA_POSITION:
				{
					VEC3D vPos;
					i3Vector::Scale(&vPos, i3Matrix::GetPos(pCtx->GetViewMatrix()), -1.0f);
					pParam->Update(&vPos);
				}
				break;

			case i3ShaderParam::SEMANTIC_CAMERA_DIRECTION:
				pParam->Update(i3Matrix::GetAt(pCtx->GetViewMatrix()));
				break;

			case i3ShaderParam::SEMANTIC_DIFFUSEMAP :
				{
					I3G_TEXTURE_BIND_CONTEXT * pTexCtx = pCtx->getTextureBindContext( I3G_TEXTURE_BIND_DIFFUSE);

					pParam->Update( pTexCtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_NORMALMAP :
				{
					I3G_TEXTURE_BIND_CONTEXT * pTexCtx = pCtx->getTextureBindContext( I3G_TEXTURE_BIND_NORMAL);

					pParam->Update( pTexCtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_SPECULARMAP :
				{
					I3G_TEXTURE_BIND_CONTEXT * pTexCtx = pCtx->getTextureBindContext( I3G_TEXTURE_BIND_SPECULAR);

					pParam->Update( pTexCtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_EMISSIVEMAP :
				{
					I3G_TEXTURE_BIND_CONTEXT * pTexCtx = pCtx->getTextureBindContext( I3G_TEXTURE_BIND_EMISSIVE);

					pParam->Update( pTexCtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_REFLECTMAP :
				{
					I3G_TEXTURE_BIND_CONTEXT * pTexCtx = pCtx->getTextureBindContext( I3G_TEXTURE_BIND_REFLECT);

					pParam->Update( pTexCtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_REFLECTMASKMAP :
				{
					I3G_TEXTURE_BIND_CONTEXT * pTexCtx = pCtx->getTextureBindContext( I3G_TEXTURE_BIND_REFLECT_MASK);

					pParam->Update( pTexCtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_LUXMAP :
				{
					I3G_TEXTURE_BIND_CONTEXT * pTexCtx = pCtx->getTextureBindContext( I3G_TEXTURE_BIND_LUX);

					pParam->Update( pTexCtx);
				}
				break;

			case i3ShaderParam::SEMANTIC_FRESNEL_IOR :
				{
					pParam->Update( pCtx->GetFresnelIndexOfReflect());
				}
				break;

			case i3ShaderParam::SEMANTIC_FRESNEL_COLOR :
				{
					pParam->Update( pCtx->GetFresnelColor());
				}
				break;

			case i3ShaderParam::SEMANTIC_LIGHT_PROBE :
				{
					pParam->Update( pCtx->getLightProbeTexture());
				}
				break;

			case i3ShaderParam::SEMANTIC_SECONDARY_COLOR :
				pParam->Update( pCtx->getSecondaryColor());
				break;

			case i3ShaderParam::SEMANTIC_TETRA_COLOR :
				pParam->Update( pCtx->getTetraColor());
				break;

			case i3ShaderParam::SEMANTIC_TIME :
				pParam->Update( m_tmLocal);

				m_tmLocal += m_timer.GetDeltaSec();
				break;

			case i3ShaderParam::SEMANTIC_SCREENSIZE :
				{
					VEC4D  vec;
					i3Vector::Set(&vec, (REAL32)pCtx->GetCurrentRTWidth(), (REAL32)pCtx->GetCurrentRTHeight(), 1.0f / pCtx->GetCurrentRTWidth(), 1.0f / pCtx->GetCurrentRTHeight());
					pParam->Update(&vec);
				}
				break;

			case i3ShaderParam::SEMANTIC_XRAY_COLOR :
				pParam->Update( pCtx->GetXrayColor() );
				break;
		}

		//__EP( pParam->getSemantic());
	} 
}

void i3ShaderContext::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
}

void i3ShaderContext::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
}

UINT32 i3ShaderContext::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3ShaderContext::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}

