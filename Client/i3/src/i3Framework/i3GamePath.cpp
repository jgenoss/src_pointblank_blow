#include "i3FrameworkPCH.h"
#include "i3GamePath.h"
#include "i3Base/itl/vector_util.h"


I3_CLASS_INSTANCE( i3GamePath);

///////////////////////////////////////////////////////////////////////////

i3GamePath::~i3GamePath( void)
{
	I3_SAFE_RELEASE( m_pBuff);

	m_pPoints = nullptr;
	m_PointCount = 0;

	m_pLinks = nullptr;
	m_LinkCount = 0;

	m_pIndexTable = nullptr;
	m_IndexCount = 0;
}

static INT32 _getLinkCount( i3PathPoint * pPoint, const i3::vector<i3PathLine*>& LinkList, INT32 * pIndexTable)
{
	INT32 cnt = 0;

	for(size_t i = 0; i < LinkList.size(); i++)
	{
		i3PathLine * pLine = LinkList[i];

		if( pLine->getPathStyle() & i3PathLine::STYLE_BIDIRECTION)
		{
			if( (pLine->getStart() == pPoint) || (pLine->getEnd() == pPoint))
			{
				if( pIndexTable != nullptr)
					pIndexTable[ cnt] = i;

				cnt++;
			}
		}
		else
		{
			if( pLine->getStart() == pPoint)
			{
				if( pIndexTable != nullptr)
					pIndexTable[ cnt] = i;

				cnt++;
			}
		}
	}

	return cnt;
}

void i3GamePath::Create( const i3::vector<i3PathPoint*>& PointList, const i3::vector<i3PathLine*>& LinkList)
{
	INT32 buffSz = 0;
	

	// 필요한 메모리의 크기를 구한다.
	{
		for(size_t i = 0; i < PointList.size(); i++)
		{
			i3PathPoint * pPoint = PointList[i];

			INT32 linkCount = _getLinkCount( pPoint, LinkList, nullptr);

			buffSz += sizeof(i3::pack::PATH_POINT_INFO) + (sizeof(INT32) * linkCount);
		}

		buffSz += sizeof(i3::pack::PATH_LINK_INFO) * LinkList.size();
	}

	if( buffSz <= 0)
		return;

	I3_SAFE_RELEASE( m_pBuff);

	m_pBuff = i3MemoryBuffer::new_object();

	m_pBuff->Create( buffSz);

	i3mem::FillZero( m_pBuff->getBuffer(), buffSz);

	Create( m_pBuff, (INT32)PointList.size(), (INT32)LinkList.size());

	// Point들의 값 설정
	INT32 accm = 0;

	for(INT32 i = 0; i < m_PointCount; i++)
	{
		i3PathPoint * pPoint = PointList[i];
		
		pPoint->SetTo( & m_pPoints[i]);

		m_pPoints[i].m_idxLink = accm;
		m_pPoints[i].m_cntLink = _getLinkCount( pPoint, LinkList, & m_pIndexTable[ accm]);

		accm += m_pPoints[i].m_cntLink;
	}

	// Link들의 값 설정.
	for(INT32 i = 0; i < m_LinkCount; i++)
	{
		i3PathLine * pLine = LinkList[i];

		pLine->SetTo( & m_pLinks[i]);

		i3PathPoint * p1, * p2;

		p1 = pLine->getStart();
		p2 = pLine->getEnd();
		
		size_t idxStart = i3::vu::index_of(PointList, pLine->getStart());
		size_t idxEnd   = i3::vu::index_of(PointList, pLine->getEnd());
		
		m_pLinks[i].m_idxStart	= (idxStart != PointList.size()) ? INT32(idxStart) : -1;
		m_pLinks[i].m_idxEnd	= (idxEnd != PointList.size()) ? INT32(idxEnd) : -1; 

		i3Vector::Sub( & m_pLinks[i].m_Dir, p2->getPos(), p1->getPos());
		i3Vector::Normalize( & m_pLinks[i].m_Dir, & m_pLinks[i].m_Dir);
	}
}

void i3GamePath::Create( i3MemoryBuffer * pBuffer, INT32 pointCount, INT32 linkCount)
{
	I3_REF_CHANGE( m_pBuff, pBuffer);

	// Point Array
	m_pPoints = (i3::pack::PATH_POINT_INFO *) m_pBuff->getBuffer();
	m_PointCount = pointCount;

	// Link Array
	m_pLinks = (i3::pack::PATH_LINK_INFO *) (m_pPoints + m_PointCount);
	m_LinkCount = linkCount;

	// Index Array
	m_pIndexTable = (INT32 *) (m_pLinks + m_LinkCount);

	m_IndexCount = m_pBuff->GetSize() - (sizeof(i3::pack::PATH_POINT_INFO) * m_PointCount) - (sizeof(i3::pack::PATH_LINK_INFO) * m_LinkCount);
	m_IndexCount = m_IndexCount / sizeof(INT32);
}

