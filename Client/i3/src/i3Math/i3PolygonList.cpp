#include "i3MathType.h"
#include "i3PolygonList.h"

I3_CLASS_INSTANCE( i3PolygonList);

i3PolygonList::i3PolygonList(void) 
{
	m_List.reserve(128);								// 이 부분은 reserve로 대체..
}

i3PolygonList::~i3PolygonList(void)
{
	reset( true);
}

void i3PolygonList::setCount( INT32 cnt)
{
	if( cnt > INT32(m_List.size()) )
	{
		INT32 i, oldCount;

		oldCount = m_List.size();

		m_List.resize( cnt);

		for( i = oldCount; i < cnt; i++)
		{
			i3Vector3List * pNew = i3Vector3List::new_object();

//			m_List.SetItem( i, pNew);
			m_List[i] = pNew;
		}
	}

	m_Count = cnt;
}

void i3PolygonList::setVectorCount( INT32 cnt)
{
	INT32 i;

	for( i = 0; i < getCount(); i++)
	{
		i3Vector3List * pPoly = getPolygon( i);

		pPoly->setCount( cnt);
	}
}

void i3PolygonList::reset( bool bFreeAll)
{

	m_Count = 0;

	for(size_t i = 0; i < m_List.size(); i++)
	{
		i3Vector3List * pPoly = getPolygon( i);

		pPoly->Reset( bFreeAll);

		if( bFreeAll)
		{
			I3_SAFE_RELEASE( pPoly);
		}
	}

	if( bFreeAll)
	{
		m_List.clear();
	}
}

void	i3PolygonList::removeEnd(void)
{
	m_Count--;
}

void i3PolygonList::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	I3ASSERT( i3::same_of<i3PolygonList*>(pDest));

	i3PolygonList * pList = (i3PolygonList *) pDest;
	INT32 i;
	i3Vector3List * pPoly;

	pList->setCount( getCount());

	for( i = 0; i < getCount(); i++)
	{
		pPoly = getPolygon( i);

		if( method == I3_COPY_REF)
		{
			pList->setPolygon( i, pPoly);
		}
		else
		{
			i3Vector3List * pNewPoly = pList->getPolygon( i);

			pPoly->CopyTo( pNewPoly, method);
		}
	}
}

#if defined( I3_DEBUG)
void i3PolygonList::vlog( bool bPlot, UINT8 r, UINT8 g, UINT8 b)
{
	INT32 i;
	i3Vector3List * pList;

	for( i = 0; i < getCount(); i++)
	{
		pList = getPolygon( i);
		pList->vlog( bPlot, r, g, b);
	}
}
#endif
