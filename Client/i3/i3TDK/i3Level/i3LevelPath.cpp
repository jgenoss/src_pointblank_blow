#include "stdafx.h"
#include "i3LevelPath.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelPathLink.h"
#include "i3LevelPathPoint.h"
#include "i3LevelResPath.h"
#include "i3Base/itl/container_util.h"

I3_CLASS_INSTANCE( i3LevelPath);

i3LevelPath::i3LevelPath(void)
{
	setInstanceClassName( "i3GamePath");

	AddStyle( I3_LEVEL_STYLE_UNSELECT | I3_LEVEL_STYLE_NOMOVE | I3_LEVEL_STYLE_NOSCALE |
				I3_LEVEL_STYLE_NOROTATE_X | I3_LEVEL_STYLE_NOROTATE_Y | I3_LEVEL_STYLE_NOROTATE_Z);
}

i3LevelPath::~i3LevelPath(void)
{
	I3_SAFE_RELEASE( m_pPath);

	_RemoveAll();
}

i3LevelPathPoint * i3LevelPath::_CreatePointIcon(void)
{
	i3LevelPathPoint * pIcon;

	pIcon = i3LevelPathPoint::new_object();

	pIcon->Create();
	pIcon->SetOwner( this);

	m_PointList.push_back( pIcon);

	if( GetState() & I3_LEVEL_STATE_ATTACHED)
	{
		char conv[256];

		sprintf( conv, "%s_Point", GetName());

		g_pScene->AddElement( pIcon, conv);
	}

	return pIcon;
}

void i3LevelPath::_RemoveAll(void)
{
	i3LevelPathLink * pLink;
	i3LevelPathPoint * pIcon;

	// Points
	for( size_t i = 0; i < m_PointList.size(); i++)
	{
		pIcon = m_PointList[i];

		if( pIcon->GetState() & I3_LEVEL_STATE_ATTACHED)
		{
			g_pScene->RemoveElement( pIcon);	--i;	// 내부에서 m_PointList의 erase처리가 발생..(루틴개선이 필요할수 있음)
		}
		else
		{
			I3_MUST_RELEASE(pIcon);		// --> OnRemoveIcon과 중복되서 괜이 한번 더 지워짐...둘중 하나를 포기해야함...(2012.09.12.수빈)
		}
	}

	m_PointList.clear();

	// Lines
	for(size_t i = 0; i < m_LineList.size(); i++)
	{
		pLink = m_LineList[i];

		if( pLink->GetState() & I3_LEVEL_STATE_ATTACHED)
		{
			g_pScene->RemoveElement( pLink);	--i;	// 내부에서 m_LineList의 erase처리가 발생..(루틴개선이 필요할수 있음)
		}
		else
		{
			I3_MUST_RELEASE(pLink);	//--> OnRemoveIcon과 중복이 위와 비슷한 양상인듯 싶다..(2012.09.12.수빈)
		}
	}

	m_LineList.clear();
}

void i3LevelPath::AddPoint( VEC3D * pPos)
{
	m_pRes->addResState( I3LV_RES_STATE_MODIFIED);

	i3LevelPathPoint * pPoint = _CreatePointIcon();

	pPos->y += 0.25f;

	pPoint->setPos( pPos);
}

void i3LevelPath::BindRes( i3LevelRes * pRes)
{
	i3LevelElement3D::BindRes( pRes);

	if( i3::kind_of<i3LevelResPath*>(pRes))
	{
		i3LevelResPath * pPathRes = (i3LevelResPath *) pRes;

		pPathRes->OnValidate( g_pScene, false);

		m_pPath = pPathRes->getPathObject();
		I3_MUST_ADDREF(m_pPath);

		{
			i3LevelResSceneGraph * pSgRes = i3LevelScene::GetCommonPathResource();

			i3Node * pNode = i3Scene::CreateClone( pSgRes->GetSceneRoot(), I3_COPY_INSTANCE);

			SetSymbol( pNode);
		}
	}

	//_UpdateAll();
}

