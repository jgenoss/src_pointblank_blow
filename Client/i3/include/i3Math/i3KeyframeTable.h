#if !defined( __I3_KEYFRAME_TABLE_H)
#define __I3_KEYFRAME_TABLE_H

#include "i3MathDefine.h"

class I3_EXPORT_MATH i3KeyframeTable : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3KeyframeTable, i3PersistantElement);
protected:
	REAL32	m_fCount = 0.0f;
	REAL32	m_fStep = 0.0f;
	INT32		m_Count = 0;

	REAL32 *	m_pTable = nullptr;
	REAL32 *	m_pTime = nullptr;
protected:

	INT32		getIndex( REAL32 t)
	{
		return (INT32)( i3Math::Mul( t, m_fCount));
	}

	UINT32		_GetTimeIndex( REAL32 t);

public:
	enum INTERPOLATION
	{
		NONE,
		LERP,
		SLERP,
		SPLINE
	};

public:
	virtual ~i3KeyframeTable(void);

	void		Create( INT32 count, bool bFixedInterval);
	INT32		GetCount(void)					{ return m_Count; }

	REAL32 *	GetTable(void)					{ return m_pTable; }

	bool		IsFixedInterval(void)			{ return m_pTime == nullptr; }

	REAL32	GetKeyframe( REAL32 t, INTERPOLATION mode);
	REAL32	GetKeyframe( INT32 idx)			{ return m_pTable[idx]; }

	void		SetKeyframe( REAL32 t, REAL32 val);
	void		SetKeyframeByIndex( INT32 idx, REAL32 val);

	void		InsertKeyframe( REAL32 t, REAL32 val);
	void		RemoveKeyframe( INT32 idx);

	REAL32	GetTime( INT32 idx)					{ return m_pTime[idx]; }
	void		SetTime( INT32 idx, REAL32 t)	{ m_pTime[idx] = t; }

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
