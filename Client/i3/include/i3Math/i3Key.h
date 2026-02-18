// i3KeyFrame.h

#ifndef i3KeyFrameData___H___
#define i3KeyFrameData___H___

#include "i3Vector.h"
#include "i3Color.h"

struct PACKED I3_KEY_FRAME_DATA
{
	REAL32		_Time = 0.0f;
	union
	{
		i3::pack::VEC2D		_Vector2d;
		i3::pack::VEC3D		_Vector3d;
		i3::pack::VEC4D		_Vector4d;
		i3::pack::COLORREAL	_Color;
	};

	I3_KEY_FRAME_DATA() {}
};

bool	operator<( const I3_KEY_FRAME_DATA& a1, const I3_KEY_FRAME_DATA& a2 );

template<> struct i3::less<I3_KEY_FRAME_DATA*> : i3::less_ptr<I3_KEY_FRAME_DATA*> {};

class i3KeyFrame
{
public:
	virtual ~i3KeyFrame();

protected:
	REAL32				m_TimeMin = -1.0f;
	REAL32				m_TimeMax = -1.0f;
	i3::vector_multiset<I3_KEY_FRAME_DATA*> m_KeyFrame;

	INT32 FindKeyIndex(REAL32 tm);

public:
	void SetKeyData(REAL32 tm, VEC2D* pVec);
	void SetKeyData(REAL32 tm, VEC3D* pVec);
	void SetKeyData(REAL32 tm, VEC4D* pVec);
	void SetKeyData(REAL32 tm, COLORREAL* pCol);
	void SetKeyData(REAL32 tm, COLOR* pCol);
	void SetKeyData(REAL32 tm, REAL32 v1, REAL32 v2, REAL32 v3, REAL32 v4);

	void GetKeyData(REAL32 tm, VEC2D* pVec);
	void GetKeyData(REAL32 tm, VEC3D* pVec);
	void GetKeyData(REAL32 tm, VEC4D* pVec);
	void GetKeyData(REAL32 tm, COLORREAL* pCol);
	void GetKeyData(REAL32 tm, COLOR* pCol);

	void GetKeyData(INT32 index, VEC2D* pVec);
	void GetKeyData(INT32 index, VEC3D* pVec);
	void GetKeyData(INT32 index, VEC4D* pVec);
	void GetKeyData(INT32 index, COLORREAL* pCol);
	void GetKeyData(INT32 index, COLOR* pCol);

	I3_KEY_FRAME_DATA* GetKeyFrame(INT32 index);
	I3_KEY_FRAME_DATA* GetNearestKeyFrame(REAL32 tm);

	void RemoveKeyFrame(I3_KEY_FRAME_DATA* pData);
	void RemoveKeyFrame(INT32 index);
	
	INT32 GetCount(void)	{ return (INT32)m_KeyFrame.size();}
	REAL32 GetTimeMin(void)	{return m_TimeMin;}
	REAL32 GetTimeMax(void)		{return m_TimeMax;}

};


#endif