void i3LevelPath::_RemoveAllRelatedLine( i3LevelControl * pElm)
{
	
	i3LevelPathLink * pLink;

	for( size_t i = 0; i < m_LineList.size(); i++)
	{
		pLink = m_LineList[i];

		if( (pLink->GetStartElement() == pElm) || (pLink->GetEndElement() == pElm))
		{
			m_LineList.erase( m_LineList.begin() + i);

			g_pScene->RemoveElement( pLink);

			I3_MUST_RELEASE(pLink);
			i--;

			m_pRes->addResState( I3LV_RES_STATE_MODIFIED);
		}
	}
}

bool i3LevelPath::OnRemoveIcon( i3LevelControl * pElm)
{
	

	size_t idx = i3::vu::index_of( m_PointList, pElm);
	if( idx != m_PointList.size() )
	{
		{
			I3_MUST_RELEASE(pElm);

			m_PointList.erase(m_PointList.begin() + idx);

			m_pRes->addResState( I3LV_RES_STATE_MODIFIED);
		}

		_RemoveAllRelatedLine( pElm);

		//_UpdateAll();

		return true;
	}

	idx = i3::vu::index_of(m_LineList, pElm);
	if( idx != m_LineList.size() )
	{
		{
			I3_MUST_RELEASE(pElm);

			m_LineList.erase( m_LineList.begin() + idx);
		}

		m_pRes->addResState( I3LV_RES_STATE_MODIFIED);

		//_UpdateAll();
	}

	return true;
}

void i3LevelPath::_UpdateLink( i3LevelPathPoint * pPoint)
{
	
	for( size_t i = 0; i < m_LineList.size(); i++)
	{
		i3LevelPathLink * pLink = m_LineList[i];

		if(( pLink->GetStartElement() == pPoint) || (pLink->GetEndElement() == pPoint))
		{
			pLink->UpdateLink();
		}
	}
}

void i3LevelPath::OnMovingIcon( i3LevelControl * pElm)
{
	if( i3::same_of<i3LevelPathPoint* >(pElm))
	{
		_UpdateLink( (i3LevelPathPoint *) pElm);
	}
}

void i3LevelPath::OnScalingIcon( i3LevelControl * pElm)
{
	if( i3::same_of<i3LevelPathPoint* >(pElm))
	{
		_UpdateLink( (i3LevelPathPoint *) pElm);
	}
}

void i3LevelPath::OnRotatingIcon( i3LevelControl * pElm)
{
	if( i3::same_of<i3LevelPathPoint* >(pElm))
	{
		_UpdateLink( (i3LevelPathPoint *) pElm);
	}
}

void i3LevelPath::OnSelectedIcon( i3LevelControl * pElm)
{
	if( i3::same_of<i3LevelPathPoint*>(pElm))
	{
		_UpdateLink( (i3LevelPathPoint *) pElm);
	}
}


void i3LevelPath::AddLine( i3LevelPathPoint * pStart, i3LevelPathPoint * pEnd)
{
	i3LevelPathLink * pLink;

	pLink = i3LevelPathLink::new_object();

	pLink->SetOwner( this);
	pLink->SetStartElement( pStart);
	pLink->SetEndElement( pEnd);

	m_LineList.push_back( pLink);

	if( GetState() & I3_LEVEL_STATE_ATTACHED)
	{
		char conv[256];

		sprintf( conv, "%s_Link", GetName());

		g_pScene->AddElement( pLink, conv);
	}

	m_pRes->addResState( I3LV_RES_STATE_MODIFIED);
}

i3LevelPathLink * i3LevelPath::FindLinkByPoint( i3LevelPathPoint * pStart, i3LevelPathPoint * pEnd)
{
	i3LevelPathLink * pLink;

	for(size_t i = 0; i < m_LineList.size(); i++)
	{
		pLink = m_LineList[i];

		if( (pLink->GetStartElement() == pStart) && (pLink->GetEndElement() == pEnd))
			return pLink;

		if( (pLink->GetStartElement() == pEnd) && (pLink->GetEndElement() == pStart))
			return pLink;
	}

	return nullptr;
}

