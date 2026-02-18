// i3KeyFrame.cpp

#include "i3MathType.h"
#include "i3Key.h"
#include "i3Color.h"
#include "i3Vector.h"
/*
static INT32 _KeyFrameCompareProc(I3_KEY_FRAME_DATA* pKey1, I3_KEY_FRAME_DATA* pKey2)
{
	if(pKey1->_Time > pKey2->_Time)
		return 1;
	return -1;
}
*/
bool	operator<( const I3_KEY_FRAME_DATA& a1, const I3_KEY_FRAME_DATA& a2 )
{
	return a1._Time < a2._Time;
}

i3KeyFrame::~i3KeyFrame()
{
	I3_KEY_FRAME_DATA* pData;
	for(size_t i=0;i<m_KeyFrame.size();i++)
	{
		pData = m_KeyFrame.get_vector()[i];
	
		NetworkDump_Free( pData, __FILE__, __LINE__);
        delete pData;
	}
}

void i3KeyFrame::SetKeyData(REAL32 tm, VEC2D* pVec)
{
	if(m_TimeMin < 0.f || m_TimeMin > tm ) m_TimeMin = tm;
	if(m_TimeMax < tm ) m_TimeMax = tm;

	I3_KEY_FRAME_DATA* pData = new I3_KEY_FRAME_DATA;

	NetworkDump_Malloc( pData, sizeof( I3_KEY_FRAME_DATA), __FILE__, __LINE__);

	pData->_Time = tm;
	pData->_Vector2d.x = i3Vector::GetU(pVec);
	pData->_Vector2d.y = i3Vector::GetV(pVec);

	m_KeyFrame.insert(pData);
}

void i3KeyFrame::SetKeyData(REAL32 tm, VEC3D* pVec)
{
	if(m_TimeMin < 0.f || m_TimeMin > tm ) m_TimeMin = tm;
	if(m_TimeMax < tm ) m_TimeMax = tm;

	I3_KEY_FRAME_DATA* pData = new I3_KEY_FRAME_DATA;

	NetworkDump_Malloc( pData, sizeof( I3_KEY_FRAME_DATA), __FILE__, __LINE__);

	pData->_Time = tm;
	pData->_Vector3d.x = i3Vector::GetX(pVec);
	pData->_Vector3d.y = i3Vector::GetY(pVec);
	pData->_Vector3d.z = i3Vector::GetZ(pVec);

	m_KeyFrame.insert(pData);
}

void i3KeyFrame::SetKeyData(REAL32 tm, VEC4D* pVec)
{
	if(m_TimeMin < 0.f || m_TimeMin > tm ) m_TimeMin = tm;
	if(m_TimeMax < tm ) m_TimeMax = tm;

	I3_KEY_FRAME_DATA* pData = new I3_KEY_FRAME_DATA;

	NetworkDump_Malloc( pData, sizeof( I3_KEY_FRAME_DATA), __FILE__, __LINE__);

	pData->_Time = tm;
	pData->_Vector4d.x = i3Vector::GetX(pVec);
	pData->_Vector4d.y = i3Vector::GetY(pVec);
	pData->_Vector4d.z = i3Vector::GetZ(pVec);
	pData->_Vector4d.w = i3Vector::GetW(pVec);

	m_KeyFrame.insert(pData);
}

void i3KeyFrame::SetKeyData(REAL32 tm, COLORREAL* pCol)
{
	if(m_TimeMin < 0.f || m_TimeMin > tm ) m_TimeMin = tm;
	if(m_TimeMax < tm ) m_TimeMax = tm;

	I3_KEY_FRAME_DATA* pData = new I3_KEY_FRAME_DATA;

	NetworkDump_Malloc( pData, sizeof( I3_KEY_FRAME_DATA), __FILE__, __LINE__);
	pData->_Time = tm;
	pData->_Color.r = i3Color::GetR(pCol);
	pData->_Color.g = i3Color::GetG(pCol);
	pData->_Color.b = i3Color::GetB(pCol);
	pData->_Color.a = i3Color::GetA(pCol);

	m_KeyFrame.insert(pData);
}

