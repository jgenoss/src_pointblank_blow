#include "i3SceneDef.h"
#include "i3RenderAttrListAttr.h"
#include "i3GeometryAttr.h"
#include "i3ModelViewMatrixAttr.h"
#include "i3BlendModeAttr.h"

I3_CLASS_INSTANCE( i3RenderAttrListAttr);

i3RenderAttrListAttr::i3RenderAttrListAttr(void)
{
	SetID( I3_ATTRID_RENDER_ATTR_LIST);

	m_List.reserve(128);
}

i3RenderAttrListAttr::~i3RenderAttrListAttr(void)
{
	Reset();
}

void i3RenderAttrListAttr::AppendAttr( i3RenderAttr * pAttr)
{
	m_List.push_back( pAttr);

	I3_SAFE_ADDREF( pAttr);
}

//
// 아래 함수는 현재 엔진 수준에서 볼때 말이 안된다...아마도 구버전 구현을 그냥 남겨놓은듯 하고...
// 실제로 호출되지는 않는 모양..
//

INT32 i3RenderAttrListAttr::_FindInsertPoint( REAL32 dist)
{
	INT32 idx, startidx, endidx;
	i3RenderAttrListAttr * pAttr;

	if( m_List.size() == 0)
		return -1;

	pAttr = (i3RenderAttrListAttr *) m_List[0];
	I3ASSERT( i3::same_of<i3RenderAttrListAttr*>(pAttr));

	if( dist < pAttr->getDist())
		return 0;

	pAttr = (i3RenderAttrListAttr *) m_List[m_List.size() - 1];
	I3ASSERT( i3::same_of<i3RenderAttrListAttr*>(pAttr));

	if( dist >= pAttr->getDist())
		return -1;

	startidx = 0;
	endidx = INT32(m_List.size()) - 1;
	idx = (startidx + endidx) >> 1;

	do 
	{
		pAttr = (i3RenderAttrListAttr *) m_List[idx];
		I3ASSERT( i3::same_of<i3RenderAttrListAttr*>(pAttr));

		if( dist < pAttr->getDist()) 
			endidx = idx - 1;
		else 
			startidx = idx + 1;	
		
		idx = (startidx + endidx) >> 1;
	} while( startidx < endidx);

	pAttr = (i3RenderAttrListAttr *) m_List[idx];
	I3ASSERT( i3::same_of<i3RenderAttrListAttr*>(pAttr));

	if( dist > pAttr->getDist())
		idx++;

	return idx;
}

void i3RenderAttrListAttr::AddGroup( i3RenderAttrListAttr * pAttrList)
{
	INT32 idx = _FindInsertPoint( pAttrList->getDist());

	m_List.insert(m_List.begin() + idx, pAttrList);
	
	I3_SAFE_ADDREF( pAttrList);

	if( pAttrList->getDist() < m_Dist)
		m_Dist = pAttrList->getDist();
}

void i3RenderAttrListAttr::Reset(void)
{
	
	i3RenderAttr * pAttr;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pAttr = Get( i);

		I3_MUST_RELEASE( pAttr);
	}

	m_List.resize( 0);
	m_Dist = 1.0f;
}

void i3RenderAttrListAttr::Apply( i3RenderContext * pContext)
{
	i3RenderAttr * pAttr;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pAttr = Get( i);

		//__SP( pAttr->GetID());
		pAttr->Apply( pContext);
		//__EP( pAttr->GetID());
	}

	//I3TRACE( "--------------------\n");
}

void i3RenderAttrListAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	// i3RenderAttrListAttr은 파일에 저장될 수 있는 Attribute가 아니며,
	// 복사하는 것이 의미가 없는 Attribute이기 때문에
	// 아무런 처리를 하지 않는다.
}

void i3RenderAttrListAttr::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	
	i3RenderAttr * pAttr;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pAttr = Get( i);
		
		List.push_back( pAttr);
	}

	i3RenderAttr::OnBuildObjectList( List);
}

UINT32 i3RenderAttrListAttr::OnSave( i3ResourceFile * pResFile)
{
	
	UINT32 val, Rc, Result = 0;

	val = m_List.size();

	Rc = pResFile->GetStream()->Write( &val, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3RenderAttrListAttr::OnSave()", "Could not write attribues count.");
		return STREAM_ERR;
	}
	Result += Rc;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		OBJREF ref = pResFile->GetObjectPersistID( Get( i));

		Rc = pResFile->GetStream()->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3RenderAttrListAttr::OnSave()", "Could not write attribute ID.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3RenderAttrListAttr::OnLoad( i3ResourceFile * pResFile)
{
	INT32 i;
	UINT32 val, count, Rc, Result = 0;
	i3RenderAttr * pAttr;

	Rc = pResFile->GetStream()->Read( &count, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3RenderAttrListAttr::OnLoad()", "Could not read attributes count.");
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0; i < (INT32) count; i++)
	{
		Rc = pResFile->GetStream()->Read( &val, sizeof(UINT32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3RenderAttrListAttr::OnLoad()", "Could not read attribute ID.");
			return STREAM_ERR;
		}
		Result += Rc;

		pAttr = (i3RenderAttr *) pResFile->FindObjectByID( val);
		I3ASSERT( pAttr != nullptr);

		m_List.push_back( pAttr);
	}

	return Result;
}

#if defined( I3_DEBUG)
void i3RenderAttrListAttr::Dump(void)
{
	i3RenderAttr * pAttr;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		pAttr = m_List[i];

		pAttr->Dump();
	}
}
#endif
