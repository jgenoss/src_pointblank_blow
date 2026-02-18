#ifndef __PCCAFE_MANAGER_H__
#define __PCCAFE_MANAGER_H__

#include "PCCafeBase.h"

class CPCCafeManager
{
	UINT32					m_ui32WorkThreadCount;

	UINT32					m_ui32PushThreadIdx;
	UINT32					m_ui32PopThreadIdx;

	CPCCafeBase**			m_ppPCCafe;

public:
	CPCCafeManager();
	~CPCCafeManager();

	BOOL					Create();
	void					Destroy();

	INT32					PushData( PCCAFE_INPUT* pInputData );

	INT32					PopCount_T();
	PCCAFE_OUTPUT*			PopData_T();
	void					PopIdx_T();
};

#endif