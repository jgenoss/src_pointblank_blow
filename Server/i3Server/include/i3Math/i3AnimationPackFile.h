#if !defined( __I3_ANIMATION_PACK_FILE_H)
#define __I3_ANIMATION_PACK_FILE_H

#include "i3Vector3List.h"
#include "i3Vector4List.h"
#include "i3Vector3List16Bits.h"
#include "i3Vector4List16Bits.h"
#include "i3Animation.h"
#include "i3Animation2.h"

class I3_EXPORT_MATH i3AnimationPackFile : public i3ElementBase
{
	I3_CLASS_DEFINE( i3AnimationPackFile);
protected:
	i3Vector3List		m_TList;
	i3Vector4List		m_RList;
	i3Vector3List		m_SList;

	INT32				m_TotalTCount;
	INT32				m_TotalRCount;
	INT32				m_TotalSCount;

	/// for 16Bits data	///
	// 16Bits data는 따로 List를 관리하도록 한다. Pack에는 동시에 갖고 있을 수 있다.
	i3Vector3List16Bits	m_TList16Bits;		///< 16Bits Translation Key List
	i3Vector4List16Bits	m_RList16Bits;		///< 16Bits Rotation Key List
	i3Vector3List16Bits	m_SList16Bits;		///< 16Bits Scale Key List

	INT32				m_TotalT16Count;
	INT32				m_TotalR16Count;
	INT32				m_TotalS16Count;

protected:
	void				_BuildArrays( i3List * pAnimList);
	i3Animation2 *		_Convert2Anim2( i3Animation * pAnim);

public:
	i3AnimationPackFile(void);
	virtual ~i3AnimationPackFile(void);

	UINT32		SaveToFile( char * pszPath, i3List * pAnimList);
	UINT32		LoadFromFile( char * pszPath);
};

#endif
