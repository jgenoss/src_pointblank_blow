
#include "stdafx.h"
#include "CtlLVI3APrivate.h"



LVI3AValBase::LVI3AValBase(EVALTYPE type)
{
	m_type = type;
}

LVI3AValBase::~LVI3AValBase()
{
}


//
// LVI3AVal_DefaultName
//

LVI3AVal_DefaultName::LVI3AVal_DefaultName()
	: LVI3AValBase(EVAL_DEFAULT)
{

}

LVI3AVal_DefaultName::~LVI3AVal_DefaultName()
{

}


//
// LVI3AVal_AIStateName
//
LVI3AVal_AIStateName::LVI3AVal_AIStateName()
	: LVI3AValBase(EVAL_AISTATE_NAME)
{

}

LVI3AVal_AIStateName::~LVI3AVal_AIStateName()
{

}

