
#ifndef _CTLLVI3APRIVATE_H_
#define _CTLLVI3APRIVATE_H_


class LVI3AValBase
{
public:
	enum EVALTYPE
	{ 
		EVAL_DEFAULT = 0,
		EVAL_AISTATE_NAME = 1,
		EVAL_MAX
	};

	virtual ~LVI3AValBase();

protected:
	LVI3AValBase() {}
	LVI3AValBase(EVALTYPE type);

private:
	EVALTYPE							m_type;
};


class LVI3AVal_DefaultName : public LVI3AValBase
{
public:
	LVI3AVal_DefaultName();
	virtual ~LVI3AVal_DefaultName();

};


class LVI3AVal_AIStateName : public LVI3AValBase
{
public:
	LVI3AVal_AIStateName();
	virtual ~LVI3AVal_AIStateName();


};




#endif