INT32 i3LevelPath::FindShortestPath( i3LevelPathPoint * pStart, i3LevelPathPoint * pEnd, i3LevelPathPoint ** pTable, INT32 maxLen)
{
	I3_POINTIDX idxStart, idxEnd;
	I3_POINTIDX * pIndexTable;
	INT32 i, cnt;

	pIndexTable = new I3_POINTIDX[ maxLen];

	_MakeGameData();

	idxStart	= (INT32) i3::vu::index_of(m_PointList, pStart);
	if (idxStart == (INT32)m_PointList.size() ) idxStart = -1;

	idxEnd		= (INT32) i3::vu::index_of(m_PointList, pEnd);
	if (idxEnd == (INT32)m_PointList.size()) idxEnd = -1;

	cnt = m_pPath->FindShortestPath( idxStart, idxEnd, pIndexTable, maxLen);

	for( i = 0; i < cnt; i++)
	{
		pTable[i] = m_PointList[ pIndexTable[i]];
	}

	delete [] pIndexTable;

	return cnt;
}

i3GameObj *	i3LevelPath::CreateGameInstance(void)
{
	I3_SAFE_ADDREF( m_pPath);

	if( m_pRes->getResState() & I3LV_RES_STATE_MODIFIED)
	{
		_MakeGameData();
	}

	return m_pPath;
}

void i3LevelPath::OnPrepareSerialize(void)
{
	_MakeGameData();

	m_pRes->addResState( I3LV_RES_STATE_MODIFIED);
}

void i3LevelPath::_MakeGameData(void)
{
	i3::vector<i3PathPoint*> pointList;
	i3::vector<i3PathLine*> linkList;


	for(size_t i = 0;  i < m_PointList.size(); i++)
	{
		i3PathPoint * pDestPoint = i3PathPoint::new_object();
		i3LevelPathPoint * pSrcPoint = m_PointList[i];

		pDestPoint->setPos( pSrcPoint->getPos());

		COLOR icol;
		COLORREAL rcol;

		pSrcPoint->GetColor( &icol);
		i3Color::Set( &rcol, &icol);
		pDestPoint->setColor( &rcol);

		pDestPoint->setPriority( pSrcPoint->getPriority());

		pointList.push_back( pDestPoint);
	}

	for(size_t i = 0; i < m_LineList.size(); i++)
	{
		i3PathLine * pDestLink = i3PathLine::new_object();
		i3LevelPathLink * pSrcLink = m_LineList[i];

		pDestLink->setCost( pSrcLink->getCost());
		pDestLink->setPathStyle( pSrcLink->getPathStyle());

		i3PathPoint * pStart, * pEnd;

		pStart = pEnd = nullptr;

		if( pSrcLink->GetStartElement() != nullptr)
		{
			size_t idx = i3::vu::index_of(m_PointList, pSrcLink->GetStartElement());
			I3ASSERT( idx != m_PointList.size() );

			pStart = pointList[idx];
		}

		if( pSrcLink->GetEndElement() != nullptr)
		{
			size_t idx = i3::vu::index_of(m_PointList, pSrcLink->GetEndElement());
			I3ASSERT( idx != m_PointList.size() );

			pEnd = pointList[idx];
		}

		pDestLink->setStart( pStart);
		pDestLink->setEnd( pEnd);

		linkList.push_back( pDestLink);
	}

	I3ASSERT( m_pPath != nullptr);
	m_pPath->Create( pointList, linkList);

//	pointList.SafeReleaseAll();
//	linkList.SafeReleaseAll();
	i3::for_each(pointList.begin(), pointList.end(), i3::fu::functor_safe(i3::mem_fun(&i3PathPoint::Release)));
	pointList.clear();
	i3::for_each(linkList.begin(), linkList.end(), i3::fu::functor_safe(i3::mem_fun(&i3PathLine::Release)));
	linkList.clear();
}

