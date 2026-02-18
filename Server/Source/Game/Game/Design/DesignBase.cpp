#include "pch.h"
#include "DesignBase.h"
#include "UserSession.h"

CDesignBase::CDesignBase()
{
 
}

CDesignBase::~CDesignBase()
{
	OnDestroy();
}


BOOL CDesignBase::OnCreate()
{
	return TRUE;
}


void CDesignBase::OnDestroy()
{
	return;
}