INT32	i3GamePath::FindClosestPoint( VEC3D * pPos)
{
	INT32 i, idx = -1;
	REAL32 minDist = 0.0f;
	i3::pack::PATH_POINT_INFO * pTemp;
	VEC3D diff;

	for( i = 0; i < m_PointCount; i++)
	{
		pTemp  = getPoint( i);

		i3Vector::Sub( &diff, pPos, &pTemp->m_Pos);

		REAL32 dist = i3Vector::LengthSq( &diff);

		if( idx == -1)
		{
			idx = i;
			minDist = dist;
		}
		else
		{
			if( minDist > dist)
			{
				minDist = dist;
				idx = i;
			}
		}
	}

	return idx;
}

INT32	i3GamePath::FindClosestPoints( VEC3D * pPos, INT32 * pidxArray, INT32 maxLength)
{
	if( maxLength > 30)
	{
		I3PRINTLOG(I3LOG_WARN,  "FindClosestPoints: maxLengths는 최대 30까지만 지원합니다");

		maxLength = 29;
	}

    REAL32	distArray[30];
	INT32 cnt = 0;

	INT32 i,j, tmpIdx = -1;
	REAL32 tmpDist;

	i3::pack::PATH_POINT_INFO * pTemp;
	VEC3D diff;

	i3mem::FillZero( distArray, sizeof( distArray));

	for( i = 0; i < m_PointCount; i++)
	{
		pTemp  = getPoint( i);

		i3Vector::Sub( &diff, pPos, &pTemp->m_Pos);

		REAL32 dist = i3Vector::LengthSq( &diff);

		if( cnt < maxLength)
		{
			distArray[cnt] = dist;
			pidxArray[cnt] = i;

			cnt++;
		}
		else
		{
			if( cnt > 0 && distArray[cnt-1] > dist)		// cnt > 0 추가.. maxLength가 0보다크면 괜찮겠지만. 0이라면..(2015.04.06.수빈)
			{
				distArray[cnt-1] = dist;
				pidxArray[cnt-1] = i;
			}
		}

		for( j = cnt -1; j > 0; --j)
		{
			if( distArray[j-1] > distArray[j])
			{
				tmpDist = distArray[j];
				tmpIdx	= pidxArray[j];

				distArray[j] = distArray[j-1];
				pidxArray[j] = pidxArray[j-1];

				distArray[j-1] = tmpDist;
				pidxArray[j-1] = tmpIdx;
			}
		}
	}

	return cnt;
}

I3_POINTIDX i3GamePath::FindClosestPointByPriority( VEC3D * pPos, INT32 limitPriority)
{
	INT32 i, idx = -1;
	REAL32 minDist = 0.0f;
	i3::pack::PATH_POINT_INFO * pTemp;
	VEC3D diff;

	for( i = 0; i < m_PointCount; i++)
	{
		pTemp  = getPoint( i);

		if( pTemp->m_Priority < limitPriority)
			continue;

		i3Vector::Sub( &diff, pPos, &pTemp->m_Pos);

		REAL32 dist = i3Vector::LengthSq( &diff);

		if( idx == -1)
		{
			idx = i;
			minDist = dist;
		}
		else
		{
			if( minDist > dist)
			{
				minDist = dist;
				idx = i;
			}
		}
	}

	return idx;
}

INT32 i3GamePath::GetLinkedPoint( I3_POINTIDX idxStart, I3_NEXTPOINTINFO * pArray, INT32 maxCount)
{
	INT32 i, cnt;
	i3::pack::PATH_POINT_INFO * pPoint;
	i3::pack::PATH_LINK_INFO * pLink;

	pPoint = getPoint( idxStart);

	cnt = min( pPoint->m_cntLink, maxCount);

	for( i = 0; i < cnt; i++)
	{
		INT32 idxLink = m_pIndexTable[ pPoint->m_idxLink + i];

		pLink = getLink( idxLink);

		pArray[i].m_idxLink = idxLink;

		if( pLink->m_Style & i3PathLine::STYLE_BIDIRECTION)
		{
			if( pLink->m_idxStart == idxStart)
				pArray[i].m_idxNext = pLink->m_idxEnd;
			else
				pArray[i].m_idxNext = pLink->m_idxStart;
		}
		else
		{
			pArray[i].m_idxNext = pLink->m_idxEnd;
		}
	}

	return cnt;
}

