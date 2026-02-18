#ifndef _PTMEDIATOR_H_
#define _PTMEDIATOR_H_

class PTCollegue;

class PTMediator
{
public:
	virtual ~PTMediator() {}

	virtual void CreateCollegues() = 0;
	virtual void CollegueChanged(PTCollegue* c) = 0;
};

#endif