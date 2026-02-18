#include "stdafx.h"
#include "i3XFileParser.h"

#include "i3Graph.h"
#include "i3GraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define		NO_IMP
#define		NEEDED_IMPROVE
#define		TEMP_CODE


// 파일 헤더
#define		MAGIC_NUMBER				"xof "
#define		VERSION_NUMBER				"0303"
#define		FORMAT_TYPE_TXT				"txt "
#define		FORMAT_TYPE_BIN				"bin "
#define		FORMAT_TYPE_TXTZIP			"tzip"
#define		FORMAT_TYPE_BINZIP			"bzip"
#define		FLOAT_SIZE32				"0032"
#define		FLOAT_SIZE64				"0064"






//------------------------------------------------------------------------
//		i3XFileParser


i3XFileParser::i3XFileParser()
: m_pLexer(NULL)
, m_pStream(NULL)

, m_CreatedRoot(FALSE)

, m_Header()
, m_pSgInfo(NULL)

, m_pRootNode(NULL)
, m_pRootAttrSet(NULL)
, m_TextureEnable(FALSE)

, m_DataObjectList()

, m_MeshList()
, m_FrameList()
, m_SkinWeightList()
, m_AnimationSetList()

, m_pLogWnd(NULL)
{
	i3mem::FillZero(m_FileFullPath, sizeof(m_FileFullPath));
	i3mem::FillZero(m_FilePath, sizeof(m_FilePath));
	i3mem::FillZero(m_FileName, sizeof(m_FileName));

	i3mem::FillZero(&m_Header, sizeof(m_Header));

	i3XFileMesh::RegisterMeta();
	i3XFileMeshNormals::RegisterMeta();
	i3XFileMeshTextureCoords::RegisterMeta();
	i3XFileMaterial::RegisterMeta();
	i3XFileMeshMaterialList::RegisterMeta();
	i3XFileTextureFilename::RegisterMeta();
	i3XFileFrame::RegisterMeta();
	i3XFileFrameTransformMatrix::RegisterMeta();
	i3XFileXSkinMeshHeader::RegisterMeta();
	i3XFileSkinWeights::RegisterMeta();
	i3XFileAnimationSet::RegisterMeta();
	i3XFileAnimation::RegisterMeta();
	i3XFileAnimationKey::RegisterMeta();

}


i3XFileParser::~i3XFileParser()
{
	Clear();
}


BOOL i3XFileParser::Open(const char* pFileName)
{
	i3FileStream* pStream = i3FileStream::NewObject();

	if ( !pStream->Open(pFileName, STREAM_READ|STREAM_SHAREREAD) )
	{
		Log(I3GRAP_LOG_ERROR, "파일 오픈을 실패하였습니다.");
		return FALSE;
	}

	Clear();

	i3String::Copy(m_FileFullPath, pFileName, MAX_PATH );
	i3String::SplitPath(pFileName, m_FilePath);
	i3String::SplitFileName(pFileName, m_FileName, TRUE);

	BOOL rv = Open(pStream);

	pStream->Close();
	I3_SAFE_RELEASE(pStream);

	return rv;
}


BOOL	i3XFileParser::Open(i3FileStream* pStream)
{
	m_pStream = pStream;

	if ( !CheckFileHeader(pStream) )
		return FALSE;

	if ( !BeginParsing() )
		return FALSE;

	if ( !EndParsing() )
		return FALSE;

	EndLogMessage();
	return TRUE;
}


void		i3XFileParser::Log(INT32 LogLevel, const char *format, ...)
{
	char	tempstr[256];

	va_list marker;
	va_start( marker, format);
	vsprintf( tempstr, format, marker);

	m_pLogWnd->Log(LogLevel, tempstr);
}


