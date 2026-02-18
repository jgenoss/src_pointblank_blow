#if !defined( __I3_ANIMATION2_H)
#define __I3_ANIMATION2_H

#include "i3Base.h"
#include "i3Math.h"

#include "i3Animation.h"

class I3_EXPORT_SCENE i3Animation2 : public i3Animation
{
	I3_CLASS_DEFINE( i3Animation2);

protected:
	INT32	 *			m_pBoneIndex;

public:
	i3Animation2(void);
	virtual ~i3Animation2(void);

	virtual BOOL		Create( INT32		TrackCount);

	INT32 *				getBoneIndexTable(void)					{ return m_pBoneIndex; }
	void				setBoneIndexTable( INT32 * pTable)		{ m_pBoneIndex = pTable; }

	INT32				GetBoneIndex( INT32 idx)				{ return m_pBoneIndex[idx]; }
	void				SetBoneIndex( INT32 idx, INT32 bidx)	{ m_pBoneIndex[idx] = bidx; }

	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

};

#endif
