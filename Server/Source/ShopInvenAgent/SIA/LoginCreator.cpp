#include "pch.h"
#include "LoginCreator.h"
#include "LoginWorker_NO.h"
#include "LoginWorker_ZPT.h"
#include "LoginWorker_Thai.h"
#include "LoginWorker_SM.h"
#include "LoginWorker_Vietnam.h"
#include "LoginWorker_Korea.h"
#include "LoginWorker_NorthA.h"
#include "LoginWorker_Philippines.h"
#include "LoginWorker_Italy.h"
#include "LoginWorker_NorthA_Steam.h"
#include "LoginWorker_Brazil.h"
#include "LoginWorker_Russia.h"
#include "LoginWorker_Latin.h"

CLoginBase * CLoginCreator::GetWorker( INT32 i32TransMethod )
{
	switch(i32TransMethod)
	{
	case LINK_METHOD_NONE:
		return new CLoginWorker_NO;
	case LINK_METHOD_ZPT:
		return new CLoginWorker_ZPT;
	case LINK_METHOD_THAI:
		return new CLoginWorker_Thai;
	case LINK_METHOD_SM:
		return new CLoginWorker_SM;
	case LINK_METHOD_VIETNAM:
		return new CLoginWorker_Vietnam;
	case LINK_METHOD_KOREA:
		return new CLoginWorker_Korea;
	case LINK_METHOD_NORTHAMERICA:
		return new CLoginWorker_NorthA;
	case LINK_METHOD_PHILIPPINES:
		return new CLoginWorker_Philippines;
	case LINK_METHOD_ITALY:
		return new CLoginWorker_Italy;
	case LINK_METHOD_NORTHA_STEAM:
		return new CLoginWorker_NorthA_Steam;
	case LINK_METHOD_BRAZIL:
		return new CLoginWorker_Brazil;
	case LINK_METHOD_RUSSIA:
		return new CLoginWorker_Russia;
	case LINK_METHOD_LATINAMERICA:
		return new CLoginWorker_Latin;
	default: 
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[ERROR]create undefined TransMethod(%d) LoginWorker", i32TransMethod );
		return new CLoginBase;
	}
}
