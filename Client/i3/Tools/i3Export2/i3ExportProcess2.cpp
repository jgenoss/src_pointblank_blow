#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"
#include "i3EXPTriObject.h"
#include "i3EXPBitmapStorage.h"
#include "iLayer.h"
#include "max_control.h"

void i3Export::DoExport( char * pszPath, char * pszName)
{
	Interface * ip = GetCOREInterface();
	INode * pRoot;
	INT32 i;
	char szPath[MAX_PATH], szExportName[MAX_PATH];

	ClearLog();

	pRoot = ip->GetRootNode();

	m_TotalNodeCount = 0;
	m_CameraCount = 0;
	m_LightCount = 0;
	m_bExcludeBiped = true;
	I3_SAFE_RELEASE( m_pAnim);

	m_pSkinBoneRoot = NULL;

	{
		Interval intv;

		intv = ip->GetAnimRange();

		m_timeStart = intv.Start();
		m_timeEnd = intv.End();
	}

	Traverse( pRoot, _NodeCountProc, this);

	ip->SetCancel( FALSE);
	ip->ProgressStart( "i3Engine Exporter", TRUE, _nullProc, NULL);


	// 1. 실제 Export될 Node들을 검색해 관련된 Bone Node들의 List를 구성한다.
	// 2. 최상단 Root로 i3Transform Node를 생성하고, Bone List를 이용해 전체 Sg.의 틀을 마련한다.
	//	  이때, Skinning에 이용되는 Bone들은 i3Body Node로 구성하고 제거한다.

	// 실제적인 Exporting 처리
	m_ProcessedNodeCount = 0;
	Traverse( pRoot, _NodeTraverseProc, this);

	LinkNode();

	// Light가 발견되었다면 Lighting을 On 시킨다.
	if((m_pRoot != NULL) && ( m_LightCount > 0))
	{
		i3AttrSet * pAttrSet = i3AttrSet::NewObject();

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::NewObjectRef();

			pAttr->Set( TRUE);
			pAttrSet->AddAttr( pAttr);
		}

		pAttrSet->SetName( m_pRoot->GetName());

		for( i = 0; i < m_pRoot->GetChildCount(); i++)
		{
			i3Node * pChild = m_pRoot->GetChild( i);

			pAttrSet->AddChild( pChild);
		}

		m_pRoot->Release();
		m_pRoot = pAttrSet;
	}

	// i3Skin Node로 생성된 Skinning Mesh라면
	// i3Body Node로 대체해 Export 한다.
	{
		if( i3Scene::FindNodeByExactType( m_pRoot, i3Skin::GetClassMeta()) != NULL)
		{
			i3OptScene2Body opt;

			I3_SG_DUMP( m_pRoot);

			opt.Trace( m_pRoot);
		}
	}


	// 만약 최상위가 쓸모없는 Node인지 확인...
	{
		if((m_pRoot->IsExactTypeOf( i3Node::GetClassMeta())) && (m_pRoot->GetChildCount() == 1))
		{
			i3Node * pChild = m_pRoot->GetChild( 0);

			pChild->AddRef();

			m_pRoot->RemoveChild( pChild);

			I3_REF_CHANGE( m_pRoot, pChild);

			pChild->Release();
		}
	}

	// Animation이 있다면 재생을 설정해준다.
	if( m_pAnim != NULL)
	{
		_PlayAnim();
	}

	// Post-Process
	_DoPostProcess();

	// 3DSMAX Axis -> i3Engine Axis
	if( m_Option.m_bRotateAxis == TRUE)
	{
		i3Transform * pTrans = i3Transform::NewObject();
		MATRIX * pMtx = pTrans->GetMatrix();
		
		i3Matrix::SetRight(	pMtx,	-1.0f,	0.0f,	0.0f,	0.0f);
		i3Matrix::SetUp(	pMtx,	0.0f,	0.0f,	1.0f,	0.0f);
		i3Matrix::SetAt(	pMtx,	0.0f,	1.0f,	0.0f,	0.0f);
		i3Matrix::SetPos(	pMtx,	0.0f,	0.0f,	0.0f,	1.0f);

		pTrans->SetName( "AxisRotate");
		pTrans->AddFlag( I3_NODEFLAG_CONTROL);

		{
			pTrans->AddChild( m_pRoot);
			m_pRoot->Release();

			m_pRoot = pTrans;
		}
	}

	// Bounding Sphere 생성
	{
		i3OptBuildBoundSphere opt;

		opt.Trace( m_pRoot);
	}

	// Vertex Format에 대한 Optimize
	{
		i3OptVertexFormat opt;

		opt.Trace( m_pRoot);
	}

	// Global-Environment-Map의 설정
	{
		if( ip->GetUseEnvironmentMap() && (ip->GetEnvironmentMap() != NULL))
		{
			Texmap * pTex = ip->GetEnvironmentMap();
			bool bError = true;

			if( pTex->ClassID() == Class_ID( BMTEX_CLASS_ID, 0))
			{
				BitmapTex * pBmpTex = (BitmapTex *) pTex;

				// Texture 
				if( pBmpTex->GetBitmap( getStartTime()) != NULL)
				{
					i3Texture * pTex;

					pTex = _CreateTexture( pBmpTex->GetMapName(), pBmpTex->GetBitmap( getStartTime()));

					if( pTex != NULL)
					{
						i3ReflectMapBindAttr * pAttr = i3ReflectMapBindAttr::NewObjectRef();

						pAttr->SetTexture( pTex);

						pTex->Release(); 

						// 
						i3AttrSet * pAttrSet = i3AttrSet::NewObjectRef();

						pAttrSet->AddAttr( pAttr);

						i3Scene::MoveChildren( pAttrSet, m_pRoot);

						m_pRoot->AddChild( pAttrSet);

						bError = false;
					}
				}
			}

			if( bError)
			{
				Log( NULL, "Invalid global environment map \n");
			}
		}
	}

	// 최종적으로 생성된 Scene Graph를 파일에 저장
	if( (ip->GetCancel() == FALSE) && (m_pRoot != NULL))
	{
		sprintf( szPath, "%s\\%s", pszPath, pszName);

		strcpy( szExportName, szPath);

		if( m_Option.m_bExportI3S)
		{
			i3SceneFile file;

			file.SetSceneGraph( m_pRoot);
			if( file.Save( szPath) == STREAM_ERR)
			{
				char conv[256];

				sprintf( conv, "%s 파일을 저장할 수 없습니다. 경로를 확인해 주세요.", szPath);
				Log( NULL, conv);
			}
		}

		// Animation
		if((m_Option.m_bExportAnim) && (m_pAnim != NULL))
		{
			i3String::RemoveExt( szPath);
			i3String::Concat( szPath, ".i3a");

			{
				i3ResourceFile file;

				file.setKeyObject( m_pAnim);

				file.Save( szPath);
			}
		}

		if( m_Option.m_bViewAfterExport)
		{
			ShellExecute( NULL, "open", szExportName, NULL, pszPath, SW_SHOW);
		}
	}

	ip->ProgressUpdate( 100);
	ip->ProgressEnd();


	_ResetExportContext();
}