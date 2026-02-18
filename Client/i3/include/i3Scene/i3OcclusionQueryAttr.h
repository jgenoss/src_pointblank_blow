#if !defined( __I3_OCCLUSION_QUERY_ATTR_H)
#define __I3_OCCLUSION_QUERY_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3OcclusionQueryAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3OcclusionQueryAttr, i3RenderAttr);

public:
	enum STATE
	{
		STATE_BEGIN,
		STATE_END,
		STATE_WAITRESULT,
	};

	enum PHASE
	{
		PHASE_1STCALL,
		PHASE_2NDCALL,
	};

protected:
	i3GfxOcclusionTest *	m_pQuery = nullptr;
	STATE					m_State = STATE_BEGIN;
	bool					m_bWaitResult = false;
	bool					m_bValidResult = false;
	INT32					m_Result = 0;
	PHASE					m_Phase = PHASE_1STCALL;

public:
	i3OcclusionQueryAttr(void);
	virtual ~i3OcclusionQueryAttr(void);

	bool		isReady(void)			{ return m_bValidResult; }
	INT32		getResult(void)			{ return m_Result; }

	virtual void Apply( i3RenderContext * pContext) override;
	//virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	//virtual BOOL IsSame( i3RenderAttr * pAttr);

	//virtual UINT32	OnSave( i3ResourceFile * pResFile);
	//virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	#if defined( I3_DEBUG)
	virtual void Dump(void) override;
	#endif
};

#endif
