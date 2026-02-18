#include "stdafx.h"
#include "i3XFileTemplate.h"

#include "i3XFileLexer.h"
#include "i3XFileParser.h"


#include "DXErr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define		NO_IMP
#define		NEEDED_IMPROVE
#define		TEMP_CODE

//#define		FACE_DIV


void	DiffDumpVertexArray(const char * pFileName, i3VertexArray * pVA1, i3VertexArray * pVA2);



I3_ABSTRACT_CLASS_INSTANCE( i3XFileTemplate, i3ElementBase);

I3_CLASS_INSTANCE( i3XFileMesh, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileMeshNormals, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileMeshTextureCoords, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileMaterial, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileMeshMaterialList, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileTextureFilename, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileFrame, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileFrameTransformMatrix, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileXSkinMeshHeader, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileSkinWeights, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileAnimationSet, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileAnimation, i3XFileTemplate);
I3_CLASS_INSTANCE( i3XFileAnimationKey, i3XFileTemplate);

i3XFileTemplate::i3XFileTemplate()
: m_pParser(NULL)
, m_pParent(NULL)
{
	m_Name[0] = NULL;
}


i3XFileTemplate::~i3XFileTemplate()
{
	m_ChildList.SafeReleaseAll();
}

BOOL	i3XFileTemplate::ApplyChild(i3XFileTemplate* pChild)
{
	if (m_pParent)
	{
		return m_pParent->ApplyChild(pChild);
	}
	return TRUE;
}


void	i3XFileTemplate::FirstParse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32	IdenNumber = pLexer->GetToken(TokenBuffer);
	if (XTOKEN_BLOCK_BEGIN == IdenNumber)
	{
		m_Name[0] = NULL;
	}
	else if (XTOKEN_STRING == IdenNumber)
	{
		i3String::Copy(m_Name, TokenBuffer, MAX_PATH );

		// 이름이 있는 경우 블록시작토큰이 제거되지 않았기 때문에 블록시작토큰을 제거한다.
		BOOL	brv = pLexer->FindToken(XTOKEN_BLOCK_BEGIN);
		I3ASSERT(brv == TRUE);
	}
	else
	{
		I3ASSERT_0;
	}


	m_pParser		= pParser;
	m_pParent		= pParent;

	pParser->StartParseTemplate(this);
}


BOOL	i3XFileTemplate::LastParse(i3XFileLexer * pLexer)
{
	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32	IdenNumber = pLexer->GetToken(TokenBuffer);


	// 데이터 마지막 부분에 시작 블록이 먼저 나온다면
	// 데이터 참조인 것이다. (데이터 참조는 문서를 참고하면 된다.)
	if (XTOKEN_BLOCK_BEGIN == IdenNumber)
	{
		RefObjectApply(pLexer);
		IdenNumber = pLexer->GetToken(TokenBuffer);
	}


	while (XTOKEN_BLOCK_END != IdenNumber)
	{
		// 템플릿 식별자가 나왔을 때는 그 템플릿클래스를 생성하여
		// 파싱을 진행한다.
		if (pLexer->IsTemplateIden(IdenNumber))
		{
			if ( IsIgnoreTemplate(IdenNumber) )
			{
				pLexer->FindToken(XTOKEN_BLOCK_BEGIN);
				pLexer->SkipCurrentBlock();
			}
			else
			{
				char TemplateClassName[MAX_PATH] = "i3XFile";
				i3String::Concat(TemplateClassName, TokenBuffer);

				i3ClassMeta * pMeta = i3ClassMeta::FindClassMetaByName(TemplateClassName);
				i3XFileTemplate * pTemplateClass = (i3XFileTemplate*)CREATEINSTANCE(pMeta);

				if ( !pTemplateClass->Parse(pLexer, m_pParser, this) )
				{
					m_pParser->Log(I3GRAP_LOG_ERROR, "파싱을 실패하였습니다.[%s]", TemplateClassName);
					return FALSE;
				}

				pTemplateClass->AddRef();
				m_ChildList.Add(pTemplateClass);
			}
		}
		else if (XTOKEN_BLOCK_BEGIN == IdenNumber)
		{
			if (!RefObjectApply(pLexer))
			{
				m_pParser->Log(I3GRAP_LOG_ERROR, "데이터 참조를 실패하였습니다");
				return FALSE;
			}
		}
		else
		{
			I3ASSERT_0;
		}
		IdenNumber = pLexer->GetToken(TokenBuffer);
	}


	// 자신의 파싱이 끝났으면 부모에게 알려준다.
	if (m_pParent)
	{
		if ( !m_pParent->ApplyChild(this) )
		{
			m_pParser->Log(I3GRAP_LOG_ERROR, "데이터 적용을 실패하였습니다.");
			return FALSE;
		}
	}

	// 파서에게 자신의 파싱이 끝났다고 알려준다.
	m_pParser->EndParseTemplate(this);

	return TRUE;
}



BOOL	i3XFileTemplate::RefObjectApply(i3XFileLexer * pLexer)
{
	// 데이터 참조일 경우 그 데이터를 자신의 클래스에 적용시킨다.
	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32	IdenNumber = pLexer->GetToken(TokenBuffer);
	i3XFileTemplate *	pTemplateClass = NULL;
	while(XTOKEN_BLOCK_END != IdenNumber)
	{
		pTemplateClass = m_pParser->GetDataObjectByName(TokenBuffer);
		I3ASSERT(pTemplateClass != NULL);
		if ( !ApplyChild(pTemplateClass) )
			return FALSE;

		pTemplateClass->AddRef();
		m_ChildList.Add(pTemplateClass);

		IdenNumber = pLexer->GetToken(TokenBuffer);
	}
	return TRUE;
}


//-----------------------------------------------------------------------------
//		i3XFileMesh
//-----------------------------------------------------------------------------
i3XFileMesh::i3XFileMesh()
: i3XFileTemplate()
, m_pAttrSet(NULL)
, m_pVertexPosArr(NULL)
, m_pIndexArr(NULL)
, m_pNormals(NULL)
, m_pTexCoords(NULL)
, m_pXSkinMeshHeader(NULL)
, m_SkinWeightList()
, m_pVertexWeights(NULL)
{
	m_pAttrSet = i3AttrSet::NewObject();
}


i3XFileMesh::~i3XFileMesh()
{
	I3_SAFE_RELEASE(m_pAttrSet);
	I3_SAFE_RELEASE(m_pVertexPosArr);
	I3_SAFE_RELEASE(m_pIndexArr);

	I3_SAFE_RELEASE(m_pNormals);
	I3_SAFE_RELEASE(m_pTexCoords);

	I3_SAFE_RELEASE(m_pXSkinMeshHeader);
	m_SkinWeightList.SafeReleaseAll();
	I3_SAFE_DELETE_ARRAY(m_pVertexWeights);
}