void	i3XFileParser::Clear()
{
	m_FileFullPath[0] = NULL;
	m_FilePath[0] = NULL;
	m_FileName[0] = NULL;

	I3_SAFE_DELETE(m_pLexer);
	m_pStream = NULL;

	m_CreatedRoot = FALSE;

	i3mem::FillZero(&m_Header, sizeof(m_Header));
	I3_SAFE_RELEASE(m_pSgInfo);
	m_TextureEnable = FALSE;

	m_DataObjectList.SafeReleaseAll();

	m_MeshList.SafeReleaseAll();
	m_FrameList.SafeReleaseAll();
	m_SkinWeightList.SafeReleaseAll();
	m_AnimationSetList.SafeReleaseAll();
}


void	i3XFileParser::EndLogMessage()
{
	char mg[5] = {};
	char vn[5] = {};
	char ft[5] = {};
	char fs[5] = {};

	i3String::NCopy(mg, m_Header.MagicNumber, MAGIC_NUMBER_SIZE);
	i3String::NCopy(vn, m_Header.VersionNumber, VERSION_NUMBER_SIZE);
	i3String::NCopy(ft, m_Header.FormatType, FORMAT_TYPE_SIZE);
	i3String::NCopy(fs, m_Header.FloatSize, FLOAT_SIZE);

	Log(I3GRAP_LOG_NORMAL, "Import %s format[%s,%s,%s,%s]",
		m_FileName, mg, vn, ft, fs);

}



i3XFileTemplate *	i3XFileParser::GetDataObjectByName(const char* pName)
{
	i3XFileTemplate *	pTemplate;
	INT32	NumTemplate = m_DataObjectList.GetCount();
	for (INT32 i = 0 ; i < NumTemplate ; ++i)
	{
		pTemplate = (i3XFileTemplate*)m_DataObjectList.GetItem(i);
		if (0 == i3String::Compare(pTemplate->GetName(), pName))
			return pTemplate;
	}

	return NULL;
}


void		i3XFileParser::StartParseTemplate(i3XFileTemplate * pTemplate)
{
	if (pTemplate->IsExactTypeOf(i3XFileFrame::GetClassMeta()))
	{
		pTemplate->AddRef();
		m_FrameList.Add(pTemplate);
	}
}


void		i3XFileParser::EndParseTemplate(i3XFileTemplate * pTemplateClass)
{
	if (pTemplateClass->IsExactTypeOf(i3XFileMesh::GetClassMeta()))
	{
		i3XFileMesh * pMesh = (i3XFileMesh*)pTemplateClass;
		m_MeshList.Add(pMesh);
		pMesh->AddRef();
	}
	else if (pTemplateClass->IsExactTypeOf(i3XFileSkinWeights::GetClassMeta()))
	{
		i3XFileSkinWeights * pSkinWeight = (i3XFileSkinWeights*)pTemplateClass;
		m_SkinWeightList.Add(pSkinWeight);
		pSkinWeight->AddRef();
	}
	else if (pTemplateClass->IsExactTypeOf(i3XFileTextureFilename::GetClassMeta()))
	{
		m_TextureEnable = TRUE;
	}
	else if (pTemplateClass->IsExactTypeOf(i3XFileAnimationSet::GetClassMeta()))
	{
		m_AnimationSetList.Add(pTemplateClass);
		pTemplateClass->AddRef();
	}

	pTemplateClass->AddRef();
	m_DataObjectList.Add(pTemplateClass);
}


