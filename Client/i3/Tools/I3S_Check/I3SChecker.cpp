#include "stdafx.h"
#include "I3SChecker.h"
#include "i3Base/string/ext/remove_slash_filename.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEF_OUT_NAME		"check_result.txt"

I3SChecker::I3SChecker(void)
{
	m_szTargetPath[ 0] = 0;
	m_szWorkPath[0] = 0;
	m_szOutputPath[0] = 0;
	m_bCheckHidden = false;

	m_bCheckIndexedGeometry	= true;
	m_bCheckTangentBasis	= true;
	m_bCheckTextureFormat	= true;
	m_bCheckBoundBox		= true;
	m_bCheckBoundSphere		= true;
	m_bCheckVertexBlend		= true;
	m_bCheckTexCoordSet		= true;
	m_bCheckVertexColor		= true;
	m_bCheckStaticTransform	= true;

	m_pOutStream = NULL;
}

I3SChecker::~I3SChecker(void)
{
}

void I3SChecker::_ResetLog( const char * pszFile)
{
	m_LogList.clear();

	char conv[MAX_PATH + 128];

	sprintf( conv, "%s ----------------\n", pszFile);

	m_LogList.push_back( conv);
}

static char tempstr[4096];
static char tempFormat[512];

void I3SChecker::_AddLog( const char * pszFormat, ...)
{
	va_list marker;

	va_start( marker, pszFormat);

	sprintf( tempFormat, " * %s", pszFormat);

	vsprintf( tempstr, tempFormat, marker);

	va_end( marker);

	m_LogList.push_back( tempstr);
}

void I3SChecker::_FlushLog(void)
{
	
	if( m_LogList.size() <= 1)
		return;

	for(size_t i = 0; i < m_LogList.size(); i++)
	{
		printf( m_LogList[i].c_str() );

		if( m_pOutStream != NULL)
		{
			fprintf( m_pOutStream, m_LogList[i].c_str());
		}
	}

	printf( "\n");
}

I3SG_TRAVERSAL_RESULT I3SChecker::_CheckNodeProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	I3SChecker * pChecker = (I3SChecker *) pUserData;

	if( pNode->GetBound() != NULL)
	{
		pChecker->m_BBoxCount ++;
	}

	if( i3::kind_of<i3Geometry*>(pNode)) //->IsTypeOf( i3Geometry::static_meta()))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;

		pChecker->_CHK_Geometry( pGeo);
	}
	else if( i3::kind_of<i3AttrSet*>(pNode)) //->IsTypeOf( i3AttrSet::static_meta()))
	{
		pChecker->_CHK_AttrSet( (i3AttrSet *) pNode);
	}
	else if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::static_meta()))
	{
		pChecker->_CHK_StaticTransform( (i3Transform *) pNode);
	}
	else if( i3::kind_of<i3Transform2*>(pNode)) //->IsTypeOf( i3Transform2::static_meta()))
	{
		i3Transform2 * pTrans2 = (i3Transform2 *) pNode;

		if( pTrans2->getShapeSet())
		{
			pChecker->_CHK_PhysixShapeSet( pTrans2->getShapeSet());
		}
	}
	else if( i3::kind_of<i3ShapeSetContainer*>(pNode)) //->IsTypeOf( i3ShapeSetContainer::static_meta()))
	{
		i3ShapeSetContainer * pContainer = (i3ShapeSetContainer *) pNode;

		if( pContainer->getShapeSet() != NULL)
		{
			pChecker->_CHK_PhysixShapeSet( pContainer->getShapeSet());
		}
	}
	else if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::static_meta()))
	{
		pChecker->_CHK_Body( (i3Body *) pNode);
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void I3SChecker::CheckFile( const char * pszFile)
{
	//printf( "CHECK : %s\n", pszFile);
	i3SceneFile file;

	_ResetLog( pszFile);

	if( m_szWorkPath[0] != 0)
	{
		::SetCurrentDirectory( m_szWorkPath);
	}

	if( file.Load( pszFile) == STREAM_ERR)
	{
		_AddLog( "파일을 열 수 없음.");
		goto ExitPart;
	}

	i3SceneGraphInfo * pSgInfo = file.getSceneGraphInfo();
	i3Node *		pRoot = NULL;

	// 처리 전에 Common Sg.가 있는 경우에 대한 처리.
	// 임시로 Common Sg.와 Instance Sg.를 이어주어
	// 한번의 Traversal로 처리가 가능하도록 해준다.
	if( pSgInfo->getCommonSg() != NULL)
	{
		pRoot = pSgInfo->getCommonSg();

		if( pSgInfo->getInstanceSg() != NULL)
		{
			i3Node * pTempNode = i3Node::new_object_ref();

			pTempNode->AddFlag( I3_NODEFLAG_VOLATILE);

			pRoot->AddChild( pTempNode);

			pTempNode->AddChild( pSgInfo->getInstanceSg());
		}
	}
	else
	{
		pRoot = pSgInfo->getInstanceSg();
	}

	if( pRoot == NULL)
	{
		_AddLog( "Root Node가 없는 빈 파일.\n");
		goto ExitPart;
	}

	// SceneGraph를 돌면서 개별 Node에 대한 검사 수행
	m_BBoxCount = 0;

	pRoot->Instantiate();

	i3Scene::Traverse( pRoot, (I3_SG_TRAVERSE_PROC) _CheckNodeProc, this);

	_CHK_BoundBox();

	m_Count++;

ExitPart:
	_FlushLog();
}

bool I3SChecker::_I3SCallback( INT32 Level, char * pszPath, void * pFileInfo, void * pUserData)
{
	I3SChecker * pChecker = (I3SChecker *) pUserData;
	WIN32_FIND_DATA * pInfo = (WIN32_FIND_DATA *) pFileInfo;
	
	char szFull[ MAX_PATH];

	if( pChecker->getCheckHiddenState() == false)
	{
		if( pInfo->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			return true;
	}

	sprintf( szFull, "%s/%s", pszPath, pInfo->cFileName);

	pChecker->CheckFile( szFull);

	return true;
}

INT32 I3SChecker::Check(void)
{
	if( m_szTargetPath[0] == 0)
	{
		printf( "[ERR] 검사할 디렉토리가 지정되지 않았으.\n");
		return -1;
	}

	if( m_szOutputPath[0] == 0)
	{
		char szTemp[MAX_PATH];

		if( i3System::IsDirectory( m_szTargetPath))
		{
			sprintf( m_szOutputPath, "%s/%s", m_szTargetPath, DEF_OUT_NAME);
		}
		else
		{
			i3::safe_string_copy( szTemp, m_szTargetPath, MAX_PATH );
		//	i3String::RemoveFileName( szTemp);
			i3::remove_slash_filename(szTemp);

			sprintf( m_szOutputPath, "%s/%s", szTemp, DEF_OUT_NAME);
		}

		m_pOutStream = fopen( m_szOutputPath, "wt");
	}

	m_Count = 0;

	if( i3System::IsDirectory( m_szTargetPath))
	{
		i3System::LookForFiles( m_szTargetPath, "*.I3S", _I3SCallback, this);
	}
	else if( i3System::IsFile( m_szTargetPath))
	{
		CheckFile( (const char *) m_szTargetPath);
	}
	else
	{
		fclose( m_pOutStream);

	//	printf( "[ERR] 지정된 %s 경로는 뭣도 아님. --\n");
		printf("[ERR] 지정된 경로는 뭣도 아님. --\n");
		return -1;
	}

	fclose( m_pOutStream);

	printf( "\nTotal %d files are checked.\n", m_Count);

	return 0;
}
