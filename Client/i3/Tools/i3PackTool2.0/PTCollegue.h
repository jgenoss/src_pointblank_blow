#ifndef _PTCOLLEGUE_H_
#define _PTCOLLEGUE_H_

class PTMediator;

class PTCollegue
{
public:
	virtual ~PTCollegue() {}

	virtual void SetMediator(PTMediator* m) = 0;
};

#endif