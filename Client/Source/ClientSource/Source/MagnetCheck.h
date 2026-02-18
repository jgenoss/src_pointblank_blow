#ifndef		__MAGNETCHECK_H__
#define		__MAGNETCHECK_H__


//----------------------------------------------------------//
//Hack Protect//
//----------------------------------------------------------//
class CMagnetCheck
{
	enum
	{
		CHARA_POS_CHECK_TIME = 1,		//1초
		CHARA_POS_CHECK_RANGE = 10,		//10cm
		CHARA_POS_CHECK_START_TIME = 30 //게임 시작 30초 후 부터 검사
	};

	struct COMPARE_POS_INFO
	{
		INT32			idx;
		INT32			idx2;

		VEC3D			pos;
		VEC3D			pos2;

		COMPARE_POS_INFO()
		{
			idx = idx2 = -1;
			ZeroMemory( &pos, sizeof(VEC3D) );
			ZeroMemory( &pos2, sizeof(VEC3D) );
		}
	};
public:
	CMagnetCheck();
	~CMagnetCheck();

	const i3::vector<CGameCharaBase*>* 	m_pCharaList;
	
	COMPARE_POS_INFO		m_Compare;

	bool					m_bSendInvalidSpeed;
	bool					m_bDoubtPos;
	bool					m_bStartDetecting;
	bool					m_bIsInit;

	REAL32					m_fTime;
	REAL32					m_fWaittingTime;
	
	void					Init(const i3::vector<CGameCharaBase*>& CharaList );
	void					Release();
	void					Update( REAL32 fElapesedTime );

	inline void				InitStartTime() { m_bStartDetecting = false; m_fWaittingTime = 0.f; }
private:
	bool					InRange( const VEC3D* v1, const VEC3D* v2 );

	void					Detect();
	bool					Comfirm();
};


#endif