BOOL	i3XFileParser::CheckFileHeader(i3FileStream* pStream)
{
	pStream->Read(&m_Header, sizeof(m_Header));

	// 매직넘버
	INT32	ret = 0;
	ret = i3String::NCompare(m_Header.MagicNumber, MAGIC_NUMBER, MAGIC_NUMBER_SIZE);
	if ( ret != 0 )
		return FALSE;

	// 파일 버전
	INT32	ParserVer = atoi(VERSION_NUMBER);
	INT32	FileVer	= atoi(m_Header.VersionNumber);
	if ( ParserVer < FileVer )
	{
		Log(I3GRAP_LOG_WARNING, "버전이 맞지 않아 문제가 발생할 수 있습니다. parser ver:%s, file ver:%s",
			VERSION_NUMBER, m_Header.VersionNumber);
	}

	// 파일 타입
	if (0 == i3String::NCompare(m_Header.FormatType, FORMAT_TYPE_TXT, FORMAT_TYPE_SIZE))
	{
		m_pLexer = new i3XFileTextLexer();
	}
	else if (0 == i3String::NCompare(m_Header.FormatType, FORMAT_TYPE_BIN, FORMAT_TYPE_SIZE))
	{
		m_pLexer = new i3XFileBinLexer();
	}
	else
	{
		Log(I3GRAP_LOG_ERROR,"파일을 읽을수 없습니다. 지원하지 않는 형식입니다.[type:%s]\n", m_Header.FormatType);
		return FALSE;
	}

	

	ret = i3String::NCompare(m_Header.FloatSize, FLOAT_SIZE32, FLOAT_SIZE);
	if ( ret != 0 )
	{
		ret = i3String::NCompare(m_Header.FloatSize, FLOAT_SIZE64, FLOAT_SIZE);
		if ( ret != 0 )
		{
			Log(I3GRAP_LOG_ERROR, "Float Size가 잘못되었습니다.");
			return FALSE;
		}
	}

	return TRUE;
}


BOOL	i3XFileParser::BeginParsing()
{
	BOOL	brv = TRUE;
	char	TokenBuffer[TOKEN_BUF_SIZE];
	UINT32	TokenNumber = XTOKEN_INVALID;


	m_pLexer->SetStream(m_pStream);

	while(true)
	{
		TokenNumber = m_pLexer->GetToken( TokenBuffer);

		// 현재 파싱중인 타입을 저장해 둔다.
		if (m_pLexer->IsTemplateIden(TokenNumber))
		{
			if (IsIgnoreTemplate(TokenNumber))
			{
				m_pLexer->FindToken(XTOKEN_BLOCK_BEGIN);
				m_pLexer->SkipCurrentBlock();
				goto NoTemplateProcess;
			}

			char TemplateClassName[MAX_PATH] = "i3XFile";
			i3String::Concat(TemplateClassName, TokenBuffer);

			i3ClassMeta * pMeta = i3ClassMeta::FindClassMetaByName(TemplateClassName);
			I3ASSERT(pMeta != NULL);

			i3XFileTemplate * pTemplateClass = (i3XFileTemplate*)CREATEINSTANCE(pMeta);

			if (XTOKEN_MESH == TokenNumber && m_CreatedRoot != TRUE)
			{
				m_pRootNode = i3Node::NewObjectRef();
				m_pRootAttrSet = i3AttrSet::NewObjectRef();
				m_pRootNode->AddChild(m_pRootAttrSet);

				m_CreatedRoot = TRUE;
			}
			else if (XTOKEN_FRAME == TokenNumber && TRUE != m_CreatedRoot)
			{
				i3Transform *	pTransform	= i3Transform::NewObjectRef();
				i3Body *	pBody	= i3Body::NewObjectRef();
				i3LOD *		pLOD	= i3LOD::NewObjectRef();
				i3Node *	pNode	= i3Node::NewObjectRef();

				m_pRootAttrSet	= i3AttrSet::NewObjectRef();
				m_pRootNode = pTransform;

				pTransform->AddChild(pBody);
				pBody->AddLOD(pLOD);
				pLOD->setShapeNode(pNode);
				pNode->AddChild(m_pRootAttrSet);

				m_CreatedRoot = TRUE;
			}

			if ( !pTemplateClass->Parse(m_pLexer, this, NULL) )
			{
				Log(I3GRAP_LOG_ERROR, "파싱을 실패하였습니다.[%s]", TemplateClassName);
				return FALSE;
			}
		}
		else
		{
			switch ( TokenNumber )
			{

				// 무시하는 토큰들.
			case XTOKEN_TEMPLATE:
			case XTOKEN_HEADER:
				m_pLexer->FindToken(XTOKEN_BLOCK_END);
				break;

			case XTOKEN_FILE_END:
				goto ExitLoop;
				break;

			case XTOKEN_INVALID:
				I3TRACE1("정의되지 않은 예약어입니다. [%s]\n", m_pLexer->GetIdenString(TokenNumber));
				I3ASSERT_0;
				break;

			default:
				I3ASSERT_0;
				break;
			}
		}

NoTemplateProcess:

		if (TRUE != brv)
			return FALSE;
	}

ExitLoop:


	return TRUE;
}


