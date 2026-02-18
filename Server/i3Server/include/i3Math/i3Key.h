// i3KeyFrame.h

#ifndef i3KeyFrameData___H___
#define i3KeyFrameData___H___

typedef struct PACKED _I3_KEY_FRAME_DATA
{
	REAL32		_Time;
	union
	{
		PERSIST_VEC2D		_Vector2d;
		PERSIST_VEC3D		_Vector3d;
		PERSIST_VEC4D		_Vector4d;
		PERSIST_COLORREAL	_Color;
	};

} I3_KEY_FRAME_DATA;

class i3KeyFrame
{
public:
	i3KeyFrame();
	virtual ~i3KeyFrame();
protected:
	REAL32				m_TimeMin;
	REAL32				m_TimeMax;
	i3BinList			m_KeyFrame;

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
	
	INT32 GetCount(void)	{ return m_KeyFrame.GetCount();}
	REAL32 GetTimeMin(void)	{return m_TimeMin;}
	REAL32 GetTimeMax(void)		{return m_TimeMax;}

};


#endif