INT32 i3GamePath::_FindLinkedPoint( I3_POINTIDX idxStart, I3_NEXTPOINTINFO * pArray, INT32 maxCount)
{
	INT32 i, cnt, idxNext, idx = 0;
	i3::pack::PATH_POINT_INFO * pPoint;
	i3::pack::PATH_LINK_INFO * pLink;

	pPoint = getPoint( idxStart);

	I3ASSERT( pPoint->m_cntLink <= maxCount);

	cnt = min( pPoint->m_cntLink, maxCount);

	for( i = 0; i < cnt; i++)
	{
		INT32 idxLink = m_pIndexTable[ pPoint->m_idxLink + i];

		pLink = getLink( idxLink);

		if( pLink->m_Style & i3PathLine::STYLE_BIDIRECTION)
		{
			if( pLink->m_idxStart == idxStart)
				idxNext = pLink->m_idxEnd;
			else
				idxNext = pLink->m_idxStart;
		}
		else
		{
			idxNext = pLink->m_idxEnd;
		}

		i3::pack::PATH_POINT_INFO * pNext = getPoint( idxNext);

		if( pNext->m_idFind != m_FindID)
		{
			pArray[idx].m_idxNext = idxNext;
			pArray[idx].m_idxLink = idxLink;
			pArray[idx].m_dot = -1.0f;

			idx++;
		}
	}

	return idx;
}


bool i3GamePath::_Rec_FindShortestPath( I3_POINTIDX idxCur, I3_PATH_FIND_INFO * pInfo)
{
	VEC3D dir;
	I3_NEXTPOINTINFO	tblNext[ 20];
	//INT32 idx = 0;	// C4189
	INT32 i, j;
	bool bAdd;
	I3_POINTIDX idxNext;
	i3::pack::PATH_POINT_INFO * pCur;

	if( idxCur == pInfo->m_idxTarget)
		return true;

	pCur = getPoint( idxCur);

	pCur->m_idFind = m_FindID;


	// 방향
	i3Vector::Sub( &dir, &pInfo->m_pTarget->m_Pos, & pCur->m_Pos);
	i3Vector::Normalize( &dir, &dir);

	INT32 nextCount = _FindLinkedPoint( idxCur, tblNext, 20);

	// 진행 방향에 대한 dot 값을 저장
	for( i = 0; i < nextCount; i++)
	{
		VEC3D dir2;

		i3::pack::PATH_LINK_INFO * pLink = getLink( tblNext[i].m_idxLink);

		if( pLink->m_idxEnd == idxCur)
			i3Vector::Scale( &dir2, &pLink->m_Dir, -1.0f);
		else
			i3Vector::Copy( &dir2, &pLink->m_Dir);

		tblNext[i].m_dot = i3Vector::Dot( &dir, &dir2);
	}

	// 진행 방향에 가까운 순으로 Table을 Sorting한다.
	for( i = 0; i < nextCount - 1; i++)
	{
		for( j = i + 1; j < nextCount; j++)
		{
			if( tblNext[i].m_dot < tblNext[j].m_dot)
			{
				I3_POINTIDX idxTemp		= tblNext[j].m_idxNext;
				I3_LINKIDX	idxTemp2	= tblNext[j].m_idxLink;
				REAL32 tempDot			= tblNext[j].m_dot;

				tblNext[j].m_idxNext	= tblNext[i].m_idxNext;
				tblNext[j].m_idxLink	= tblNext[i].m_idxLink;
				tblNext[j].m_dot		= tblNext[i].m_dot;

				tblNext[i].m_idxNext	= idxTemp;
				tblNext[i].m_idxLink	= idxTemp2;
				tblNext[i].m_dot		= tempDot;
			}
		}
	}

	for( i = 0; i < nextCount; i++)
	{
		idxNext = tblNext[ i].m_idxNext;

		bAdd = false;

		if( pInfo->m_idxCur < pInfo->m_maxLen)
		{
			pInfo->m_pTable[ pInfo->m_idxCur] = idxNext;
			pInfo->m_idxCur++;

			bAdd = true;
		}

		bool bFind = _Rec_FindShortestPath( idxNext, pInfo);

		if( bFind)
		{
			return true;
		}

		// 못찾았다.
		if( bAdd)
		{
			pInfo->m_idxCur --;
		}
	}

	return false;
}