void	i3LevelPath::OnAttachScene( i3LevelScene * pScene)
{
	
	for(size_t i = 0; i < m_PointList.size(); i++)
	{
		i3LevelPathPoint * pIcon = m_PointList[i];

		pScene->AddElement( pIcon, GetName());
	}

	for(size_t i = 0; i < m_LineList.size(); i++)
	{
		i3LevelPathLink * pLink = m_LineList[i];

		pScene->AddElement( pLink, GetName());
	}

	i3LevelElement3D::OnAttachScene( pScene);
}

void	i3LevelPath::OnDetachScene( i3LevelScene * pScene)
{
	
	for(size_t i = 0; i < m_PointList.size(); i++)
	{
		i3LevelPathPoint * pIcon = m_PointList[i];

		pScene->RemoveElement( pIcon);
	}

	for(size_t i = 0; i < m_LineList.size(); i++)
	{
		i3LevelPathLink * pLink = m_LineList[i];

		pScene->RemoveElement( pLink);
	}

	i3LevelElement3D::OnDetachScene( pScene);
}

void i3LevelPath::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	for( size_t i = 0; i < m_PointList.size(); i++)
	{
		i3LevelPathPoint * pPoint = m_PointList[i];

		pPoint->OnBuildObjectList( List);
	}

	for( size_t i = 0; i < m_LineList.size(); i++)
	{
		i3LevelPathLink * pLink = m_LineList[i];

		pLink->OnBuildObjectList( List);
	}
	
	i3LevelElement3D::OnBuildObjectList( List);
}

void i3LevelPath::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
}

#if defined(I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct PATH
	{
		UINT8			m_ID[4] = { 'L', 'P', 'H', '1' };
		UINT32			pad[32] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	i3LevelPath::OnSave( i3ResourceFile * pFile)
{
	pack::PATH data;
	UINT32 rc, result;

	result = i3LevelElement3D::OnSave( pFile);
	I3_CHKIO( result);
	
	rc = pFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	rc = SaveFromListToResourceFile(m_PointList, pFile);	// m_PointList.SaveTo( pFile);
	I3_CHKIO( rc);
	result += rc;

	rc = SaveFromListToResourceFile(m_LineList, pFile);	//	m_LineList.SaveTo( pFile);
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3LevelPath::OnLoad( i3ResourceFile * pFile)
{
	pack::PATH data;
	UINT32 rc, result;
	
	result = i3LevelElement3D::OnLoad( pFile);
	I3_CHKIO( result);

	rc = pFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	rc = LoadFromResourceFileToList(m_PointList, pFile, true);	// m_PointList.LoadFrom( pFile, true);
	I3_CHKIO( rc);
	result += rc;

	for(size_t i = 0; i < m_PointList.size(); i++)
	{
		i3LevelPathPoint * pPoint = m_PointList[i];

		pPoint->Create();
		pPoint->SetOwner( this);
	}

	rc = LoadFromResourceFileToList(m_LineList, pFile, true);	//	m_LineList.LoadFrom( pFile, true);
	I3_CHKIO( rc);
	result += rc;

	for( size_t i = 0; i < m_LineList.size(); i++)
	{
		i3LevelPathLink * pLink = m_LineList[i];

		pLink->SetOwner(this);
	}

	return result;
}

bool i3LevelPath::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3LevelElement3D::OnSaveXML( pFile, pXML);

	SaveFromListToXML(m_PointList, "PointList", pFile, pXML, true);
	SaveFromListToXML(m_LineList, "LineList", pFile, pXML, true);

	return true;
}

bool i3LevelPath::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	
	i3LevelElement3D::OnLoadXML( pFile, pXML);

	LoadFromXMLToList(m_PointList, "PointList", pFile, pXML, true);
	LoadFromXMLToList(m_LineList, "LineList", pFile, pXML, true); 

	for(size_t i = 0; i < m_PointList.size(); i++)
	{
		i3LevelPathPoint * pPoint = m_PointList[i];

		pPoint->Create();
		pPoint->SetOwner( this);
	}

	for(size_t i = 0; i < m_LineList.size(); i++)
	{
		i3LevelPathLink * pLink = m_LineList[i];

		pLink->SetOwner( this);
	}

	return true;
}