BOOL	i3XFileMesh::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	// 메쉬 템플릿
	//template Mesh
	//{
	//	<3D82AB44-62DA-11CF-AB39-0020AF71E433>
	//	DWORD nVertices;
	//	array Vector vertices[nVertices];
	//	DWORD nFaces;
	//	array MeshFace faces[nFaces];
	//	[...]
	//}

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);

	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32 TokenNumber = XTOKEN_INVALID;

	//
	// 버텍스값을 읽는다.
	//

	UINT32	NumVert = (UINT32)pLexer->GetTokenInt(TokenBuffer);

	m_pVertexPosArr	= i3Vector3Array::NewObject();
	m_pVertexPosArr->SetVectorCount(NumVert);

	REAL32	Value[3];
	for (UINT32 i = 0 ; i < NumVert ; ++i)
	{
		for (INT32 k = 0 ; k < 3 ; ++k)
			Value[k] = pLexer->GetTokenReal(TokenBuffer);
		m_pVertexPosArr->SetVector(i, Value[0], Value[1], Value[2]);
	}


	//
	// 페이스값을 읽는다.
	//
	UINT32	NumFace = (UINT32)pLexer->GetTokenInt(TokenBuffer);

	i3List	IdxList;
	i3List	FaceValues;

	for (UINT32 i = 0 ; i < NumFace ; ++i)
	{
		INT32	NumFaceValue = pLexer->GetTokenInt(TokenBuffer);
		I3ASSERT(NumFaceValue >= 3);

		FaceValues.Clear();
		for (INT32 k = 0 ; k < NumFaceValue ; ++k)
			FaceValues.Add((void*)pLexer->GetTokenInt(TokenBuffer));

		if (NumFaceValue == 3)
		{
			for (INT32 k = 0 ; k < 3 ; ++k)
				IdxList.Add((void*)FaceValues.GetItem(k));
		}
		else
		{
			//
			// 인덱스가 4개 이상이면 페이스를 분할한다.
			//
			INT32	NumDivFace = FaceValues.GetCount() - 2;
			UINT16	StartIndex = (UINT16)FaceValues.GetFirst();
			FaceValues.Delete(0);

			for (INT32 i = 0 ; i < NumDivFace ; ++i)
			{
				IdxList.Add((void*)StartIndex);
				IdxList.Add((void*)FaceValues.GetItem(0));
				IdxList.Add((void*)FaceValues.GetItem(1));

				FaceValues.Delete(0);
			}

			// 위 방식으로 페이스를 분할하면 인덱스는 하나만 남아야 한다.
			I3ASSERT(FaceValues.GetCount() == 1);
		}
	}

	NumFace = IdxList.GetCount();
	m_pIndexArr	= i3IndexArray::NewObject();
	if ( ! m_pIndexArr->Create(IdxList.GetCount(), I3G_USAGE_WRITEONLY) )
	{
		m_pParser->Log(I3GRAP_LOG_ERROR, "Index Array 생성을 패하였습니다.");
		return FALSE;
	}

	m_pIndexArr->Lock();
	{
		for (UINT32 k = 0 ; k < NumFace ; ++k)
			m_pIndexArr->SetIndex( k, (UINT16)IdxList.GetItem(k));
	}
	m_pIndexArr->Unlock();

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileMesh::ApplyChild(i3XFileTemplate* pChild)
{
	if (pChild->IsExactTypeOf(i3XFileMeshNormals::GetClassMeta()))
	{
		m_pNormals = (i3XFileMeshNormals*)pChild;
		m_pNormals->AddRef();
	}
	else if (pChild->IsExactTypeOf(i3XFileMeshTextureCoords::GetClassMeta()))
	{
		m_pTexCoords = (i3XFileMeshTextureCoords*)pChild;
		m_pTexCoords->AddRef();
	}
	else if (pChild->IsExactTypeOf(i3XFileXSkinMeshHeader::GetClassMeta()))
	{
		m_pXSkinMeshHeader = (i3XFileXSkinMeshHeader*)pChild;
		m_pXSkinMeshHeader->AddRef();
	}
	else if (pChild->IsExactTypeOf(i3XFileSkinWeights::GetClassMeta()))
	{
		m_SkinWeightList.Add(pChild);
		I3ASSERT((UINT32)m_SkinWeightList.GetCount() <= m_pXSkinMeshHeader->GetNumBones());
		pChild->AddRef();
	}
	else if (pChild->IsExactTypeOf(i3XFileMaterial::GetClassMeta()))
	{
		i3XFileMaterial *	pMaterial = (i3XFileMaterial*)pChild;
		m_pAttrSet->AddAttr(pMaterial->GetMaterialAttr());

		// 텍스쳐를 포함하고 있으면 적용
		i3XFileTextureFilename *	pTexture = pMaterial->GetTextureFilename();
		if (pTexture && pTexture->GetTextureBindAttr())
			m_pAttrSet->AddAttr(pTexture->GetTextureBindAttr());
	}



	return i3XFileTemplate::ApplyChild(pChild);
}


BOOL	i3XFileMesh::SkinWeightsToVectexWeightsArray()
{
	if (!m_pXSkinMeshHeader)
		return TRUE;

	UINT32 NumVertex = m_pVertexPosArr->GetVectorCount();
	m_pVertexWeights = new VertexWeight[NumVertex];
	i3mem::FillZero(m_pVertexWeights, sizeof(VertexWeight) * NumVertex);


	UINT32	NumBones = m_pXSkinMeshHeader->GetNumBones();
	for (UINT32 i = 0 ; i < NumBones ; ++i)
	{
		i3XFileSkinWeights *	pSkinWeight	= GetSkinWeight(i);
		for (UINT32 k = 0 ; k < pSkinWeight->GetNumWeight() ; ++k)
		{
			VertexWeight* pVW = &m_pVertexWeights[pSkinWeight->GetVertexIndex(k)];

			pVW->Indices[pVW->NumUsedIndex] = (UINT8)pSkinWeight->GetFrameIndex();
			pVW->Weight[pVW->NumUsedWeight] = pSkinWeight->GetWeight(k);

			++pVW->NumUsedIndex;
			++pVW->NumUsedWeight;
		}
	}

	return TRUE;
}