void i3KeyFrame::SetKeyData(REAL32 tm, COLOR* pCol)
{
	if(m_TimeMin < 0.f || m_TimeMin > tm ) m_TimeMin = tm;
	if(m_TimeMax < tm ) m_TimeMax = tm;

	I3_KEY_FRAME_DATA* pData = new I3_KEY_FRAME_DATA;

	NetworkDump_Malloc( pData, sizeof( I3_KEY_FRAME_DATA), __FILE__, __LINE__);
	pData->_Time = tm;
	pData->_Color.r = ((REAL32)i3Color::GetR(pCol))/255.f;
	pData->_Color.g = ((REAL32)i3Color::GetG(pCol))/255.f;
	pData->_Color.b = ((REAL32)i3Color::GetB(pCol))/255.f;
	pData->_Color.a = ((REAL32)i3Color::GetA(pCol))/255.f;

	m_KeyFrame.insert(pData);
}

void i3KeyFrame::SetKeyData(REAL32 tm, REAL32 v1, REAL32 v2, REAL32 v3, REAL32 v4)
{
	if(m_TimeMin < 0.f || m_TimeMin > tm ) m_TimeMin = tm;
	if(m_TimeMax < tm ) m_TimeMax = tm;

	I3_KEY_FRAME_DATA* pData = new I3_KEY_FRAME_DATA;

	NetworkDump_Malloc( pData, sizeof( I3_KEY_FRAME_DATA), __FILE__, __LINE__);

	pData->_Time = tm;
	pData->_Color.r = v1;
	pData->_Color.g = v2;
	pData->_Color.b = v3;
	pData->_Color.a = v4;

	m_KeyFrame.insert(pData);
}


INT32 i3KeyFrame::FindKeyIndex(REAL32 tm)
{
	I3_KEY_FRAME_DATA* pData1;
	I3_KEY_FRAME_DATA* pData2;
	
	if (!m_KeyFrame.empty())
	{
		for(size_t i=0;i<m_KeyFrame.size()-1;i++)
		{
			pData1 = m_KeyFrame.get_vector()[i];
			pData2 = m_KeyFrame.get_vector()[i+1];

			if( pData1->_Time <= tm && tm < pData2->_Time)
			{
				return i;
			}
		}
	}
	return -1;
}

void i3KeyFrame::GetKeyData(REAL32 tm, VEC2D* pVec)
{
	INT32 index = FindKeyIndex(tm);

	if( index >= 0 )
	{
		I3_KEY_FRAME_DATA* pData1 = m_KeyFrame.get_vector()[index  ];
		I3_KEY_FRAME_DATA* pData2 = m_KeyFrame.get_vector()[index+1];

		REAL32 t1 = tm - pData1->_Time; // 3.t - 3.0
		REAL32 t2 = pData2->_Time - tm; // 5.1 - 3.t

		t1 = t1 / (t1+t2); //Normalize

		i3Vector::Set( pVec,
			(1.f-t1)*pData1->_Vector2d.x + t1*pData2->_Vector2d.x,
			(1.f-t1)*pData1->_Vector2d.y + t1*pData2->_Vector2d.y);
	}
	else
	{
		i3Vector::Set( pVec, 0.f, 0.f);
	}
}

void i3KeyFrame::GetKeyData(INT32 index, VEC2D* pVec)
{
	I3_KEY_FRAME_DATA* pData1 = m_KeyFrame.get_vector()[index];
	
	i3Vector::Set( pVec,
		pData1->_Vector2d.x,
		pData1->_Vector2d.y);
}

