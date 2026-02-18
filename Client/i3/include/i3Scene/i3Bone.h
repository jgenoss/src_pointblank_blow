#if !defined( __I3_BONE_H)
#define __I3_BONE_H

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace i3
{
	namespace pack
	{
		struct ALIGN8 BONE
		{
			char					m_szName[MAX_NAME] = { 0 };
			INT32					m_ParentIndex = -1;
			INT32					temp[3] = { 0, 0, 0 };
			::MATRIX				m_Matrix;
			UINT32					m_Depth = 0;
			INT32					temp2[3] = { 0, 0, 0 };
		};

		struct ALIGN8 BONE2
		{
			char					m_szName[MAX_NAME] = { 0 };
			INT32					m_ParentIndex = -1;
			::MATRIX				m_Matrix;
			UINT32					m_Depth = 0;
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop )
#endif

struct ALIGN8 i3Bone : i3::class_common_pool<i3Bone>
{
	i3::rc_string			m_strName;			
	MATRIX					m_Matrix;
	INT16					m_ParentIndex = 0;
	UINT8					m_Depth = 0;

public:

	const char *			getName(void) const				{ return m_strName.c_str(); }
	const i3::rc_string&	getNameString() const			{ return m_strName; }

	void					setName(const i3::rc_string& strName)	{ m_strName = strName; }

	INT32					getParentIndex(void)			{ return static_cast<INT32>(m_ParentIndex); }
	void					setParentIndex(INT32 idx)		{ I3ASSERT( idx < 32768); m_ParentIndex = static_cast<INT16>(idx); }

	MATRIX *				getMatrix(void)					{ return &m_Matrix; }
	void					setMatrix(const MATRIX * pMtx)		{ i3Matrix::Copy( &m_Matrix, pMtx); }
	void					setMatrix(const i3::pack::MATRIX * pMtx) { i3Matrix::Copy(&m_Matrix, pMtx); }

	UINT32					getDepth(void)					{ return static_cast<UINT32>(m_Depth); }
	void					setDepth( UINT32 depth)			{ I3ASSERT( depth < 256); m_Depth = static_cast<UINT8>(depth); }
};

#endif
