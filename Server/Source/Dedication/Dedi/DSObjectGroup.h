#ifndef __DS_OBJECT_GROUP_H__
#define __DS_OBJECT_GROUP_H__

#include "DSObject.h"

#define DS_OBJECT_GROUP_COUNT		5

enum DS_OBJECT_GROUP_TYPE
{
	DS_OBJECT_GROUP_TYPE_NONE,
	DS_OBJECT_GROUP_TYPE_TURN,			// 오브젝트들이 순차적으로 활성화 됩니다.
	DS_OBJECT_GROUP_TYPE_SHARE,			// 그룹 내용을 공유합니다. 한 개의 오브젝트를 활성화 시키면 함께 활성화 됩니다.
};

class CDSObjectGroupIdx
{
	DS_OBJECT_GROUP_TYPE	m_eGroupType;

	INT32					m_i32ObjectCount;
	INT32					m_i32ObjectIdx[ DS_OBJECT_GROUP_COUNT ];

public:
	CDSObjectGroupIdx();
	~CDSObjectGroupIdx();

	DS_OBJECT_GROUP_TYPE	GetGroupType()										{	return m_eGroupType;					}
	void					SetGroupType( DS_OBJECT_GROUP_TYPE eGroupType )		{	m_eGroupType = eGroupType;				}

	INT32					GetObjectCount()									{	return m_i32ObjectCount;				}
	INT32					GetObjectList( INT32 i32Idx )						{	return m_i32ObjectIdx[ i32Idx ];		}
	BOOL					InsertObject( INT32 i32Idx );
};

class CDSObjectGroup
{
	DS_OBJECT_GROUP_TYPE	m_eGroupType;

	INT32					m_i32ObjectCount;
	CDSObject*				m_pObject[ DS_OBJECT_GROUP_COUNT ];


public:
	CDSObjectGroup();
	~CDSObjectGroup();

	BOOL					SetGroup( CDSObjectGroupIdx* pGroupIdx, CDSObject* pObject );

	void					Reset();
	void					Update();
};

#endif
