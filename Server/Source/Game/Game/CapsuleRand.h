#ifndef __CAPSULERAND_H__
#define __CAPSULERAND_H__

// 2011.04.22 안국정
// boost random library를 사용합니다.
#include "boost/random.hpp"
//#include "D:/Project_V1.5/PointBlank/Source_New_Dev/GameServerSource/boost/random.hpp"
//#include "D:/Project_V1.5/i3Server/include/boost/random.hpp"

// 26가지의 랜덤엔진 중 한개만 디파인 설정해주시면 됩니다.
// 랜덤엔진들 성능 테스트 결과가 첨부된 주소는 아래와 같습니다.
// http://deki.zepetto.com/Projects/Point_Blank/Game_Design/KnowHow/Manual/%eb%bd%91%ea%b8%b0_%ec%95%84%ec%9d%b4%ed%85%9c_%ed%88%b4_%ec%82%ac%ec%9a%a9%eb%b2%95%ea%b3%bc_%eb%bd%91%ea%b8%b0_%ea%b5%ac%ec%84%b1_%eb%b0%a9%ec%8b%9d_%eb%b3%80%ea%b2%bd_%ec%82%ac%ed%95%ad

//#define	BOOST_RANDOM_ENGINE		minstd_rand0
//#define	BOOST_RANDOM_ENGINE		minstd_rand
//#define	BOOST_RANDOM_ENGINE		rand48
//#define	BOOST_RANDOM_ENGINE		ecuyer1988
#define	BOOST_RANDOM_ENGINE		kreutzer1986
//#define	BOOST_RANDOM_ENGINE		taus88
//#define	BOOST_RANDOM_ENGINE		hellekalek1995
//#define	BOOST_RANDOM_ENGINE		mt11213b
//#define	BOOST_RANDOM_ENGINE		mt19937
//#define	BOOST_RANDOM_ENGINE		lagged_fibonacci607
//#define	BOOST_RANDOM_ENGINE		lagged_fibonacci1279
//#define	BOOST_RANDOM_ENGINE		lagged_fibonacci2281
//#define	BOOST_RANDOM_ENGINE		lagged_fibonacci3217
//#define	BOOST_RANDOM_ENGINE		lagged_fibonacci4423
//#define	BOOST_RANDOM_ENGINE		lagged_fibonacci9689
//#define	BOOST_RANDOM_ENGINE		lagged_fibonacci19937
//#define	BOOST_RANDOM_ENGINE		lagged_fibonacci23209
//#define	BOOST_RANDOM_ENGINE		lagged_fibonacci44497
//#define	BOOST_RANDOM_ENGINE		ranlux3
//#define	BOOST_RANDOM_ENGINE		ranlux4
//#define	BOOST_RANDOM_ENGINE		ranlux64_3
//#define	BOOST_RANDOM_ENGINE		ranlux64_4
//#define	BOOST_RANDOM_ENGINE		ranlux3_01
//#define	BOOST_RANDOM_ENGINE		ranlux4_01
//#define	BOOST_RANDOM_ENGINE		ranlux64_3_01
//#define	BOOST_RANDOM_ENGINE		ranlux64_4_01

class CCapsuleRand : public i3ElementBase
{
	I3_CLASS_DEFINE(CCapsuleRand);

	INT32		m_i32CountMax;
	UINT8*		m_ui8Capsule;

public:
	CCapsuleRand();
	CCapsuleRand( INT32 i32Count );
	virtual ~CCapsuleRand();

	BOOL		Create( INT32 i32Count );
	void		Insert( INT32 iCount, UINT8 ui8Value );
	void		ShuffleMix();
	UINT8		Rand();

	INT32		GetCountMax()				{	return m_i32CountMax;	};
};

#endif