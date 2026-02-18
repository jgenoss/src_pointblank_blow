#include "i3FrameworkPCH.h"
#include "i3GameResSceneGraph.h"
#include "i3Framework.h"
#include "i3Chara.h"

#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/i3RSGHelper.h"

I3_CLASS_INSTANCE( i3GameResSceneGraph);

i3GameResSceneGraph::i3GameResSceneGraph(void)
{
	m_Type	= I3_GAMERES_SCENE;
}

i3GameResSceneGraph::~i3GameResSceneGraph(void)
{
	Destroy();
}

void i3GameResSceneGraph::Bind( i3Framework * pFramework, i3GameObjBase * pObj, bool bInstancing, bool bClone)
{
	if( m_pBodyRoot == nullptr)
	{
		// 아직 한번도 Framework에 들어간 적이 없다.
		if (i3::kind_of<i3Chara* >(pObj))
		{
			//CHAR_SOLO_RENDER
			i3Chara* pChar = (i3Chara*)pObj;
			int nPVType = pChar->GetPVType();

			//i3Node* pTmpNode = pFramework->getPlayerSgContext()->get3DRoot();

			if (nPVType == 0)//CHARA_VIEW_1PV
				m_pBodyRoot = pFramework->OnPlayerQueryAttachNode(nullptr);//1인칭일 경우.. Depth를 클리어한 노드에 넣어준다...
			else
				m_pBodyRoot = pFramework->OnQueryAttachNode(this);
		}
		else
		{
			m_pBodyRoot = pFramework->OnQueryAttachNode( this);
		}

		// revision 2116
		if( (m_pSgInfo != nullptr) && (m_pSgInfo->getCommonSg() != nullptr) )
		{
			m_pBodyRoot->AddChild( m_pSgInfo->getCommonSg());

			// 가장 Leaf Node를 찾는다.
			for( m_pBodyRoot = m_pSgInfo->getCommonSg(); m_pBodyRoot->GetChildCount() > 0;)
			{
				m_pBodyRoot = m_pBodyRoot->GetChild( 0);
			}

			m_bBindedCommonSG = true;
		}

		// Common Sg가 없는 경우엔 Framework에서 지정한 지점에 직접 붙인다.
	}

	if( bInstancing)
	{
		if( getScene() != nullptr)
		{
			if( bClone)
			{
				i3Node * pClone = i3Scene::CreateClone( getScene(), I3_COPY_INSTANCE);

				pObj->SetInstanceSg( pClone);
			}
			else
			{
				pObj->SetInstanceSg( getScene());
			}
		}
	}

	i3GameRes::Bind( pFramework, pObj, bInstancing, bClone);
}

void i3GameResSceneGraph::SetCommonScene( i3Node * pNode)
{
	if( m_pSgInfo == nullptr)
	{
		m_pSgInfo = i3SceneGraphInfo::new_object();
	}

	m_pSgInfo->setCommonSg( pNode);
}

void i3GameResSceneGraph::SetScene( i3Node * pNode)
{
	if( m_pSgInfo == nullptr)
	{
		m_pSgInfo = i3SceneGraphInfo::new_object();
	}

	m_pSgInfo->setInstanceSg( pNode);
}

void i3GameResSceneGraph::Destroy( void)
{
	if( m_pSgInfo != nullptr)
	{
		// Framework의 Sg.에서 제거한다.
		if( m_bBindedCommonSG && m_pSgInfo->getCommonSg() != nullptr)
		{
			m_pSgInfo->getCommonSg()->RemoveFromParent();
		}

		I3_MUST_RELEASE( m_pSgInfo);
		m_pSgInfo = nullptr;
	}
}

void i3GameResSceneGraph::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node * pNode;

	i3GameRes::CopyTo( pDest, method);

	if( !i3::same_of<i3GameResSceneGraph* >(pDest))
		return;

	i3GameResSceneGraph * pRes = (i3GameResSceneGraph *) pDest;

	// Common Scene
	if( getCommonScene() != nullptr)
	{
		if( method == I3_COPY_INSTANCE)
			pNode = i3Scene::CreateClone( getCommonScene(), I3_COPY_INSTANCE);
		else
			pNode = getCommonScene();

		pRes->SetCommonScene( pNode);
	}

	// Scene
	if( getScene() != nullptr)
	{
		if( method == I3_COPY_INSTANCE)
			pNode = i3Scene::CreateClone( getScene(), I3_COPY_INSTANCE);
		else
			pNode = getScene();

		pRes->SetScene( pNode);
	}

	pRes->setLayer( getLayer());
}

