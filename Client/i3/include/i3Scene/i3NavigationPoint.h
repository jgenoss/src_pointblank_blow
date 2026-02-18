#ifndef __I3_NAVIGATION_POINT_H__
#define __I3_NAVIGATION_POINT_H__

#include "i3Node.h"

#define I3_MAX_NVPOINT_COUNT	0xFF	// 포인트 최대 개수. 255개 (0 ~ 254 )
#define	I3_NVPOINT_INDEX_NONE	0xFF	// 존재하지 않는 삼각형 인덱스를 나타냄. (255)
#define I3_MAX_NVPOINT_NEIGHBOR	0xF		// 이웃 최대 개수. 15개 ( 0 ~ 14 )
#define	I3_NVPOINT_DIR_NONE		0xF		// 존재하지 않는 길찾기 방향을 나타냄. 15

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace i3
{
	namespace pack
	{
		struct PACKED NAVPOINT_INFO
		{
			VEC2D	vecPos;
			UINT8	nNeighbor[I3_MAX_NVPOINT_NEIGHBOR] = { 0 };
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class I3_EXPORT_SCENE i3NavigationPoint : public i3Node
{
	I3_EXPORT_CLASS_DEFINE( i3NavigationPoint, i3Node );
protected:
	i3::pack::NAVPOINT_INFO	*m_pPointInfo = nullptr;
	UINT8				m_nCount = 0;
    UINT8				*m_pNavInfo = nullptr;
	INT32				m_NavInfoSize = 0;

public:
	virtual ~i3NavigationPoint();

	UINT8				GetCount(void)					{ return m_nCount; }
	void				SetCount( UINT8 nCount );

	i3::pack::VEC2D *	GetPosition( UINT8 idx)			{ return &m_pPointInfo[idx].vecPos; }

	i3::pack::NAVPOINT_INFO *	GetPointArray(void)				{ return m_pPointInfo; }
	i3::pack::NAVPOINT_INFO *	GetPoint( UINT8 idx )			{ return m_pPointInfo + idx; }

	// 길찾기 정보 설정
	void			SetPathDir( UINT8 nStart, UINT32 nDest, UINT8 nDir );

	// 길찾기 정보 얻기
	UINT8			GetPathDir( UINT8 nStart, UINT32 nDest );

	// 이웃의 방향 인덱스 구하기
	UINT8			GetNeighborDirFromTriIndex( UINT8 nIndex, UINT8 nFind )
	{
		UINT8 i;
		for( i = 0; i < I3_MAX_NVPOINT_NEIGHBOR; i++ )
		{
			if( m_pPointInfo[ nIndex ].nNeighbor[ i ] == nFind )
			{
				return i;
			}
		}
		return I3_NVPOINT_DIR_NONE;
	}

	// 이웃한 메쉬인지 여부
	bool			IsNeighbor( UINT8 nIndex, UINT8 nNeighbor )
	{
		I3ASSERT( nIndex != I3_NVPOINT_INDEX_NONE );
		I3ASSERT( nNeighbor != I3_NVPOINT_INDEX_NONE );
		UINT8 i;
		for( i = 0; i < I3_MAX_NVPOINT_NEIGHBOR; i ++ )
		{
			if( m_pPointInfo[ nIndex ].nNeighbor[ i ] == nNeighbor )
			{
				return true;
			}
		}
		return false;
	}

	UINT8			GetNeighborIndex( UINT8 nIndex, UINT8 nCount )
	{
		//I3ASSERT( m_pPointInfo != nullptr && nIndex < m_nCount );
		//I3ASSERT( 0 <= nCount && nCount < I3_MAX_NVPOINT_NEIGHBOR );
		return m_pPointInfo[ nIndex ].nNeighbor[ nCount ];
	}

	i3::pack::NAVPOINT_INFO*	GetNeighbor( UINT8 nIndex, UINT8 nCount )
	{
		//I3ASSERT( m_pPointInfo != nullptr && nIndex < m_nCount );
		//I3ASSERT( 0 <= nCount && nCount < I3_MAX_NVPOINT_NEIGHBOR );
		UINT8 nNeborIndex = m_pPointInfo[ nIndex ].nNeighbor[ nCount ];
		if( nNeborIndex == I3_NVPOINT_INDEX_NONE )
		{
			return nullptr;
		}
		I3ASSERT( nNeborIndex < m_nCount );
		return &m_pPointInfo[nNeborIndex];
	}

	// pStartPos	(In)	: 현재 위치
	// pEndPos		(In)	: 최종 목표 위치
	UINT8 GetPathDirection( UINT8 nStartIndex, UINT8 nEndIndex );

	// 길찾기 정보 생성
	bool			BuildNavigationInfo();

	virtual void OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32 OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32 OnLoad( i3ResourceFile * pResFile ) override;
};

#endif //__I3_NAVIGATION_MESH_H__
