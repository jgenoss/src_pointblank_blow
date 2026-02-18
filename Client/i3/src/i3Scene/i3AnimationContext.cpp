#include "i3SceneDef.h"
#include "i3AnimationContext.h"

#include "i3Skeleton.h"

I3_CLASS_INSTANCE( i3AnimationContext);

i3AnimationContext::~i3AnimationContext(void)
{
	I3_SAFE_RELEASE( m_pCacheMatrixArray);
	_RemoveAllCombiner();
	m_pFrameSkeleton = nullptr;
}

void i3AnimationContext::Create( i3Skeleton * pSkel)
{
	I3ASSERT( pSkel != nullptr);

	setFrameSkeleton( pSkel);

	I3_SAFE_RELEASE( m_pCacheMatrixArray);

	m_pCacheMatrixArray = i3MatrixArray::new_object();
	m_pCacheMatrixArray->Create( pSkel->getBoneCount());

	_RemoveAllCombiner();
	_PrepareCombiner( pSkel->getBoneCount());
}

void i3AnimationContext::setLocalTime( REAL32 tm)
{
	
	i3TransformSourceCombiner * pCombiner;

	m_timeLocal = tm;

	for(size_t i = 0; i < m_CombinerList.size(); i++)
	{
		pCombiner = m_CombinerList[i];

		pCombiner->SetPlayTime( tm);
	}
}

void i3AnimationContext::_RemoveAllCombiner(void)
{
	
	i3TransformSourceCombiner * pCombiner;

	for(size_t i = 0; i < m_CombinerList.size(); i++)
	{
		pCombiner = m_CombinerList[i];

		I3_MUST_RELEASE( pCombiner);
	}

	m_CombinerList.clear();
}

void i3AnimationContext::_PrepareCombiner( INT32 count)
{
	INT32 i;

	for( i = 0; i < count; i++)
	{
		i3TransformSourceCombiner * pCombiner;

		pCombiner = i3TransformSourceCombiner::new_object();

		m_CombinerList.push_back( pCombiner);
	}
}

void i3AnimationContext::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	I3ASSERT( i3::same_of<i3AnimationContext*>(pDest));

	i3AnimationContext * pCtx = (i3AnimationContext *) pDest;

	if( m_pCacheMatrixArray != nullptr)
	{
		pCtx->Create( m_pFrameSkeleton);
	}
}

const char * i3AnimationContext::GetBoneName( INT32 idx) const
{
	I3ASSERT( m_pFrameSkeleton != nullptr);
	return m_pFrameSkeleton->getName( idx);
}

const i3::rc_string&	i3AnimationContext::GetBoneNameString(INT32 idx) const
{
	I3ASSERT( m_pFrameSkeleton != nullptr);
	return m_pFrameSkeleton->getNameString( idx);
}
