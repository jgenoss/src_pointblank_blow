#include "pch.h"
#include "PCCafeCreator.h"
#include "PCCafeWorker_Korea.h"
#include "PCCafeWorker_NO.h"
#include "PCCafeWorker_NorthA.h"
#include "PCCafeWorker_Philippines.h"
#include "PCCafeWorker_SM.h"
#include "PCCafeWorker_Thai.h"
#include "PCCafeWorker_Vietnam.h"
#include "PCCafeWorker_ZPT.h"
#include "PCCafeWorker_Italy.h"
#include "PCCafeWorker_Russia.h"

CPCCafeBase * CPCCafeCreator::GetWorker( INT32 i32LinkMethod )
{
	switch( i32LinkMethod )
	{
	case LINK_METHOD_NONE:
		return new CPCCafeWorker_NO;
	case LINK_METHOD_ZPT:
		return new CPCCafeWorker_ZPT;
	case LINK_METHOD_THAI:
		return new CPCCafeWorker_Thai;
	case LINK_METHOD_SM:
		return new CPCCafeWorker_SM;
	case LINK_METHOD_VIETNAM:
		return new CPCCafeWorker_Vietnam;
	case LINK_METHOD_KOREA:
		return new CPCCafeWorker_Korea;
	case LINK_METHOD_NORTHAMERICA:
		return new CPCCafeWorker_NorthA;
	case LINK_METHOD_PHILIPPINES:
		return new CPCCafeWorker_Philippines;
	case LINK_METHOD_ITALY:
		return new CPCCafeWorker_Italy;
	case LINK_METHOD_RUSSIA:
		return new CPCCafeWorker_Russia;
	default: 
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[ERROR]create undefined LinkMethod(%i) LoginWorker", i32LinkMethod );
		return new CPCCafeBase;
	}
}
