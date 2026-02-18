#include "pch.h"
#include "DesignBase.h"

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