BOOL	i3XFileMesh::Build()
{
	I3ASSERT(m_pVertexPosArr != NULL);

	i3Vector3Array *	pNormals	= NULL;
	i3Vector2Array *	pTexCoords	= NULL;

	if (m_pNormals)
		pNormals = m_pNormals->GetVertexNormalArr();

	if (m_pTexCoords)
		pTexCoords = m_pTexCoords->GetVertexTexArr();

	//
	// 노말이 없으면 만들어준다.
	//
	if (pNormals == NULL)
	{
		pNormals = ComputeNormal();
		if (pNormals == NULL)
		{
			m_pParser->Log(I3GRAP_LOG_WARNING, "노말이 정보가 없는 메쉬입니다.\n");
		}
	}

	SkinWeightsToVectexWeightsArray();

	//
	// 읽은 정보들로 버텍스 포맷은 판별한다.
	//
	i3VertexFormat * pVF = i3VertexFormat::NewObject();

	pVF->SetHasPosition();
	if (pNormals != NULL)
		pVF->SetHasNormal();
	if (m_pTexCoords != NULL)
		pVF->SetTextureCoordSetCount(1);
	if (m_pXSkinMeshHeader)
	{
		I3ASSERT( m_pXSkinMeshHeader->GetNumBones() > 0 );

		INT8 BWCount = (INT8)m_pXSkinMeshHeader->GetMaxSkinWeightsPerVertex();
		INT8 BICount = (INT8)m_pXSkinMeshHeader->GetMaxSkinWeightsPerVertex();
		pVF->SetBlendWeightCount(BWCount);
		pVF->SetBlendIndexCount(BICount);
	}

	//
	// 버텍스 버퍼 생성하여 값을 입력한다
	//
	UINT32	NumVert = m_pVertexPosArr->GetVectorCount();
	I3ASSERT(NumVert > 0);
	i3VertexArray * pVA = i3VertexArray::NewObjectRef();

	if ( ! pVA->Create(pVF, NumVert, I3G_USAGE_WRITEONLY) )
	{
		m_pParser->Log(I3GRAP_LOG_ERROR, "버텍스 버퍼 생성 실패\n");
		return FALSE;
	}

	// 버텍스 값 입력
	pVA->Lock();
	{
		// 버텍스 버퍼를 0으로 초기화 한다.
		INT8 * pBuffer = pVA->GetLockedBuffer();
		i3mem::FillZero(pBuffer, pVA->GetStride() * NumVert);

		// 위치
		pVA->SetPositions(0, NumVert, m_pVertexPosArr->GetVectorArray());

		// 노말
		if (pVF->GetHasNormal())
			pVA->SetNormals(0, NumVert, pNormals->GetVectorArray());

		// 텍스쳐 좌표
		INT8 setCount = pVF->GetTextureCoordSetCount();
		if (setCount >= 1)
			pVA->SetTextureCoords(0, 0, NumVert, pTexCoords->GetVectorArray());


		// 가중치와 본 인덱스
		if (m_pXSkinMeshHeader)
		{
			INT32	NumWeightCount = (INT32)m_pXSkinMeshHeader->GetMaxSkinWeightsPerVertex();
			for (UINT32 i = 0 ; i < NumVert ; ++i)
			{
				VertexWeight*	pVW = &m_pVertexWeights[i];
				for (INT32 k = 0 ; k < NumWeightCount ; ++k)
					pVA->SetBlendIndex(k, i, pVW->Indices[k]);

				for (INT32 k = 0 ; k < NumWeightCount ; ++k)
					pVA->SetBlendWeight(k, i, pVW->Weight[k]);
			}
		}
	}
	pVA->Unlock();

	//
	// 만들어진 정보로 노드를 만든다.
	//
	UINT32 primCount = m_pIndexArr->GetCount();

	i3GeometryAttr * pGeoAttr = i3GeometryAttr::NewObjectRef();

#define		VA_OPT
//#define		VA_DIFF_CHECK

#ifdef VA_DIFF_CHECK
	i3VertexArray *	pCloneVertArray = i3VertexArray::NewObject();
	pCloneVertArray->Create(pVF, NumVert, I3G_USAGE_WRITEONLY);
	pVA->CopyTo(pCloneVertArray, I3_COPY_INSTANCE);
#endif

#ifdef VA_OPT
	pGeoAttr->SetIndexArray(m_pIndexArr);
	pGeoAttr->SetPrimitiveType(I3G_PRIM_TRILIST);
	pGeoAttr->SetPrimitiveCount(primCount);
	pGeoAttr->SetVertexArray(pVA, true);
#else
	pGeoAttr->SetIndexArray(m_pIndexArr);
	pGeoAttr->SetPrimitiveType(I3G_PRIM_TRILIST);
	pGeoAttr->SetPrimitiveCount(primCount);
	pGeoAttr->SetVertexArray(pVA, I3G_PRIM_TRILIST, primCount, 0, false);
#endif

#ifdef VA_DIFF_CHECK
	DiffDumpVertexArray(pVA, pCloneVertArray);
#endif


	i3Geometry * pGeoNode = i3Geometry::NewObjectRef();
	pGeoNode->SetName(m_Name);
	pGeoNode->AppendGeometryAttr(pGeoAttr);

	m_pAttrSet->AddChild(pGeoNode);

	pVF->Release();

	return TRUE;
}



i3Vector3Array *	i3XFileMesh::ComputeNormal()
{
	NEEDED_IMPROVE;


	ID3DXMesh* pDXMesh;
	HRESULT	hr;
	LPD3DXBUFFER pAdjacency;
	LPD3DXBUFFER pMtrls;
	LPD3DXBUFFER pEffects;
	DWORD	NumMtrl = 0;
	if ( FAILED(hr = D3DXLoadMeshFromX(m_pParser->GetFullPath(), D3DXMESH_32BIT|D3DXMESH_SYSTEMMEM, g_pD3DDevice,
		&pAdjacency, &pMtrls, &pEffects, &NumMtrl, &pDXMesh)) )
	{
		const char* str = DXGetErrorString(hr);
		I3TRACE1("노말을 계산하기 위해서 파일을 사용할 수 없습니다.[error code :%s]\n", str);
		return NULL;
	}

	ID3DXMesh* pCloneMesh;
	DWORD	FVF = pDXMesh->GetFVF() | D3DFVF_NORMAL;
	if ( FAILED ( hr = pDXMesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, FVF, g_pD3DDevice, &pCloneMesh)) )
	{
		const char* str = DXGetErrorString(hr);
		I3TRACE1("노말을 계산하기 위해서 메쉬를 복제할 수 없습니다.[error code :%s]\n", str);
		return NULL;
	}

	pDXMesh->Release();
	pDXMesh = pCloneMesh;
	pCloneMesh = NULL;


	// 노말 계산을 수행한다.
	if ( FAILED(hr = D3DXComputeNormals(pDXMesh, NULL)) )
	{
		const char* str = DXGetErrorString(hr);
		I3TRACE1("노말을 계산을 실패하였습니다.[error code :%s]\n", str);
		return NULL;
	}


	D3DVERTEXELEMENT9	Decl[MAX_FVF_DECL_SIZE];
	D3DVERTEXELEMENT9	DeclEnd = {0xFF,0,D3DDECLTYPE_UNUSED, 0,0,0};
	pDXMesh->GetDeclaration(Decl);

	// 버텍스 버퍼에서 노말의 오프셋을 얻어낸다.
	DWORD NormalOffset = 0xff;
	for (INT32 i = 0 ;
		Decl[i].Stream != DeclEnd.Stream && Decl[i].Type != DeclEnd.Type ;
		++i)
	{
		if (Decl[i].Usage != D3DDECLUSAGE_NORMAL)
			continue;

		NormalOffset = Decl[i].Offset;
		break;
	}

	I3ASSERT(NormalOffset != 0xff);


	i3Vector3Array * pVertexNormalArr = i3Vector3Array::NewObject();
	pVertexNormalArr->SetVectorCount(m_pVertexPosArr->GetVectorCount());

	BYTE* pVertices = NULL;
	DWORD NumVert = pDXMesh->GetNumVertices();
	DWORD VertBytes = pDXMesh->GetNumBytesPerVertex();
	VEC3D* pNormal;
	pDXMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVertices);
	{
		for (DWORD i = 0 ; i < NumVert ; ++i)
		{
			pNormal = (VEC3D*)(pVertices + (i * VertBytes) + NormalOffset);
			pVertexNormalArr->SetVector((INT32)i, pNormal);
		}
	}
	pDXMesh->UnlockVertexBuffer();

	return pVertexNormalArr;
}


//-----------------------------------------------------------------------------
//		i3XFileMeshNormals
//-----------------------------------------------------------------------------
i3XFileMeshNormals::i3XFileMeshNormals()
: i3XFileTemplate()
, m_pVertexNormalArr(NULL)
{
}


i3XFileMeshNormals::~i3XFileMeshNormals()
{
	I3_SAFE_RELEASE(m_pVertexNormalArr);
}

