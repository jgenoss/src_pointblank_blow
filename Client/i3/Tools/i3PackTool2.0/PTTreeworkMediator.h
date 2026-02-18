#ifndef _PTTREEWORK_MEDIATOR_H_
#define _PTTREEWORK_MEDIATOR_H_

#include "PTMediator.h"

class PTTreeworkBase;
class PTTreeworkQA;
class PTTreeworkLive;

class PTTreeworkMediator : public PTMediator
{
	friend PTTreeworkMediator& TheMediator();
public:
	~PTTreeworkMediator();

	virtual void CreateCollegues();
	virtual void CollegueChanged(PTCollegue* c);

	PTTreeworkLive* GetTreeworkLive() const { return m_twLive; }
	PTTreeworkQA*	GetTreeworkQA() const { return m_twQA; }

private:
	void QAChanged();
	void LiveChanged();

private:
	void ExpandBestfitTreeItem();

private:
	PTTreeworkQA*		m_twQA;
	PTTreeworkLive*		m_twLive;

	PTTreeworkMediator();
	PTTreeworkMediator(const PTTreeworkMediator& src);
	PTTreeworkMediator& operator=(const PTTreeworkMediator& src);
};

PTTreeworkMediator& TheMediator();

#endif