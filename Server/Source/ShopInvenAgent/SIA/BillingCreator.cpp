#include "pch.h"
#include "BillingCreator.h"

#include "BillingWorker_NO.h"
#include "BillingWorker_ZPT.h"
#include "BillingWorker_Thai.h"
#include "BillingWorker_SM.h"
#include "BillingWorker_Vietnam.h"
#include "BillingWorker_Korea.h"
#include "BillingWorker_NorthA.h"
#include "BillingWorker_Philippines.h"
#include "BillingWorker_Italy.h"
#include "BillingWorker_NorthA_Steam.h"
#include "BillingWorker_Brazil.h"
#include "BillingWorker_Russia.h"
#include "BillingWorker_Latin.h"

CBillingBase * CBillingCreator::GetWorker( INT32 i32LinkMethod )
{
	switch( i32LinkMethod )
	{
	case LINK_METHOD_NONE:
		return new CBillingWorker_NO;
	case LINK_METHOD_ZPT:
		return new CBillingWorker_ZPT;
	case LINK_METHOD_THAI:
		return new CBillingWorker_Thai;
	case LINK_METHOD_SM:
		return new CBillingWorker_SM;
	case LINK_METHOD_VIETNAM:
		return new CBillingWorker_Vietnam;
	case LINK_METHOD_KOREA:
		return new CBillingWorker_Korea;
	case LINK_METHOD_NORTHAMERICA:
		return new CBillingWorker_NorthA;
	case LINK_METHOD_PHILIPPINES:
		return new CBillingWorker_Philippines;
	case LINK_METHOD_ITALY:
		return new CBillingWorker_Italy;
	case LINK_METHOD_NORTHA_STEAM:
		return new CBillingWorker_NorthA_Steam;
	case LINK_METHOD_BRAZIL:
		return new CBillingWorker_Brazil;
	case LINK_METHOD_RUSSIA:
		return new CBillingWorker_Russia;
	case LINK_METHOD_LATINAMERICA:
		return new CBillingWorker_Latin;
	default: 
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[ERROR]create undefined LinkMethod(%i) LoginWorker", i32LinkMethod );
		return new CBillingBase;
	}
}