BOOL	i3XFileMeshNormals::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template MeshNormals
	//{
	//	< F6F23F43-7686-11cf-8F52-0040333594A3 >
	//	DWORD nNormals;
	//	array Vector normals[nNormals];
	//	DWORD nFaceNormals;
	//	array MeshFace faceNormals[nFaceNormals];
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);


	char	TokenBuffer[TOKEN_BUF_SIZE];

	I3ASSERT(pParent->IsExactTypeOf(i3XFileMesh::GetClassMeta()));
	i3XFileMesh *	pParentMesh = (i3XFileMesh*)pParent;

	//
	// Vertex 노말을 읽는다.
	//
	UINT32	NumNormal = (UINT32)pLexer->GetTokenInt(TokenBuffer);

	i3Vector3Array *	pV3NormalArr	= i3Vector3Array::NewObject();
	pV3NormalArr->SetVectorCount(NumNormal);

	VEC3D	V;
	for (UINT32 i = 0 ; i < NumNormal ; ++i)
	{
		for (INT32 k = 0 ; k < 3 ; ++k)
			V.vec[k] = pLexer->GetTokenReal(TokenBuffer);
		pV3NormalArr->SetVector(i, &V);
	}

	//
	// 노말 페이스 인덱스를 읽는다.
	//
	UINT32	NumNormalFace = (UINT32)pLexer->GetTokenInt(TokenBuffer);

	i3List	IdxList;
	i3List	FaceValues;

	for (UINT32 i = 0 ; i < NumNormalFace ; ++i)
	{
		INT32	NumFaceValue = pLexer->GetTokenInt(TokenBuffer);
		I3ASSERT(NumFaceValue >= 3);

		FaceValues.Clear();
		for (INT32 k = 0 ; k < NumFaceValue ; ++k)
			FaceValues.Add((void*)pLexer->GetTokenInt(TokenBuffer));

		if (NumFaceValue == 3)
		{
			for (INT32 k = 0 ; k < 3 ; ++k)
				IdxList.Add((void*)FaceValues.GetItem(k));
		}
		else
		{
			//
			// 인덱스가 4개 이상이면 페이스를 분할한다.
			//
			INT32	NumDivFace = FaceValues.GetCount() - 2;
			UINT16	StartIndex = (UINT16)FaceValues.GetFirst();
			FaceValues.Delete(0);

			for (INT32 i = 0 ; i < NumDivFace ; ++i)
			{
				IdxList.Add((void*)StartIndex);
				IdxList.Add((void*)FaceValues.GetItem(0));
				IdxList.Add((void*)FaceValues.GetItem(1));

				FaceValues.Delete(0);
			}

			// 위 방식으로 페이스를 분할하면 인덱스는 하나만 남아야 한다.
			I3ASSERT(FaceValues.GetCount() == 1);
		}
	}

	NumNormalFace = IdxList.GetCount();
	I3ASSERT(NumNormalFace == pParentMesh->GetIndexArr()->GetCount());

	i3IndexArray *	pNormalIdxArr = i3IndexArray::NewObject();
	if (!pNormalIdxArr->Create(NumNormalFace, I3G_USAGE_WRITEONLY))
	{
		m_pParser->Log(I3GRAP_LOG_ERROR, "Normal Index Array 생성 실패");
		return FALSE;
	}

	pNormalIdxArr->Lock();
	{
		for (UINT32 k = 0 ; k < NumNormalFace ; ++k)
			pNormalIdxArr->SetIndex(k, (UINT16)IdxList.GetItem(k));
	}
	pNormalIdxArr->Unlock();


	//
	// 노말 정보를 채운다
	//
	UINT32	NumVert = pParentMesh->GetVertexePosArr()->GetVectorCount();
	m_pVertexNormalArr	= i3Vector3Array::NewObject();	// 최종 버텍스
	m_pVertexNormalArr->SetVectorCount(NumVert);

	pNormalIdxArr->Lock();
	{
		i3IndexArray *	pFaceArr = pParentMesh->GetIndexArr();

		INT32	NumFace = pFaceArr->GetCount();
		INT32	VertIdx = 0, NormalIdx = 0;
		pFaceArr->Lock();
		for (INT32 i = 0 ; i < NumFace ; ++i)
		{
			VertIdx		= (INT32)pFaceArr->GetIndex(i);
			NormalIdx	= (INT32)pNormalIdxArr->GetIndex(i);

			m_pVertexNormalArr->SetVector(VertIdx, pV3NormalArr->GetVector(NormalIdx));
		}
		pFaceArr->Unlock();
	}
	pNormalIdxArr->Unlock();


	pV3NormalArr->Release();
	pNormalIdxArr->Release();

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileMeshNormals::ApplyChild(i3XFileTemplate* pChild)
{
	I3ASSERT_0;
	return TRUE;
}



//-----------------------------------------------------------------------------
//		i3XFileMeshTextureCoords
//-----------------------------------------------------------------------------
i3XFileMeshTextureCoords::i3XFileMeshTextureCoords()
: i3XFileTemplate()
, m_pVertexTexArr(NULL)
{
}


i3XFileMeshTextureCoords::~i3XFileMeshTextureCoords()
{
	I3_SAFE_RELEASE(m_pVertexTexArr);
}


BOOL	i3XFileMeshTextureCoords::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template MeshTextureCoords
	//{
	//	< F6F23F40-7686-11cf-8F52-0040333594A3 >
	//	DWORD nTextureCoords;
	//	array Coords2d textureCoords[nTextureCoords] ;
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);

	char	TokenBuffer[TOKEN_BUF_SIZE];

	UINT32	NumTexCoord;
	m_pVertexTexArr = i3Vector2Array::NewObject();

	NumTexCoord = pLexer->GetTokenInt(TokenBuffer);
	m_pVertexTexArr->SetVectorCount(NumTexCoord);

	REAL32	u, v;
	for (UINT32 i = 0 ; i < NumTexCoord ; ++i)
	{
		u = pLexer->GetTokenReal(TokenBuffer);
		v = pLexer->GetTokenReal(TokenBuffer);

		m_pVertexTexArr->SetVector(i, u, v);
	}

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileMeshTextureCoords::ApplyChild(i3XFileTemplate* pChild)
{
	I3ASSERT_0;
	return TRUE;
}



//-----------------------------------------------------------------------------
//		i3XFileMaterial
//-----------------------------------------------------------------------------
i3XFileMaterial::i3XFileMaterial()
: i3XFileTemplate()
, m_pMtrlAttr(NULL)
, m_pTextureFileName(NULL)
{
}


i3XFileMaterial::~i3XFileMaterial()
{
	I3_SAFE_RELEASE(m_pMtrlAttr);
	I3_SAFE_RELEASE(m_pTextureFileName);
}