BOOL	i3XFileParser::EndParsing()
{
	BOOL	brv;
	brv = BuildFrame();
	if ( ! brv )
	{
		Log(I3GRAP_LOG_ERROR, "BuildFrame 실패\n");
		return FALSE;
	}

	brv = BuildMesh();
	if ( ! brv )
	{
		Log(I3GRAP_LOG_ERROR, "BuildMesh 실패\n");
		return FALSE;
	}

	brv = BuildAnimationSet();
	if ( ! brv )
	{
		Log(I3GRAP_LOG_ERROR, "BuildAnimationSet 실패\n");
		return FALSE;
	}

	m_pSgInfo = i3SceneGraphInfo::NewObject();
	m_pSgInfo->setInstanceSg(m_pRootNode);

	return TRUE;
}


BOOL	i3XFileParser::BuildFrame()
{
	if (m_FrameList.GetCount() == 0)
		return TRUE;

	if ( !CalcuBoneIndex() )
		return FALSE;

	if ( !CalcuSkinBoneIndex() )
		return FALSE;


	//
	// Skeleton으로 변환한다.
	//

	INT32	NumFrame = m_FrameList.GetCount();

	i3MatrixArray * pInvBoneArray = i3MatrixArray::NewObjectRef();
	pInvBoneArray->Create(NumFrame);

	i3Skeleton *	pSkeleton = i3Skeleton::NewObjectRef();
	pSkeleton->SetBoneCount(NumFrame);
	pSkeleton->getBoneMatrixListAttr()->setInverseBoneArray(pInvBoneArray);



	// 본 오프셋 TM 저장
	i3XFileFrame * pFrame;
	i3XFileSkinWeights * pSkinWeight;

	char * pName;
	MATRIX	iden;
	i3Bone*	pDestBone, *pSrcBone;
	i3Matrix::Identity(&iden);

	for (INT32 i = 0 ; i < NumFrame ; ++i)
	{
		pFrame = (i3XFileFrame*)m_FrameList.GetItem(i);
		pName = pFrame->GetName();
		pSkinWeight = FindSkinWeightInfoByName(pName);

		if (pSkinWeight)
		{
			MATRIX * pOffset = pSkinWeight->GetOffset();
			pInvBoneArray->SetMatrix(i, pOffset);
		}
		else
		{
			pInvBoneArray->SetMatrix(i, &iden);
		}


		// 본 정보 카피
		pDestBone = (i3Bone*)pSkeleton->getBone(i);
		pSrcBone = pFrame->GetBone();
		i3mem::Copy(pDestBone, pSrcBone, sizeof(i3Bone));
	}


	//
	// 그래프 구축
	//
	i3Transform *	pTransform = (i3Transform*)m_pRootNode;
	i3Body *	pBody = (i3Body*)pTransform->GetChild(0);
	i3LOD*		pLOD = (i3LOD*)pBody->getLOD(0);
	pLOD->setSkeleton(pSkeleton);

	pBody->Prepare();

	return TRUE;
}


