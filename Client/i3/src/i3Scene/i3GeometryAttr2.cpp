#include "i3SceneDef.h"
#include "i3GeometryAttr2.h"

I3_CLASS_INSTANCE( i3GeometryAttr2);

i3GeometryAttr2::~i3GeometryAttr2( void)
{
	_removeSplitData();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// local function
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool i3GeometryAttr2::_refreshLOD()
{
	return false;
}

void i3GeometryAttr2::_collapse()
{
 	if( m_pSplitData == nullptr)
		return;

	INT32 maxCollapse = INT32(m_pSplitData->size());
	if( m_curSplitIndex >= maxCollapse)
		return;

	bool	bUseTex = false;
	if( m_pVA->GetTextureCoordCount() > 0)
		bUseTex = true;

	I3GEOM_VSPLIT * pSplit = (*m_pSplitData)[m_curSplitIndex];

	m_pVA->SetPosition( pSplit->vOldID, &pSplit->vf);

	if( bUseTex == true)	m_pVA->SetTextureCoord( 0, pSplit->vOldID, &pSplit->uvf);

	//I3TRACE("delface: %d, cmV: %d, err: %f, %d->%d\n", pSplit->nFaceCount, pSplit->nSplitCount, pSplit->err, pSplit->vOldID, pSplit->vNewID);
	// id를 새로운 위치로 변경한다.
	for( INT32 i = 0;i < pSplit->nSplitCount; ++i)
	{
		INT32 id = *(pSplit->iIndicId+i);

		m_pIA->SetIndex( id, static_cast<UINT16>(pSplit->vNewID));

		m_pVA->SetPosition( *(pSplit->iOldID+i), &pSplit->vf);
		//if( bUseTex == true)			m_pVA->SetTextureCoord( 0, *(pSplit->iOldID+i), &pSplit->uvf);
	}

	for( INT32 i = 0;i < pSplit->nNewCount; ++i)
	{
		m_pVA->SetPosition( *(pSplit->iNewID+i), &pSplit->vf);
		//if( bUseTex == true)			m_pVA->SetTextureCoord( 0, *(pSplit->iNewID+i), &pSplit->uvf);
	}

	//없어지는 페이스들을 정리하고 한칸씩 밀어준다.
	for( INT32 i = 0;i < pSplit->nFaceCount; i++)
	{
		INT32 startidx = *(pSplit->iFaceID+i*4) - i;
		INT32 endidx = m_PrimCount - i;

		if( pSplit->nFaceCount > i+1)
		{
			endidx = *(pSplit->iFaceID+(i+1)*4) - i;
		}
		
		for( INT32 j = startidx*3; j < endidx*3; j++)
		{
			m_pIA->SetIndex( j, m_pIA->GetIndex( j+ (i+1)*3));
		}
	}

	m_PrimCount = m_PrimCount - pSplit->nFaceCount;
	m_curSplitIndex++;
}

void i3GeometryAttr2::_split()
{
	if( m_pSplitData == nullptr)
		return;

	if( m_curSplitIndex <= 0)
		return;

	INT32 i;
	bool	bUseTex = false;
	if( m_pVA->GetTextureCoordCount() > 0)
		bUseTex = true;

	I3GEOM_VSPLIT * pSplit = (*m_pSplitData)[ m_curSplitIndex - 1];

	m_pVA->SetPosition( pSplit->vNewID, &pSplit->v0);
	if( bUseTex)	m_pVA->SetTextureCoord( 0, pSplit->vNewID, &pSplit->uv0);

	m_pVA->SetPosition( pSplit->vOldID, &pSplit->v1);
	if( bUseTex)	m_pVA->SetTextureCoord( 0, pSplit->vOldID, &pSplit->uv1);

	INT32 last = m_PrimCount + pSplit->nFaceCount - 1;
	//I3TRACE("delface: %d, cmV: %d, err: %f, %d->%d\n", pSplit->nFaceCount, pSplit->nSplitCount, pSplit->err, pSplit->vOldID, pSplit->vNewID);
	//없어지는 페이스들을 정리하고 한칸씩 밀어준다.
	for( i = pSplit->nFaceCount - 1;i >= 0 ; i--)
	{
		INT32 startidx = *(pSplit->iFaceID+i*4);
		INT32 endidx = last;
		
		for( INT32 j = endidx*3; j > startidx*3 ; j -= 3)
		{
			m_pIA->SetIndex( j, m_pIA->GetIndex( j-((i+1)*3)));
			m_pIA->SetIndex( j+1, m_pIA->GetIndex( j-((i+1)*3)+1));
			m_pIA->SetIndex( j+2, m_pIA->GetIndex( j-((i+1)*3)+2));
		}

		m_pIA->SetIndex( startidx*3,	(*(pSplit->iFaceID+i*4+1)));
		m_pIA->SetIndex( startidx*3+1,	(*(pSplit->iFaceID+i*4+2)));
		m_pIA->SetIndex( startidx*3+2,	(*(pSplit->iFaceID+i*4+3)));

		last = startidx - 1;
	}

	for( i = 0;i < pSplit->nNewCount; ++i)
	{
		m_pVA->SetPosition( *(pSplit->iNewID+i), &pSplit->v0);

		//if( bUseTex )	m_pVA->SetTextureCoord( 0, *(pSplit->iNewID+i), &pSplit->uv0);
	}

	// id를 이전 위치로 돌린다.
	for( i = 0;i < pSplit->nSplitCount; ++i)
	{
		INT32 id = *(pSplit->iIndicId+i);

		m_pIA->SetIndex( id, static_cast<UINT16>(*(pSplit->iOldID +i)));

		m_pVA->SetPosition( *(pSplit->iOldID +i), &pSplit->v1);

		//if( bUseTex )	m_pVA->SetTextureCoord( 0, *(pSplit->iOldID+i), &pSplit->uv1);
	}

	m_PrimCount = m_PrimCount + pSplit->nFaceCount;
	m_curSplitIndex--;
}

void i3GeometryAttr2::_removeSplitData(void)
{
	if( m_pSplitData == nullptr)	return;

	for(size_t i = 0;i < m_pSplitData->size(); ++i)
	{
		I3GEOM_VSPLIT * pSplit = (*m_pSplitData)[i];

		I3MEM_SAFE_FREE( pSplit->iFaceID);
		I3MEM_SAFE_FREE( pSplit->iIndicId);
		I3MEM_SAFE_FREE( pSplit->iNewID);
		I3MEM_SAFE_FREE( pSplit->iOldID);
		I3MEM_SAFE_FREE( pSplit);
	}
	
	m_pSplitData->clear();
	m_pSplitData.reset();
//	I3_SAFE_RELEASE( m_pSplitData);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// global function
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3GeometryAttr2::SetSplitData( const i3::shared_ptr< i3::vector< I3GEOM_VSPLIT*> >& pSplitData)
{
	_removeSplitData();

	m_pSplitData = pSplitData;
}

void i3GeometryAttr2::SetDetailLevel( REAL32 rate)
{

}

REAL32 i3GeometryAttr2::GetDetailLevel( void)
{
	return 0.0f;
}

void i3GeometryAttr2::Collapse(bool oneStep, bool bLast)
{
	m_pVA->Lock();
	m_pIA->Lock();

	if( oneStep)	_collapse();
	else
	{
		INT32 maxCollapse = INT32(m_pSplitData->size());
		if( bLast )
		{
			while( m_curSplitIndex < maxCollapse)		_collapse();
		}
		else
		{
			REAL32 rate = (REAL32)m_curSplitIndex/maxCollapse;
			if( rate < 1.0f)
			{
				INT32 targetcount = (UINT32)(MIN( (rate + 0.05f) * maxCollapse, maxCollapse));
				if( m_curSplitIndex < targetcount)
				{
					while( m_curSplitIndex < targetcount)		_collapse();
				}
				else
				{
					_collapse();
				}
			}
		}
	}

	m_pVA->Unlock();
	m_pIA->Unlock();

	SetVertexArray( m_pVA, I3G_PRIM_TRILIST, m_PrimCount, 0, false);
	if( m_PrimCount > 0)
	{
		OptimizeForSkin();
	}
}
	
void i3GeometryAttr2::Split(bool oneStep, bool bFirst)
{
	m_pVA->Lock();
	m_pIA->Lock();

	if( oneStep)	_split();
	else
	{
		INT32 maxCollapse = INT32(m_pSplitData->size());
		if( bFirst )
		{
			while( m_curSplitIndex > 0)		_split();
		}
		else
		{
			REAL32 rate = (REAL32)m_curSplitIndex/maxCollapse;
			if( rate > 0.0f)
			{
				INT32 targetcount = (UINT32)(MAX( (rate - 0.05f) * maxCollapse, 0));
				if( m_curSplitIndex > targetcount)
				{
					while( m_curSplitIndex > targetcount)		_split();
				}
				else
				{
					_split();
				}
			}
		}
	}
	m_pVA->Unlock();
	m_pIA->Unlock();

	SetVertexArray( m_pVA, I3G_PRIM_TRILIST, m_PrimCount, 0, false);
	OptimizeForSkin();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// common virtual function
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3GeometryAttr2::Apply( i3RenderContext * pContext)
{
	i3GeometryAttr::Apply( pContext);
}

void i3GeometryAttr2::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3GeometryAttr::CopyTo( pObj, method);

}

void i3GeometryAttr2::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3GeometryAttr::OnBuildObjectList( List);
}

UINT32 i3GeometryAttr2::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result = 0;

	Result = i3GeometryAttr::OnSave( pResFile);
	I3_CHKIO( Result);

	return Result;
}

UINT32  i3GeometryAttr2::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result = 0;

	Result = i3GeometryAttr::OnLoad( pResFile);
	I3_CHKIO( Result);

	return Result;
}