UINT32 i3GameResSceneGraph::SaveToFile( i3Stream * pStream)
{
	UINT32 Result, Rc;
	Result = i3GameRes::SaveToFile( pStream);

	i3::pack::RSC_SCENE_2 scene;
	{
		scene.m_nLayer = m_Layer;
		scene.m_szClass[0] = '0';
		if( isCommonRes())
		{
			scene.m_bCommon = 1;
		}
		else
		{
			scene.m_bCommon = 0;
		}
	}

	Rc = pStream->Write( &scene, sizeof( scene));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3GameResSceneGraph::LoadFromFile( i3Stream * pStream, INT32 nVersion)
{
	UINT32 nResult = 0;

	if( nVersion == 1)
	{
		i3::pack::RSC_SCENE scene;
		nResult = pStream->Read( &scene, sizeof( scene));
		I3_CHKIO( nResult);

		setLayer( scene.m_nLayer);

		if( i3::generic_string_size( scene.m_szCommon) > 0)
		{
			if( i3::generic_is_equal( scene.m_szCommon, "1") )
				addResStyle( I3_RES_STYLE_COMMON);
		}
	}
	else if( nVersion == 2)
	{
		i3::pack::RSC_SCENE_2 scene;
		nResult = pStream->Read( &scene, sizeof( scene));
		I3_CHKIO( nResult);

		setLayer( scene.m_nLayer);
		if( scene.m_bCommon > 0)
		{
			addResStyle( I3_RES_STYLE_COMMON);
		}
	}
	else
	{
		nResult = STREAM_ERR;
	}

	return nResult;
}

UINT32 i3GameResSceneGraph::LoadFromFile(i3IniParser * pParser, INT32 nVersion, INT32 nType)
{
	UINT32 nResult = 0;

	i3::vector<i3::any>	_FieldData;
	_FieldData.clear();
	RSGParser(pParser, nType, _FieldData);
	if (nVersion == 1)
	{
		i3::pack::RSC_SCENE scene;
		
		scene.m_nLayer = *i3::unsafe_any_cast<INT32>(&_FieldData[0]);
		i3::generic_string_copy(scene.m_szClass, i3::unsafe_any_cast<i3::rc_string>(&_FieldData[1])->c_str());
		i3::generic_string_copy(scene.m_szCommon, i3::unsafe_any_cast<i3::rc_string>(&_FieldData[2])->c_str());
		/*nResult = pStream->Read(&scene, sizeof(scene));*/
		//I3_CHKIO(nResult);

		setLayer(scene.m_nLayer);

		if (i3::generic_string_size(scene.m_szCommon) > 0)
		{
			if (i3::generic_is_equal(scene.m_szCommon, "1"))
				addResStyle(I3_RES_STYLE_COMMON);
		}
	}
	else if (nVersion == 2)
	{
		i3::pack::RSC_SCENE_2 scene;
		//nResult = pStream->Read(&scene, sizeof(scene));
		//I3_CHKIO(nResult);
		scene.m_nLayer = *i3::unsafe_any_cast<INT32>(&_FieldData[0]);
		i3::generic_string_copy(scene.m_szClass, i3::unsafe_any_cast<i3::rc_string>(&_FieldData[1])->c_str());
		scene.m_bCommon = *i3::unsafe_any_cast<INT32>(&_FieldData[2]);

		setLayer(scene.m_nLayer);
		if (scene.m_bCommon > 0)
		{
			addResStyle(I3_RES_STYLE_COMMON);
		}
	}
	else
	{
		nResult = STREAM_ERR;
	}

	return nResult;
}

void	i3GameResSceneGraph::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( isExternal() == false)
	{
		if( m_pSgInfo != nullptr)
		{
			m_pSgInfo->OnBuildObjectList( List);
		}
	}

	i3GameRes::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 GAMERES_SCENEGRAPH
	{
		UINT8		m_ID[4] = { 'R', 'S', 'G', '1' };
		INT32		m_Layer = 0;
		OBJREF64	m_pSgInfo = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	i3GameResSceneGraph::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES_SCENEGRAPH data;

	Result = i3GameRes::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_Layer = getLayer();
	data.m_pSgInfo = (OBJREF64) pResFile->GetObjectPersistID( getSceneGraphInfo());

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GameResSceneGraph::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES_SCENEGRAPH data;

	Result = i3GameRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);

	setLayer( data.m_Layer);

	I3ASSERT( data.m_pSgInfo != 0);
	if( data.m_pSgInfo != 0)
	{
		i3SceneGraphInfo * pInfo = (i3SceneGraphInfo *) pResFile->FindObjectByID( (OBJREF) data.m_pSgInfo);
		I3ASSERT( pInfo != nullptr);
		setSceneGraphInfo( pInfo);
	}

	return Result;
}