BOOL	i3XFileMaterial::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template Material
	//{
	//	< 3D82AB4D-62DA-11CF-AB39-0020AF71E433 >
	//	ColorRGBA faceColor;
	//	FLOAT power;
	//	ColorRGB specularColor;
	//	ColorRGB emissiveColor;
	//	[...]
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);


	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32	TokenNumber = XTOKEN_INVALID;


	m_pMtrlAttr = i3MaterialAttr::NewObject();

	COLORREAL	color;
	REAL32		power;

	// face color
	color.r = pLexer->GetTokenReal(TokenBuffer);
	color.g = pLexer->GetTokenReal(TokenBuffer);
	color.b = pLexer->GetTokenReal(TokenBuffer);
	color.a = pLexer->GetTokenReal(TokenBuffer);

	m_pMtrlAttr->SetDiffuse(&color);

	// power
	power = pLexer->GetTokenReal(TokenBuffer);

	m_pMtrlAttr->SetShininess(power);

	// specular
	color.r = pLexer->GetTokenReal(TokenBuffer);
	color.g = pLexer->GetTokenReal(TokenBuffer);
	color.b = pLexer->GetTokenReal(TokenBuffer);
	color.a = 1;

	m_pMtrlAttr->SetSpecular(&color);

	// emissive
	color.r = pLexer->GetTokenReal(TokenBuffer);
	color.g = pLexer->GetTokenReal(TokenBuffer);
	color.b = pLexer->GetTokenReal(TokenBuffer);
	color.a = 1;

	m_pMtrlAttr->SetEmissive(&color);

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileMaterial::ApplyChild(i3XFileTemplate* pChild)
{
	if (pChild->IsExactTypeOf(i3XFileTextureFilename::GetClassMeta()))
	{
		// i3XFileTextureFilename의 ApplyChild는
		// 자신(i3XFileMaterial)의 부모에게 알려주는 것 막는다.
		// Material이 전역 인스턴스로 있을때 Mesh가
		// TextureFilename의 ApplyChild를 받을 수 없기 때문이다.
		pChild->AddRef();
		m_pTextureFileName = (i3XFileTextureFilename*)pChild;
		return TRUE;
	}
	return i3XFileTemplate::ApplyChild(pChild);
}



//-----------------------------------------------------------------------------
//		i3XFileMeshMaterialList
//-----------------------------------------------------------------------------
i3XFileMeshMaterialList::i3XFileMeshMaterialList()
: i3XFileTemplate()
{
}


i3XFileMeshMaterialList::~i3XFileMeshMaterialList()
{
}


BOOL	i3XFileMeshMaterialList::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template MeshMaterialList
	//{
	//	< F6F23F42-7686-11CF-8F52-0040333594A3 >
	//	DWORD nMaterials;
	//	DWORD nFaceIndexes;
	//	array DWORD faceIndexes[nFaceIndexes];
	//	[Material <3D82AB4D-62DA-11CF-AB39-0020AF71E433>]
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);


	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32 TokenNumber = XTOKEN_INVALID;


	UINT32		NumMtrl			= (UINT32)pLexer->GetTokenInt(TokenBuffer);
	UINT32		NumFaceIndices	= (UINT32)pLexer->GetTokenInt(TokenBuffer);
	UINT32 *	pFaceIndices	= new UINT32[NumFaceIndices];


	for (UINT32 i = 0 ; i < NumFaceIndices ; ++i)
		pFaceIndices[i] = (UINT32)pLexer->GetTokenInt(TokenBuffer);

	// 매트리얼이 2개 이상이면 메쉬를 분할할 필요가 있는지 검사한다.
	if ( NumMtrl >= 2 )
	{
		i3List	IndexList;	// 
		for (UINT32 i = 0 ; i < NumFaceIndices ; ++i)
		{
			if ( -1 == IndexList.Find((void*)pFaceIndices[i]) )
				IndexList.Add((void*)pFaceIndices[i]);
		}

		// 페이스 인덱스가 2개 이상이면 메쉬를 분할할 필요가 있는것이다.
		if ( IndexList.GetCount() >= 2 )
		{
			// 어떤방식으로 할지 감이 잡히지 않아서 보류.
			NO_IMP;
			m_pParser->Log(I3GRAP_LOG_WARNING,
				"복수개의 매트리얼을 가지고 있는 메쉬입니다."\
				"메쉬 분할은 미구현 되어있으므로 정상적으로 보이지 않을 수 있습니다.");
		}
	}

	delete[] pFaceIndices;


	return i3XFileTemplate::LastParse(pLexer);
}



//-----------------------------------------------------------------------------
//		i3XFileTextureFilename
//-----------------------------------------------------------------------------
i3XFileTextureFilename::i3XFileTextureFilename()
: i3XFileTemplate()
, m_pTexBindAttr(NULL)
{
	m_FullPathName[0] = NULL;
}


i3XFileTextureFilename::~i3XFileTextureFilename()
{
	I3_SAFE_RELEASE(m_pTexBindAttr);
}




BOOL	i3XFileTextureFilename::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template TextureFilename 
	//{ 
	//	< A42790E1-7810-11cf-8F52-0040333594A3 > 
	//	string filename; 
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);


	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32	TokenNumber;


	char	TextureFileName[MAX_PATH];
	TokenNumber = pLexer->GetToken(TokenBuffer);
	I3ASSERT(TokenNumber == XTOKEN_STRING);
	i3String::Copy(TextureFileName, &TokenBuffer[0], MAX_PATH );

	i3String::Copy(m_FullPathName, m_pParser->GetFilePath(), MAX_PATH );
	i3String::Concat(m_FullPathName, "\\");
	i3String::Concat(m_FullPathName, TextureFileName);

	i3String::NormalizePath(m_FullPathName);


	


	NEEDED_IMPROVE;
	//
	// .dds 로드시에 이미지가 깨지는 문제점 때문에
	// #if, #endif로 구분하였음.
	//
//#define	I3_TEXTURE_LOAD
#ifdef I3_TEXTURE_LOAD
	i3ImageFile	file;
	i3Texture*	pTexture = pTexture = file.Load(m_FullPathName);
	if ( pTexture != NULL )
	{
		m_pTexBindAttr = i3TextureBindAttr::NewObject();
		m_pTexBindAttr->SetTexture(pTexture);
	}
	else
	{
		// 텍스쳐 로드 실패는 경로상에 문제 일 수도 있도
		// 크게 문제가 되지 않으므로 로그만 남기고 실패처리하지 않는다.
		m_pParser->Log(I3GRAP_LOG_ERROR, "텍스쳐 로드 실패[%s]\n", TextureFileName);
	}

#else
	i3TextureDX*	pTexture = i3TextureDX::NewObject();
	IDirect3DTexture9* pTexture__ = NULL;
	HRESULT hr;
	if ( SUCCEEDED(hr = D3DXCreateTextureFromFile(g_pD3DDevice, m_FullPathName, &pTexture__)) )
	{
		pTexture->SetDXTexture(pTexture__);
		m_pTexBindAttr = i3TextureBindAttr::NewObject();
		m_pTexBindAttr->SetTexture(pTexture);
	}
	else
	{
		const char* str = DXGetErrorString(hr);
		m_pParser->Log(I3GRAP_LOG_WARNING, "텍스쳐 로드를 실패[%s][hr:%s].", TextureFileName, str);
	}
#endif

	pTexture->Release();

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileTextureFilename::ApplyChild(i3XFileTemplate* pChild)
{
	I3ASSERT_0;
	return TRUE;
}




//-----------------------------------------------------------------------------
//		i3XFileFrame
//-----------------------------------------------------------------------------
i3XFileFrame::i3XFileFrame()
: i3XFileTemplate()
, m_pBone(NULL)
{
}


i3XFileFrame::~i3XFileFrame()
{
	I3_SAFE_DELETE(m_pBone);
}