void i3KeyFrame::GetKeyData(REAL32 tm, VEC3D* pVec)
{
	INT32 index = FindKeyIndex(tm);

	if( index >= 0 )
	{
		I3_KEY_FRAME_DATA* pData1 = m_KeyFrame.get_vector()[index  ];
		I3_KEY_FRAME_DATA* pData2 = m_KeyFrame.get_vector()[index+1];

		REAL32 t1 = tm - pData1->_Time; // 3.t - 3.0
		REAL32 t2 = pData2->_Time - tm; // 5.1 - 3.t

		t1 = t1 / (t1+t2); //Normalize

		i3Vector::Set( pVec,
			(1.f-t1)*pData1->_Vector3d.x + t1*pData2->_Vector3d.x,
			(1.f-t1)*pData1->_Vector3d.y + t1*pData2->_Vector3d.y,
			(1.f-t1)*pData1->_Vector3d.z + t1*pData2->_Vector3d.z);
	}
	else
	{
		i3Vector::Set( pVec, 0.f, 0.f, 0.f);
	}
}

void i3KeyFrame::GetKeyData(INT32 index, VEC3D* pVec)
{
	I3_KEY_FRAME_DATA* pData1 = m_KeyFrame.get_vector()[index];

	i3Vector::Set( pVec,
		pData1->_Vector3d.x,
		pData1->_Vector3d.y,
		pData1->_Vector3d.z);
}

void i3KeyFrame::GetKeyData(REAL32 tm, VEC4D* pVec)
{
	INT32 index = FindKeyIndex(tm);

	if( index >= 0 )
	{
		I3_KEY_FRAME_DATA* pData1 =  m_KeyFrame.get_vector()[index  ];
		I3_KEY_FRAME_DATA* pData2 =  m_KeyFrame.get_vector()[index+1];

		REAL32 t1 = tm - pData1->_Time; // 3.t - 3.0
		REAL32 t2 = pData2->_Time - tm; // 5.1 - 3.t

		t1 = t1 / (t1+t2); //Normalize

		i3Vector::Set( pVec,
			(1.f-t1)*pData1->_Vector4d.x + t1*pData2->_Vector4d.x,
			(1.f-t1)*pData1->_Vector4d.y + t1*pData2->_Vector4d.y,
			(1.f-t1)*pData1->_Vector4d.z + t1*pData2->_Vector4d.z,
			(1.f-t1)*pData1->_Vector4d.w + t1*pData2->_Vector4d.w);
	}
	else
	{
		i3Vector::Set( pVec, 0.f, 0.f, 0.f, 0.f);
	}
}


void i3KeyFrame::GetKeyData(INT32 index, VEC4D* pVec)
{
	I3_KEY_FRAME_DATA* pData1 = m_KeyFrame.get_vector()[index  ];

	i3Vector::Set( pVec,
		pData1->_Vector4d.x,
		pData1->_Vector4d.y,
		pData1->_Vector4d.z,
		pData1->_Vector4d.w);
}

void i3KeyFrame::GetKeyData(REAL32 tm, COLORREAL* pCol)
{
	INT32 index = FindKeyIndex(tm);

	if( index >= 0 )
	{
		I3_KEY_FRAME_DATA* pData1 = m_KeyFrame.get_vector()[index ];
		I3_KEY_FRAME_DATA* pData2 = m_KeyFrame.get_vector()[index+1];

		REAL32 t1 = tm - pData1->_Time; // 3.t - 3.0
		REAL32 t2 = pData2->_Time - tm; // 5.1 - 3.t

		t1 = t1 / (t1+t2); //Normalize

		i3Color::Set( pCol,
			(1.f-t1)*pData1->_Color.r + t1*pData2->_Color.r,
			(1.f-t1)*pData1->_Color.g + t1*pData2->_Color.g,
			(1.f-t1)*pData1->_Color.b + t1*pData2->_Color.b,
			(1.f-t1)*pData1->_Color.a + t1*pData2->_Color.a);
	}
	else
	{
		i3Color::Set( pCol, 0.f, 0.f, 0.f, 0.f);
	}
}

