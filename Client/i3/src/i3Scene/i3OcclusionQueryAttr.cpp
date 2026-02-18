#include "i3SceneDef.h"
#include "i3OcclusionQueryAttr.h"

I3_CLASS_INSTANCE( i3OcclusionQueryAttr);

i3OcclusionQueryAttr::i3OcclusionQueryAttr(void)
{
	SetID( I3_ATTRID_OCCLUSIONQUERY);
}

i3OcclusionQueryAttr::~i3OcclusionQueryAttr(void)
{
	I3_SAFE_RELEASE( m_pQuery);
}

void i3OcclusionQueryAttr::Apply( i3RenderContext * pContext)
{
	if( m_pQuery == nullptr)
	{
		m_pQuery = i3GfxOcclusionTest::new_object();
		m_pQuery->Create();
	}

	switch( m_State)
	{
		case STATE_BEGIN :
			if((m_Phase == PHASE_1STCALL) && ( m_bWaitResult == false))
			{
				m_pQuery->Begin();
				m_bWaitResult = true;
				m_State = STATE_END;
			}
			else
			{
				m_State = STATE_WAITRESULT;
			}
			break;

		case STATE_END :
			I3ASSERT( m_Phase == PHASE_2NDCALL);

			m_pQuery->End();
			m_State = STATE_WAITRESULT;
			break;

		case STATE_WAITRESULT:
			if( m_pQuery->Update())
			{
				m_bWaitResult = false;

				m_bValidResult = true;
				m_Result = m_pQuery->getDrawnPixelCount();

				//I3TRACE( "OCC : %d\n", m_Result);
			}

			m_State = STATE_BEGIN;
			break;
	}

	(m_Phase == PHASE_1STCALL) ? (m_Phase = PHASE_2NDCALL) : (m_Phase = PHASE_1STCALL);
}

#if defined( I3_DEBUG)
void i3OcclusionQueryAttr::Dump(void)
{
	i3RenderAttr::Dump();
}
#endif