BOOL	i3XFileFrame::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template Frame
	//{
	//	< 3D82AB46-62DA-11CF-AB39-0020AF71E433 >
	//	[...]			
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);


	m_pBone = new i3Bone;
	i3mem::FillZero(m_pBone, sizeof(i3Bone));
	m_pBone->setName(m_Name);

	
	if (pParent)
	{
		I3ASSERT(pParent->IsExactTypeOf(i3XFileFrame::GetClassMeta()));
		i3XFileFrame *	pParentFrame = (i3XFileFrame *)pParent;
		UINT32	Depth = pParentFrame->GetBone()->getDepth() + 1;
		m_pBone->setDepth(Depth);
	}
	else
	{
		m_pBone->setDepth(0);
	}

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileFrame::ApplyChild(i3XFileTemplate* pChild)
{
	if (pChild->IsExactTypeOf(i3XFileFrameTransformMatrix::GetClassMeta()))
	{
		i3XFileFrameTransformMatrix *	pFrameTransform = (i3XFileFrameTransformMatrix *)pChild;
		m_pBone->setMatrix(pFrameTransform->GetMatrix());
	}
	return TRUE;
}



//-----------------------------------------------------------------------------
//		i3XFileFrameTransformMatrix
//-----------------------------------------------------------------------------
i3XFileFrameTransformMatrix::i3XFileFrameTransformMatrix()
: i3XFileTemplate()
{
	i3Matrix::Identity(&m_Mat);
}


i3XFileFrameTransformMatrix::~i3XFileFrameTransformMatrix()
{
}


BOOL	i3XFileFrameTransformMatrix::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template FrameTransformMatrix
	//{
	//	< F6F23F41-7686-11cf-8F52-0040333594A3 >
	//	Matrix4x4 frameMatrix;
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);

	char	TokenBuffer[TOKEN_BUF_SIZE];
	for (INT32 i = 0 ; i < 16 ; ++ i)
		m_Mat.v[i] = pLexer->GetTokenReal(TokenBuffer);

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileFrameTransformMatrix::ApplyChild(i3XFileTemplate* pChild)
{
	I3ASSERT_0;
	return TRUE;
}



//-----------------------------------------------------------------------------
//		i3XFileXSkinMeshHeader
//-----------------------------------------------------------------------------
i3XFileXSkinMeshHeader::i3XFileXSkinMeshHeader()
: i3XFileTemplate()
, m_MaxSkinWeightsPerVertex(0)
, m_MaxSkinWeightsPerFace(0)
, m_NumBones(0)
{
}


i3XFileXSkinMeshHeader::~i3XFileXSkinMeshHeader()
{
}


BOOL	i3XFileXSkinMeshHeader::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template XSkinMeshHeader 
	//{ 
	//	< 3CF169CE-FF7C-44ab-93C0-F78F62D172E2 >  
	//	WORD nMaxSkinWeightsPerVertex; 
	//	WORD nMaxSkinWeightsPerFace; 
	//	WORD nBones; 
	//}

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);

	char	TokenBuffer[TOKEN_BUF_SIZE];

	m_MaxSkinWeightsPerVertex	= (UINT32)pLexer->GetTokenInt(TokenBuffer);
	m_MaxSkinWeightsPerFace		= (UINT32)pLexer->GetTokenInt(TokenBuffer);
	m_NumBones					= (UINT32)pLexer->GetTokenInt(TokenBuffer);

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileXSkinMeshHeader::ApplyChild(i3XFileTemplate* pChild)
{
	I3ASSERT_0;
	return TRUE;
}



//-----------------------------------------------------------------------------
//		i3XFileSkinWeights
//-----------------------------------------------------------------------------
i3XFileSkinWeights::i3XFileSkinWeights()
: i3XFileTemplate()
, m_FrameIndex(0)
, m_NumWeight(0)
, m_pVertexIndices(NULL)
, m_pWeights(NULL)
{
	m_TransNodeName[0] = NULL;
	i3Matrix::Identity(&m_Offset);
}


i3XFileSkinWeights::~i3XFileSkinWeights()
{
	I3_SAFE_DELETE_ARRAY(m_pVertexIndices);
	I3_SAFE_DELETE_ARRAY(m_pWeights);
}


BOOL	i3XFileSkinWeights::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template SkinWeights 
	//{ 
	//	< 6F0D123B-BAD2-4167-A0D0-80224F25FABB > 
	//	STRING transformNodeName; 
	//	DWORD nWeights; 
	//	array DWORD vertexIndices[nWeights]; 
	//	array float weights[nWeights]; 
	//	Matrix4x4 matrixOffset; 
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);

	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32	TokenNumber = pLexer->GetToken(TokenBuffer);

	I3ASSERT(TokenNumber == XTOKEN_STRING);
	i3String::Copy(m_TransNodeName, TokenBuffer, MAX_PATH );

	m_NumWeight	= pLexer->GetTokenInt(TokenBuffer);
	I3ASSERT(m_NumWeight > 0);

	m_pVertexIndices	= new UINT32[m_NumWeight];
	m_pWeights			= new REAL32[m_NumWeight];

	for (UINT32 i = 0 ; i < m_NumWeight ; ++i)
		m_pVertexIndices[i] = (UINT32)pLexer->GetTokenInt(TokenBuffer);
	for (UINT32 i = 0 ; i < m_NumWeight ; ++i)
		m_pWeights[i] = pLexer->GetTokenReal(TokenBuffer);

	for (INT32 i = 0 ; i < 16 ; ++i)
		m_Offset.v[i] = pLexer->GetTokenReal(TokenBuffer);

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileSkinWeights::ApplyChild(i3XFileTemplate* pChild)
{
	I3ASSERT_0;
	return TRUE;
}



//-----------------------------------------------------------------------------
//		i3XFileAnimationSet
//-----------------------------------------------------------------------------
i3XFileAnimationSet::i3XFileAnimationSet()
: i3XFileTemplate()
, m_pAnimation(NULL)
{
}


i3XFileAnimationSet::~i3XFileAnimationSet()
{
	m_AnimationTrackList.SafeReleaseAll();
	I3_SAFE_RELEASE(m_pAnimation);
}


BOOL	i3XFileAnimationSet::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template AnimationSet
	//{
	//	< 3D82AB50-62DA-11cf-AB39-0020AF71E433 >
	//	[ Animation < 3D82AB4F-62DA-11cf-AB39-0020AF71E433 > ]
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);

	// 필요한 작업이 없음

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileAnimationSet::ApplyChild(i3XFileTemplate* pChild)
{
	if (pChild->IsExactTypeOf(i3XFileAnimation::GetClassMeta()))
	{
		pChild->AddRef();
		m_AnimationTrackList.Add(pChild);
	}
	return i3XFileTemplate::ApplyChild(pChild);
}


BOOL	i3XFileAnimationSet::Build(i3StringList * pBoneNameStringList)
{
	i3XFileAnimation *	pAnimationTrack;
	INT32 NumTrack = m_AnimationTrackList.GetCount();

	//
	// 트랙들을 빌드한다.
	//
	for (INT32 i = 0 ; i < NumTrack ; ++i)
	{
		pAnimationTrack = (i3XFileAnimation*)m_AnimationTrackList.GetItem(i);
		if (!pAnimationTrack->Build())
		{
			m_pParser->Log(I3GRAP_LOG_ERROR, "Animation Build 실패[%d]", i);
			return FALSE;
		}
	}

	//
	// 본 순서에 맞게 트랙들을 설정해준다.
	//
	INT32 NumBone = pBoneNameStringList->GetCount();
	m_pAnimation = i3Animation::NewObject();
	m_pAnimation->Create(NumBone);

	for (INT32 i = 0 ; i < NumBone ; ++i)
	{
		char * pName = (char *)pBoneNameStringList->GetItem(i);
		pAnimationTrack = GetAnimTrackByName(pName);
		if (pAnimationTrack != NULL)
		{
			m_pAnimation->SetTrack(i, pAnimationTrack->GetTransformSeq(),
						pAnimationTrack->GetRefFrameName());
		}
		else
		{
			// 본에 해당하는 애니메이션이 없을 경우
			// 임시 오브젝트를 생성하여 넣는다.
			i3TransformSequence * pTS = i3TransformSequence::NewObjectRef();
			pTS->Create(1, 1, (i3TransformSequence::I3_KEY_CHANNEL)i3TransformSequence::TRANSLATE);
			pTS->SetKeyframeInterval(1);
			m_pAnimation->SetTrack(i, pTS, pName);
		}
	}

	return TRUE;
}


