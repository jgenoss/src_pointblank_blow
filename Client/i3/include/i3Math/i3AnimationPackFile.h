#if !defined( __I3_ANIMATION_PACK_FILE_H)
#define __I3_ANIMATION_PACK_FILE_H

#include "i3Vector3List.h"
#include "i3Vector4List.h"
#include "i3Vector3List16Bits.h"
#include "i3Vector4List16Bits.h"
#include "i3Animation.h"
#include "i3Animation2.h"


class I3_EXPORT_MATH i3AniPackFileProgress
{
public:
	enum EPHASE
	{
		EPHASE_SAVE_START							= 2000,
		EPHASE_SAVE_CONV_I3ANIM_TO_I3ANIM2			, // compressing
		EPHASE_SAVE_I3ANIM2_TABLE					,
		EPHASE_SAVE_I3ANIM2							,
		EPHASE_SAVE_I3ANIM2_HEADER_AND_TABLE		,
		EPHASE_SAVE_END
	};

	enum EACT
	{
		EACT_NONE	=0,
		EACT_BEGIN,
		EACT_UPDATE,
		EACT_END,
	};

	virtual bool OnReport(EACT act=EACT_NONE) = 0;

	EPHASE							m_phase;
	std::string						m_argText0;
	INT32							m_arg0;
	INT32							m_arg1;
	INT32							m_arg2;
};


class I3_EXPORT_MATH i3AnimationPackFile : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3AnimationPackFile, i3ElementBase);
protected:
	i3Vector3List		m_TList;
	i3Vector4List		m_RList;
	i3Vector3List		m_SList;

	INT32				m_TotalTCount = 0;
	INT32				m_TotalRCount = 0;
	INT32				m_TotalSCount = 0;

	/// for 16Bits data	///
	// 16Bits data는 따로 List를 관리하도록 한다. Pack에는 동시에 갖고 있을 수 있다.
	i3Vector3List16Bits	m_TList16Bits;		///< 16Bits Translation Key List
	i3Vector4List16Bits	m_RList16Bits;		///< 16Bits Rotation Key List
	i3Vector3List16Bits	m_SList16Bits;		///< 16Bits Scale Key List

	INT32				m_TotalT16Count = 0;
	INT32				m_TotalR16Count = 0;
	INT32				m_TotalS16Count = 0;

protected:
	void				_BuildArrays( i3::vector<i3Animation*>& AnimList);
	i3Animation2 *		_Convert2Anim2( i3Animation * pAnim);

public:
	UINT32		SaveToFile( char * pszPath, const i3::vector<i3Animation*>& AnimList, 
		i3AniPackFileProgress* progressCallback= nullptr);
	UINT32		LoadFromFile( char * pszPath);

	UINT32		LoadAnimationList( const char * pszPath);
};

#endif
