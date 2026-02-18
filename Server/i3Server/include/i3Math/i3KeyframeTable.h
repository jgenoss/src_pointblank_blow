#if !defined( __I3_KEYFRAME_TABLE_H)
#define __I3_KEYFRAME_TABLE_H

#include "i3MathDefine.h"

class I3_EXPORT_MATH i3KeyframeTable : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3KeyframeTable);
protected:
	RT_REAL32	m_fCount;
	RT_REAL32	m_fStep;
	INT32		m_Count;

	RT_REAL32 *	m_pTable;
	//INT32		m_hmemTable;

	RT_REAL32 *	m_pTime;
	//INT32		m_hmemTime;
protected:

	INT32		getIndex( RT_REAL32 t)
	{
		return (INT32)( i3Math::Mul( t, m_fCount));
	}

	UINT32		_GetTimeIndex( RT_REAL32 t);

public:
	enum INTERPOLATION
	{
		NONE,
		LERP,
		SLERP,
		SPLINE
	};

public:
	i3KeyframeTable(void);
	virtual ~i3KeyframeTable(void);

	void		Create( INT32 count, BOOL bFixedInterval);
	INT32		GetCount(void)					{ return m_Count; }

	RT_REAL32 *	GetTable(void)					{ return m_pTable; }

	void		SetFixedIntervalEnable( BOOL bFlag);
	BOOL		IsFixedInterval(void)			{ return m_pTime == NULL; }

	RT_REAL32	GetKeyframe( RT_REAL32 t, INTERPOLATION mode);
	RT_REAL32	GetKeyframe( INT32 idx)			{ return m_pTable[idx]; }

	void		SetKeyframe( RT_REAL32 t, RT_REAL32 val);
	void		SetKeyframeByIndex( INT32 idx, RT_REAL32 val);

	void		InsertKeyframe( RT_REAL32 t, RT_REAL32 val);
	void		RemoveKeyframe( INT32 idx);

	RT_REAL32	GetTime( INT32 idx)					{ return m_pTime[idx]; }
	void		SetTime( INT32 idx, RT_REAL32 t)	{ m_pTime[idx] = t; }

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