i3XFileAnimation *	i3XFileAnimationSet::GetAnimTrackByName(char * pName)
{
	i3XFileAnimation *	pAnimationTrack;
	INT32 NumAnimationTrack = m_AnimationTrackList.GetCount();
	for (INT32 i = 0 ; i < NumAnimationTrack ; ++i)
	{
		pAnimationTrack = (i3XFileAnimation*)m_AnimationTrackList.GetItem(i);
		if ( 0 == i3String::Compare(pAnimationTrack->GetRefFrameName(), pName) )
			return pAnimationTrack;
	}
	return NULL;
}



//-----------------------------------------------------------------------------
//		i3XFileAnimation
//-----------------------------------------------------------------------------
i3XFileAnimation::i3XFileAnimation()
: i3XFileTemplate()
, m_pRefFrame(NULL)
, m_pTransSeq(NULL)
{
}


i3XFileAnimation::~i3XFileAnimation()
{
	I3_SAFE_RELEASE(m_pRefFrame);
	I3_SAFE_RELEASE(m_pTransSeq);
	m_AnimationKeyList.SafeReleaseAll();
}


BOOL	i3XFileAnimation::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template Animation
	//{
	//	< 3D82AB4F-62DA-11cf-AB39-0020AF71E433 >
	//	[...]
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);

	// 필요한 작업이 없음

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileAnimation::ApplyChild(i3XFileTemplate* pChild)
{
	if (pChild->IsExactTypeOf(i3XFileFrame::GetClassMeta()))
	{
		I3ASSERT(m_pRefFrame == NULL);
		m_pRefFrame = (i3XFileFrame*)pChild;
		m_pRefFrame->AddRef();
	}
	else if (pChild->IsExactTypeOf(i3XFileAnimationKey::GetClassMeta()))
	{
		pChild->AddRef();
		m_AnimationKeyList.Add(pChild);
	}
	return i3XFileTemplate::ApplyChild(pChild);
}



BOOL	i3XFileAnimation::Build()
{
	I3ASSERT(m_AnimationKeyList.GetCount() > 0);


	i3XFileAnimationKey *	pAnimKey = (i3XFileAnimationKey*)m_AnimationKeyList.GetFirst();
	INT32	NumKeyItem	= pAnimKey->GetNumKeyItem();
	REAL32	Duration	= (REAL32)pAnimKey->GetDuration();
	REAL32	Interval	= (REAL32)pAnimKey->GetKeyInterval();

	//
	// i3XFileAnimationKey의 개수와 길이를 얻어내면서
	// i3XFileAnimationKey들의 값이 모두 같은지 검증한다.
	//
	INT32	NumKey = m_AnimationKeyList.GetCount();
	for (INT32 i = 1 ; i < NumKey ; ++i)
	{
		pAnimKey = (i3XFileAnimationKey*)m_AnimationKeyList.GetItem(i);
		I3ASSERT(NumKeyItem == pAnimKey->GetNumKeyItem());
		I3ASSERT(Duration == pAnimKey->GetDuration());
		I3ASSERT(Interval == pAnimKey->GetKeyInterval());
	}


	//
	// 키 플래그를 조합한다.
	//
	UINT32	Chanel = (i3TransformSequence::TIME);	// 시간은 기본.

	for (INT32 i = 0 ; i < NumKey ; ++i)
	{
		pAnimKey = (i3XFileAnimationKey*)m_AnimationKeyList.GetItem(i);
		switch (pAnimKey->GetKeyType())
		{
		default:		I3ASSERT_0;		break;
		case XFILE_ANIM_KEY_ROTATION:	Chanel |= (UINT32)i3TransformSequence::ROTATE;	break;
		case XFILE_ANIM_KEY_SCALE:		Chanel |= (UINT32)i3TransformSequence::SCALE;	break;
		case XFILE_ANIM_KEY_POSITION:	Chanel |= (UINT32)i3TransformSequence::TRANSLATE;	break;
		case XFILE_ANIM_KEY_MATRIX:
			Chanel |= (i3TransformSequence::TRANSLATE | i3TransformSequence::ROTATE | i3TransformSequence::SCALE);
			break;
		}
	}

	//
	// 키 값을 삽입한다.
	//
	m_pTransSeq = i3TransformSequence::NewObject();
	m_pTransSeq->Create(NumKeyItem, Duration, (i3TransformSequence::I3_KEY_CHANNEL)Chanel);
	m_pTransSeq->SetKeyframeInterval(Interval);

	for (INT32 i = 0 ; i < NumKey ; ++i)
	{
		pAnimKey = (i3XFileAnimationKey*)m_AnimationKeyList.GetItem(i);
		for (INT32 k = 0 ; k < NumKeyItem ; ++k)
		{
			switch (pAnimKey->GetKeyType())
			{
			default:						I3ASSERT_0;				break;
			case XFILE_ANIM_KEY_ROTATION:	m_pTransSeq->SetRotation(k, pAnimKey->GetKeyRotation(k));		break;
			case XFILE_ANIM_KEY_SCALE:		m_pTransSeq->SetScale(k, pAnimKey->GetKeyScale(k));				break;
			case XFILE_ANIM_KEY_POSITION:	m_pTransSeq->SetTranslation(k, pAnimKey->GetKeyPossition(k));	break;
			case XFILE_ANIM_KEY_MATRIX:		m_pTransSeq->SetMatrix(k, pAnimKey->GetKeyMatrix(k));			break;
			}

			REAL32	Time = (REAL32)pAnimKey->GetTime(k);
			m_pTransSeq->SetTime(k, Time);
		}
	}

	return TRUE;
}



//-----------------------------------------------------------------------------
//		TimedFloatKeys
//-----------------------------------------------------------------------------
TimedFloatKeys::~TimedFloatKeys()
{
	I3_SAFE_DELETE_ARRAY(pValues);
}


BOOL	TimedFloatKeys::Parse(i3XFileLexer * pLexer)
{
	//template FloatKeys
	//{
	//	< 10DD46A9-775B-11cf-8F52-0040333594A3 >
	//	DWORD nValues;
	//	array float values[nValues];
	//} 

	//template TimedFloatKeys 
	//{ 
	//	< F406B180-7B3B-11cf-8F52-0040333594A3 > 
	//	DWORD time; 
	//	FloatKeys tfkeys; 
	//} 

	// FloatKeys와 TimedFloatKeys 템플릿을 결합하였음.

	char	TokenBuffer[TOKEN_BUF_SIZE];
	Time = (REAL32)pLexer->GetTokenInt(TokenBuffer) * i3XFileAnimationKey::GetTimeRate();
    NumValue = (UINT32)pLexer->GetTokenInt(TokenBuffer);

	I3ASSERT(NumValue>0);
	pValues = new REAL32[NumValue];

	for (UINT32 i = 0 ; i < NumValue ; ++i)
		pValues[i] = pLexer->GetTokenReal(TokenBuffer);
	return TRUE;
}



