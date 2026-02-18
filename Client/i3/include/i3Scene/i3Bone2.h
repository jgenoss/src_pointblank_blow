#if !defined( __I3_BONE2_H)
#define __I3_BONE2_H

#include "i3Bone.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

/** \brief Bone2 정보 구조체 */
struct ALIGN4 i3Bone2 : public i3Bone
{
	INT32					m_IKBoneInfoIndex;		///< IK BoneInfo List에서의 Index
	INT32					m_IKContextIndex;		///< IK Bone이 포함하고 있는 Context Index

	INT32					pad[4];					///< padding

public:
	/** \brief IK Info Index를 반환한다.
		\return IK Info Index */
	INT32					getIKBoneInfoIndex( void)		{ return m_IKBoneInfoIndex; }

	/** \brief IK Info Index를 설정한다.
		\param[in] idx IK Info Index
		\return N/A */
	void					setIKBoneInfoIndex( INT32 idx)	{ m_IKBoneInfoIndex = idx; }

	/** \brief IK Context Index를 반환한다.
		\return IK Context Index */
	INT32					getIKContextIndex( void)		{ return m_IKContextIndex; }

	/** \brief IK Context Index를 설정한다.
		\param[in] idx IK Context Index
		\return N/A */
	void					setIKContextIndex( INT32 idx)	{ m_IKContextIndex = idx; }

};

#if defined( I3_COMPILER_VC)
#pragma pack( pop )
#endif

#endif