INT32 i3GamePath::FindShortestPath( I3_POINTIDX idxStart, I3_POINTIDX idxEnd, I3_POINTIDX * pTable, INT32 maxLen)
{
	I3_PATH_FIND_INFO info;
	INT32 Count = 0;

	info.m_idxCur = 1;
	pTable[0] = idxStart;

	info.m_maxLen = maxLen;
	info.m_pTable = pTable;

	info.m_idxTarget = idxEnd;
	info.m_pTarget = getPoint( idxEnd);

	m_FindID++;

	if( _Rec_FindShortestPath( idxStart, &info) == false)
	{
		return 0;
	}

	Count = info.m_idxCur;

	// 검색된 Path를 역으로 찾아가며 정리한다. A*
	{
		INT32 i, step, j;
		
		for( i = Count - 1; i >= 2;)
		{
			step = _RemoveRedundantPoint( i, pTable);

			if( step > 0)
			{
				for( j = 0; j < (Count - i); j++)
					pTable[ i - step + j] = pTable[ i + j];
			}

			i -= (step + 1);
			Count -= step;

			//I3TRACE( "SEP : %d, %d, (%d)\n", Count, i, step);
		}
	}

	return Count;
}

INT32 i3GamePath::_RemoveRedundantPoint( INT32 idx, I3_POINTIDX * pTable)
{
	INT32 i, j, cntLink, idxShort = -1;
	I3_NEXTPOINTINFO	links[ 40];
	i3::pack::PATH_LINK_INFO * pLink;
	I3_POINTIDX ptCur;
	
	ptCur = pTable[ idx];

	// 현재 Point와 연결된 Point들을 검색.
	cntLink = GetLinkedPoint( ptCur, links, sizeof(links) / sizeof(I3_NEXTPOINTINFO));

	for( i = 0; i < cntLink; i++)
	{
		// i - 2의 위치에서부터 0까지 link로 연결된 Point가 있다면
		// 직접 연결할 수 있다는 뜻.

		for( j = 0; j < (idx - 1); j++)
		{
			if( links[i].m_idxNext == pTable[j])
			{
				// 양방향 연결인지 확인해야 한다.
				pLink = getLink( links[i].m_idxLink);

				if( pLink->m_Style & i3PathLine::STYLE_BIDIRECTION)
				{
					if( idxShort == -1)
						idxShort = j;
					else if( idxShort > j)
						idxShort = j;
				}
			}
		}
	}

	if( idxShort != -1)
	{
		return idx - idxShort - 1;
	}

	return 0;
}

void	i3GamePath::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GamePath * pDestPath = (i3GamePath *)pDest;

	i3GameObj::CopyTo( pDest, method);

	// Path는 Instance를 별도로 만들 필요가 없다고 판단해
	// 항상 Referencing한다.

	pDestPath->Create( m_pBuff, getPointCount(), getLinkCount());
}

void i3GamePath::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pBuff != nullptr)
	{
		m_pBuff->OnBuildObjectList( List);
	}

	i3GameObj::OnBuildObjectList( List);
}

#if defined ( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct GAME_PATH
	{
		UINT8	m_ID[4] = { 'G', 'P', 'T', '1' };
		OBJREF	m_pBuff = 0;
		INT32	m_PointCount = 0;
		INT32	m_LinkCount = 0;
		UINT32	pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3GamePath::OnSave( i3ResourceFile * pResFile)
{
	UINT32 result, rc;
	pack::GAME_PATH	data;

	result = i3GameObj::OnSave( pResFile);
	I3_CHKIO( result);
	
	data.m_PointCount = getPointCount();
	data.m_LinkCount = getLinkCount();

	if( m_pBuff != nullptr)
		data.m_pBuff = pResFile->GetObjectPersistID( m_pBuff);

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32	i3GamePath::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 result, rc;
	pack::GAME_PATH	data;

	result = i3GameObj::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pBuff != 0)
	{
		i3MemoryBuffer * pBuff = (i3MemoryBuffer *) pResFile->FindObjectByID( data.m_pBuff);
		I3ASSERT( pBuff != nullptr);

		Create( pBuff, data.m_PointCount, data.m_LinkCount);
	}

	return result;
}

bool i3GamePath::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	I3ASSERT_0;

	return true;
}

bool i3GamePath::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	I3ASSERT_0;

	return true;
}

i3GamePath * i3GamePath::LoadFromFile( const char * pszPath)
{
	i3ResourceFile file;
	i3GamePath * pPath;

	if( file.Load( pszPath) == STREAM_ERR)
		return nullptr;

	pPath = (i3GamePath *) file.getKeyObject();
	if( pPath == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Loaded path data, but can not find i3GamePath object. : %s", pszPath);
		return nullptr;
	}

	I3ASSERT( i3::same_of< i3GamePath* >(pPath));

	I3_MUST_ADDREF( pPath);

	return pPath;
}

bool i3GamePath::SaveToFile( const char * pszPath, i3GamePath * pPath)
{
	i3ResourceFile file;

	file.setKeyObject( pPath);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not save path data : %s", pszPath);
		return false;
	}

	return true;
}