//-----------------------------------------------------------------------------
//		i3XFileAnimationKey
//-----------------------------------------------------------------------------

REAL32	i3XFileAnimationKey::s_TimeRate	= 0.001f;



i3XFileAnimationKey::i3XFileAnimationKey()
: i3XFileTemplate()
, m_KeyType(XFILE_ANIM_KEY_INVALD)
, m_NumKey(0)
, m_pTimedKeys(NULL)
{
}


i3XFileAnimationKey::~i3XFileAnimationKey()
{
	I3_SAFE_DELETE_ARRAY(m_pTimedKeys);
}


BOOL	i3XFileAnimationKey::Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	//template AnimationKey
	//{
	//	< 10DD46A8-775B-11CF-8F52-0040333594A3 >
	//	DWORD keyType;
	//	DWORD nKeys;
	//	array TimedFloatKeys keys[nKeys];
	//} 

	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);

	char	TokenBuffer[TOKEN_BUF_SIZE];

	m_KeyType = (XFILE_ANIM_KEY_TYPE)pLexer->GetTokenInt(TokenBuffer);
	I3ASSERT(m_KeyType == XFILE_ANIM_KEY_ROTATION ||
			m_KeyType == XFILE_ANIM_KEY_SCALE ||
			m_KeyType == XFILE_ANIM_KEY_POSITION ||
			m_KeyType == XFILE_ANIM_KEY_MATRIX);

	m_NumKey = (UINT32)pLexer->GetTokenInt(TokenBuffer);

	m_pTimedKeys = new TimedFloatKeys[m_NumKey];

	for (UINT32 i = 0 ; i < m_NumKey ; ++i)
		m_pTimedKeys[i].Parse(pLexer);

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	i3XFileAnimationKey::ApplyChild(i3XFileTemplate* pChild)
{
	I3ASSERT_0;
	return TRUE;
}



void	DiffDumpVertexArray(const char * pFileName, i3VertexArray * pVA1, i3VertexArray * pVA2)
{
	FILE * f = fopen(pFileName, "wt");

	i3VertexArray * pVA[2] = {pVA1, pVA2};
	INT32	NumVert[2] = {pVA1->GetCount(), pVA2->GetCount()};
	INT32	NumBlendWeight[2] = {pVA1->GetBlendWeightCount(), pVA2->GetBlendWeightCount()};
	INT32	NumBlendIndex[2] = {pVA1->GetBlendIndexCount(), pVA2->GetBlendIndexCount()};
	VEC3D	V[2] = {};
	BOOL	b[2] = {};
	UINT8	BlendIndices[2][4] = {};
	REAL32	BlendWeights[2][4] = {};

	fprintf(f, "num vert: %d, %d\n", NumVert[0], NumVert[1]);
	fprintf(f, "num blend weight: %d, %d\n", NumBlendWeight[0], NumBlendWeight[1]);
	fprintf(f, "num blend index: %d, %d\n", NumBlendIndex[0], NumBlendIndex[1]);


	pVA[0]->Lock();
	pVA[1]->Lock();

	INT32	NumMaxVert = max(NumVert[0], NumVert[1]);
	for (INT32 i = 0 ; i < NumMaxVert ; ++i)
	{
		i3mem::FillZero(V, sizeof(V));
		i3mem::FillZero(b, sizeof(b));

		fprintf(f, "******** %d ********\n", i);

		// 위치 비교
		for (INT32 k = 0 ; k < 2 ; ++k)
		{
			if ( i < NumVert[k] )
			{
				pVA[k]->GetPosition(i, &V[k]);
				b[k] = TRUE;
			}
		}

		if ((b[0] != b[1]) ||
			(V[0].x != V[1].x || V[0].y != V[1].y || V[0].z != V[1].z))
		{
			fprintf(f, "diff pos[%d]\n"\
				"\t1[%d]:%f, %f, %f\n"\
				"\t2[%d]:%f, %f, %f\n",
				i,
				b[0], V[0].x, V[0].y, V[0].z,
				b[1], V[1].x, V[1].y, V[1].z);
		}

		// 가중치 비교
		i3mem::FillZero(V, sizeof(V));
		i3mem::FillZero(b, sizeof(b));
		i3mem::FillZero(BlendWeights, sizeof(BlendWeights));

		for (INT32 k = 0 ; k < 2 ; ++k)
		{
			if ( i < NumVert[k] )
			{
				for (INT32 a = 0 ; a < NumBlendWeight[k] ; ++a)
					BlendWeights[k][a] = pVA[k]->GetBlendWeight(a, i);
				b[k] = TRUE;
			}
		}
		if ((b[0] != b[1]) ||
			(BlendWeights[0][0] != BlendWeights[1][0] ||
			BlendWeights[0][1] != BlendWeights[1][1] ||
			BlendWeights[0][2] != BlendWeights[1][2] ||
			BlendWeights[0][3] != BlendWeights[1][3]))
		{
			fprintf(f, "diff bw[%d]\n"\
				"\t1[%d]:%f, %f, %f, %f\n"\
				"\t2[%d]:%f, %f, %f, %f\n",
				i,
				b[0], BlendWeights[0][0], BlendWeights[0][1], BlendWeights[0][2], BlendWeights[0][3],
				b[1], BlendWeights[1][0], BlendWeights[1][1], BlendWeights[1][2], BlendWeights[1][3]);
		}

		// 블렌드 인덱스 비교
		i3mem::FillZero(V, sizeof(V));
		i3mem::FillZero(b, sizeof(b));
		i3mem::FillZero(BlendIndices, sizeof(BlendIndices));
		for (INT32 k = 0 ; k < 2 ; ++k)
		{
			if ( i < NumVert[k] )
			{
				for (INT32 a = 0 ; a < NumBlendIndex[k] ; ++a)
					BlendIndices[k][a] = pVA[k]->GetBlendIndex(a, i);
				b[k] = TRUE;
			}
		}

		if ((b[0] != b[1]) ||
			(BlendIndices[0][0] != BlendIndices[1][0] ||
			BlendIndices[0][1] != BlendIndices[1][1] ||
			BlendIndices[0][2] != BlendIndices[1][2] ||
			BlendIndices[0][3] != BlendIndices[1][3]))
		{
			fprintf(f, "diff bi[%d]\n"\
				"\t1[%d]:%d, %d, %d, %d\n"\
				"\t2[%d]:%d, %d, %d, %d\n",
				i,
				b[0], BlendIndices[0][0], BlendIndices[0][1], BlendIndices[0][2], BlendIndices[0][3],
				b[1], BlendIndices[1][0], BlendIndices[1][1], BlendIndices[1][2], BlendIndices[1][3]);
		}
	}

	pVA1->Unlock();
	pVA2->Unlock();
	
	fclose(f);
}


/*
//-----------------------------------------------------------------------------
//		classname
//-----------------------------------------------------------------------------
::CLASS()
: i3XFileTemplate()
{
}


::~CLASS()
{
}


BOOL	Parse(i3XFileLexer * pLexer, i3XFileParser * pParser, i3XFileTemplate * pParent)
{
	i3XFileTemplate::FirstParse(pLexer, pParser, pParent);

	return i3XFileTemplate::LastParse(pLexer);
}


BOOL	ApplyChild(i3XFileTemplate* pChild)
{
	I3ASSERT_0;
	return TRUE;
}


*/