BOOL	i3XFileParser::BuildMesh()
{
	if (m_TextureEnable)
	{
		i3TextureEnableAttr *	pTexEnable = i3TextureEnableAttr::NewObjectRef();
		pTexEnable->Set(TRUE);
		m_pRootAttrSet->AddAttr(pTexEnable);
	}

	i3XFileMesh *	pMesh = NULL;
	INT32	NumMesh	= m_MeshList.GetCount();
	for (INT32 i = 0 ; i < NumMesh ; ++i)
	{
		pMesh = (i3XFileMesh*)m_MeshList.GetItem(i);
		if ( !pMesh->Build() )
			return FALSE;

		m_pRootAttrSet->AddChild(pMesh->GetAttrSet());
	}

	return TRUE;
}


BOOL	i3XFileParser::BuildAnimationSet()
{
	INT32 NumSet = m_AnimationSetList.GetCount();
	if (NumSet == 0)
		return TRUE;

	I3ASSERT(m_AnimationSetList.GetCount() < 2);

	// 본 순서와 같이 본 이름을 만들어 준다.
	i3StringList	BoneNameList;
	{
		i3XFileFrame *	pFrame;
		INT32 NumFrame = m_FrameList.GetCount();
		char * name;
		for (INT32 i = 0 ; i < NumFrame ; ++i)
		{
			pFrame = (i3XFileFrame*)m_FrameList.GetItem(i);
			name = pFrame->GetBone()->getName();
			if (*name != NULL)
				BoneNameList.Add(name);
			else
				BoneNameList.Add("__noname");
		}
	}

	i3XFileAnimationSet *	pAnimSet;
	for (INT32 i = 0 ; i < NumSet ; ++i)
	{
		pAnimSet = (i3XFileAnimationSet*)m_AnimationSetList.GetItem(i);
		if ( !pAnimSet->Build(&BoneNameList) )
			return FALSE;
	}

	pAnimSet = (i3XFileAnimationSet*)m_AnimationSetList.GetFirst();

	// 위 i3XFileAnimationSet 여러개에 대한 빌드는 하였으나
	// 아래 설정해주는것도 처리해야함.
	// 애니메이션 다수를 가지는 클래스를 찾자!
	I3ASSERT(m_pRootNode->IsExactTypeOf(i3Transform::GetClassMeta()));
	i3Transform *	pTransform = (i3Transform*)m_pRootNode;
	i3Body *	pBody = (i3Body*)pTransform->GetChild(0);
	i3Animation *	pAnimation = pAnimSet->GetAnimation();
	pBody->PlayAnim(pAnimation);

	return TRUE;
}


BOOL	i3XFileParser::CalcuBoneIndex()
{
	i3XFileFrame *	pFrame, * pParentFrame;
	i3Bone *	pBone;
	INT32	NumFrame = m_FrameList.GetCount();

	for (INT32 i = 0 ; i < NumFrame ; ++i)
	{
		pFrame = (i3XFileFrame*)m_FrameList.GetItem(i);
		pBone = pFrame->GetBone();
		I3ASSERT(pBone != NULL);

		pParentFrame = (i3XFileFrame*)pFrame->GetParent();
		if (pParentFrame != NULL)
		{
			INT32 ParentIndex = m_FrameList.IndexOf(pParentFrame);
			I3ASSERT(ParentIndex != -1);
			pBone->setParentIndex(ParentIndex);
		}
		else
		{
			pBone->setParentIndex(-1);
		}
	}

	return TRUE;
}


BOOL	i3XFileParser::CalcuSkinBoneIndex()
{
	//
	// 수집된 i3XFileSkinWeights에 Frame 인덱스를 넣어준다.
	//

	i3XFileFrame *			pFrame		= NULL;
	i3XFileSkinWeights *	pSkinWeight = NULL;

	INT32	NumSkinWeight = m_SkinWeightList.GetCount();
	for (INT32 i = 0 ; i < NumSkinWeight ; ++i)
	{
		pSkinWeight = (i3XFileSkinWeights*)m_SkinWeightList.GetItem(i);
		pFrame = FindFrameByName(pSkinWeight->GetTransformNodeName());
		I3ASSERT(m_FrameList.IndexOf(pFrame) > 0);
		INT32 Index = m_FrameList.IndexOf(pFrame);
		pSkinWeight->SetFrameIndex(Index);
	}

	return TRUE;
}


