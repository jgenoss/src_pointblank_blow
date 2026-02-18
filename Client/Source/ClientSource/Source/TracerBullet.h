#if !defined( __CTRACER_BULLET_H)
#define __CTRACER_BULLET_H

//#define DEBUG_TRACER_BULLET

enum TRACER_TYPE
{
	TRACER_TYPE_UNKNOWN		= -1,
	TRACER_TYPE_BULLET = 0,			//	일반 총알 궤적
	TRACER_TYPE_HAND_GRENADE,		//	수류탄 궤적
	TRACER_TYPE_SHELL_GRENADE,		//	유탄 궤적
	TRACER_TYPE_ARROW,				//	화살 궤적
	TRACER_TYPE_SMOKE,				//	연막탄 궤적
	TRACER_TYPE_FLASH,				//	섬관탄 궤적
	TRACER_TYPE_BULLET_SMOKE,		//	일반 총알 연기

	TRACER_TYPE_MAX
};

struct TracerBulletInfo
{
	TRACER_TYPE _Type = TRACER_TYPE_UNKNOWN;			//	타입 (모두 사용)
	UINT32 _Style = 0;	
	VEC3D _vBeginPos;			//	시작 위치 (모두 사용)
	VEC3D _vDestPos;			//	끝 위치 (모두 사용)
	I3COLOR _color;				//	텍스처 색상 (모두 사용)
	REAL32 _LifeTime = 0.0f;			//	생명 시간 (단위: 초) (TRACER_TYPE_BULLET은 사용 안함)
	REAL32 _Thickness = 0.0f;			//	두께 (단위: m) (모두 사용)
	REAL32 _GrowThickness = 0.0f;		//	알파가 사라짐에 따라 위아래로 퍼지는 두께 (단위: m)	(TRACER_TYPE_BULLET_SMOKE만 사용함)
	REAL32 _StartSpeed = 0.0f;			//	시작점 이동 속도 (총구) (단위: m/s)	(TRACER_TYPE_BULLET_SMOKE만 사용함)
	REAL32 _EndSpeed = 0.0f;			//	끝점 이동 속도 (타격점) (단위: m/s) (모두 사용)
	REAL32 _Length = 0.0f;				//	길이 (단위: m) (TRACER_TYPE_BULLET_SMOKE는 사용 안함)
	REAL32 _RepeatTexSize = 0.0f;		//	텍스처가 타일링 되는 길이 단위 (단위: m) (TRACER_TYPE_BULLET_SMOKE만 사용함)
};


class CTracerBullet : public i3GameObj
{
	I3_CLASS_DEFINE( CTracerBullet, i3GameObj);

protected:

	TracerBulletInfo	m_Info;

	INT32			m_nIdx = 0;			//	총알 인덱스
	INT32			m_nSrcAlpha = 255;	//	지정된 원래 알파값
	bool			m_bActive = false;		//	활성화 여부

	VEC3D			m_vDir;			//	방향
	VEC3D			m_vCurStart;	//	현재 날아가는 총알 시작 지점
	VEC3D			m_vCurEnd;	//	현재 날아가는 총알 끝 지점
	
	REAL32			m_fCurThickness = 0.0f;	//	두께
	REAL32			m_fDestLengthSq = 0.0f;
	REAL32			m_fCurLengthSq = 0.0f;
	REAL32			m_fGapLengthSq = 0.0f;
	REAL32			m_fCurTime = 0.0f;

	i3Billboard *	m_pBillboard = nullptr;

public:
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	bool	Create( INT32 Index);
	bool	Start( i3Billboard * pImage, TracerBulletInfo * pInfo);	

	void	SetColor( COLOR * pColor);
	void	SetColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a);
	void	SetAlpha(UINT8 a);
	void	Reset( bool bEnable = false);

	TracerBulletInfo * GetInfo(void)				{	return &m_Info;						}
	INT32			GetIdx( void)					{	return m_nIdx;						}
	VEC3D *			GetDir( void)					{	return &m_vDir;						}
	//	Total 총알이 날아가야하는 거리
	REAL32			GetDestLength( void)			{	return i3Math::Sqrt( m_fDestLengthSq);		}
	REAL32			GetCurLength( void);			//	시작점에서 현재까지 날아간 거리
	REAL32			GetRemainLength( void)			{	return (GetDestLength() - GetCurLength());	}
	
	bool			IsActive( void)					{	return m_bActive;					}
	//	총알이 도착했는지 여부
	bool			IsArrive( void)					
	{	
		switch( m_Info._Type)
		{
		case TRACER_TYPE_BULLET:		
			{
				return (m_fDestLengthSq <= m_fCurLengthSq);	
			}
			break;
		case TRACER_TYPE_HAND_GRENADE:
		case TRACER_TYPE_SHELL_GRENADE:
		case TRACER_TYPE_ARROW:
		case TRACER_TYPE_SMOKE:
		case TRACER_TYPE_FLASH:
		case TRACER_TYPE_BULLET_SMOKE:
			{
				return (m_nSrcAlpha <= 0);	
			}
			break;
		}	

		return false;	
	}

protected:
	void	OnActive( REAL32 tm);

};

#endif