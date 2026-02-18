#if !defined( __CTRACER_BULLET_MANAGER_H)
#define __CTRACER_BULLET_MANAGER_H

#include "TracerBullet.h"

#define ENABLE_TRACER_BULLET		//	총알 궤적을 활성화합니다. komet


class CTracerBulletManager : public i3GameObj
{
	I3_CLASS_DEFINE( CTracerBulletManager, i3GameObj);

	i3InstanceManager * m_pInstanceManager = nullptr;
	i3::vector<CTracerBullet*>	m_StructContainer;

	i3AttrSet *			m_pEffectAttrSet = nullptr;
	i3Billboard	*		m_pBulletBillboard[TRACER_TYPE_MAX] = { nullptr, };

	bool				m_bActive;				// [initialize at constructor]
	INT32				m_nActiveCount = 0;

public:
	CTracerBulletManager( void);
	virtual ~CTracerBulletManager( void);

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	bool	Create( i3Node * pRenderRoot);
	void	Destroy( void);
	void	ResetAll( void);
	
	bool	Fire(TracerBulletInfo * pInfo);

	INT32	GetTotalCount( void)			{	return (INT32)m_StructContainer.size();		}
	INT32	GetActiveCount(void)			{	return m_nActiveCount;						}
	void	SetActive( bool bVal = true)	{	m_bActive = bVal;							}
	CTracerBullet * GetTracerBullet( INT32 Idx) const {	return m_StructContainer[Idx];	}	
};

#endif