i3XFileFrame *			i3XFileParser::FindFrameByName(const char * pName)
{
	i3XFileFrame *			pFrame;
	INT32	NumFrame	= m_FrameList.GetCount();
	for (INT32 i = 0 ; i < NumFrame ; ++i)
	{
		pFrame = (i3XFileFrame*)m_FrameList.GetItem(i);
		if ( 0 == i3String::Compare(pFrame->GetName(), pName) )
			return pFrame;
	}

	return NULL;
}


i3XFileSkinWeights *	i3XFileParser::FindSkinWeightInfoByName(const char * pName)
{
	i3XFileSkinWeights *	pSkinWeight = NULL;
	INT32	NumSkinWeight = m_SkinWeightList.GetCount();
	for (INT32 i = 0 ; i < NumSkinWeight ; ++i)
	{
		pSkinWeight = (i3XFileSkinWeights*)m_SkinWeightList.GetItem(i);
		if (0 == i3String::Compare(pSkinWeight->GetTransformNodeName(), pName))
		{
			return pSkinWeight;
		}
	}
	return NULL;
}


BOOL	IsIgnoreTemplate(UINT32 IdenNumber)
{
	switch (IdenNumber)
	{
	//case XTOKEN_ANIMATIONSET:
	//case XTOKEN_ANIMATION:
	//case XTOKEN_ANIMATIONKEY:
	case XTOKEN_ANIMATIONOPTIONS:
	case XTOKEN_BOOLEAN:
	case XTOKEN_BOOLEAN2D:
	case XTOKEN_COLORRGB:
	case XTOKEN_COLORRGBA:
	case XTOKEN_COMPRESSEDANIMATIONSET:
	case XTOKEN_COORDS2D:
	case XTOKEN_DECLDATA:
	case XTOKEN_EFFECTDWORD:
	case XTOKEN_EFFECTFLOATS:
	case XTOKEN_EFFECTINSTANCE:
	case XTOKEN_EFFECTPARAMDWORD:
	case XTOKEN_EFFECTPARAMFLOATS:
	case XTOKEN_EFFECTPARAMSTRING:
	case XTOKEN_EFFECTSTRING:
	case XTOKEN_FACEADJACENCY:
	case XTOKEN_FLOATKEYS:
	//case XTOKEN_FRAME:
	//case XTOKEN_FRAMETRANSFORMMATRIX:
	case XTOKEN_FVFDATA:
	case XTOKEN_GUID:
	case XTOKEN_INDEXEDCOLOR:
	//case XTOKEN_MATERIAL:
	case XTOKEN_MATERIALWRAP:
	case XTOKEN_MATRIX4X4:
	//case XTOKEN_MESH:
	case XTOKEN_MESHFACE:
	case XTOKEN_MESHFACEWRAPS:
	//case XTOKEN_MESHMATERIALLIST:
	//case XTOKEN_MESHNORMALS:
	//case XTOKEN_MESHTEXTURECOORDS:
	case XTOKEN_MESHVERTEXCOLORS:
	case XTOKEN_PATCH:
	case XTOKEN_PATCHMESH:
	case XTOKEN_PATCHMESH9:
	case XTOKEN_PMATTRIBUTERANGE:
	case XTOKEN_PMINFO:
	case XTOKEN_PMVSPLITRECORD:
	//case XTOKEN_SKINWEIGHTS:
	//case XTOKEN_TEXTUREFILENAME:
	case XTOKEN_TIMEDFLOATKEYS:
	case XTOKEN_VECTOR:
	case XTOKEN_VERTEXELEMENT:
	//case XTOKEN_XSKINMESHHEADER:

		// 파싱을 하지 않을 데이터
	case XTOKEN_VERTEXDUPLICATIONINDICES:
		return TRUE;
	}

	return FALSE;
}