void i3KeyFrame::GetKeyData(INT32 index, COLORREAL* pCol)
{
	I3_KEY_FRAME_DATA* pData1 = m_KeyFrame.get_vector()[index ];

	i3Color::Set( pCol,
		pData1->_Color.r,
		pData1->_Color.g,
		pData1->_Color.b,
		pData1->_Color.a);
}

void i3KeyFrame::GetKeyData(REAL32 tm, COLOR* pCol)
{
	INT32 index = FindKeyIndex(tm);

	if( index >= 0 )
	{
		I3_KEY_FRAME_DATA* pData1 = m_KeyFrame.get_vector()[index  ];
		I3_KEY_FRAME_DATA* pData2 = m_KeyFrame.get_vector()[index+1];

		REAL32 t1 = tm - pData1->_Time; // 3.t - 3.0
		REAL32 t2 = pData2->_Time - tm; // 5.1 - 3.t

		t1 = t1 / (t1+t2); //Normalize

		i3Color::Set( pCol,
			(UINT8)(255.f * ((1.f-t1)*pData1->_Color.r + t1*pData2->_Color.r) ),
			(UINT8)(255.f * ((1.f-t1)*pData1->_Color.g + t1*pData2->_Color.g) ),
			(UINT8)(255.f * ((1.f-t1)*pData1->_Color.b + t1*pData2->_Color.b) ),
			(UINT8)(255.f * ((1.f-t1)*pData1->_Color.a + t1*pData2->_Color.a) ));
	}
	else
	{
		i3Color::Set( pCol, (UINT8) 0, 0, 0, 0);
	}
}

void i3KeyFrame::GetKeyData(INT32 index, COLOR* pCol)
{
	I3_KEY_FRAME_DATA* pData1 = m_KeyFrame.get_vector()[index  ];

	i3Color::Set( pCol,
		(UINT8)(pData1->_Color.r*255.f),
		(UINT8)(pData1->_Color.g*255.f),
		(UINT8)(pData1->_Color.b*255.f),
		(UINT8)(pData1->_Color.a*255.f));
}

I3_KEY_FRAME_DATA* i3KeyFrame::GetKeyFrame(INT32 index)
{
	return m_KeyFrame.get_vector()[index];
}

I3_KEY_FRAME_DATA* i3KeyFrame::GetNearestKeyFrame(REAL32 tm)
{
	INT32 index = FindKeyIndex(tm);

	if( index < 0 ) return nullptr;

	return m_KeyFrame.get_vector()[index];
}

void i3KeyFrame::RemoveKeyFrame(I3_KEY_FRAME_DATA* pData)
{

	i3::pair<i3::vector_multiset<I3_KEY_FRAME_DATA*>::iterator, i3::vector_multiset<I3_KEY_FRAME_DATA*>::iterator> pr = m_KeyFrame.equal_range(pData);
	
	i3::vector_multiset<I3_KEY_FRAME_DATA*>::iterator it = i3::find(pr.first, pr.second, pData);
	
	if (it != pr.second)
	{
		m_KeyFrame.erase(it);	
	
		NetworkDump_Free( pData, __FILE__, __LINE__);
		delete pData;
	}

	pData = m_KeyFrame.get_vector()[0];
	m_TimeMin = pData->_Time;
	pData = m_KeyFrame.get_vector()[m_KeyFrame.size()-1];
	m_TimeMax = pData->_Time;
}

void i3KeyFrame::RemoveKeyFrame(INT32 index)
{
	I3_KEY_FRAME_DATA* pData = m_KeyFrame.get_vector()[index];
	m_KeyFrame.erase(m_KeyFrame.begin() + index);

	NetworkDump_Free( pData, __FILE__, __LINE__);
	delete pData;

	pData = m_KeyFrame.get_vector()[0];
	m_TimeMin = pData->_Time;
	pData = m_KeyFrame.get_vector()[m_KeyFrame.size()-1];
	m_TimeMax = pData->_Time;
}
