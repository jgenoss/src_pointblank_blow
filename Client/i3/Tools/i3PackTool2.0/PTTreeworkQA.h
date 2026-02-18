#ifndef _PTTREEWORK_QA_H_
#define _PTTREEWORK_QA_H_

#include "PTTreeworkBase.h"

enum QA_STATE
{
	QS_WORKINGFOLDER_OPENED,
	QS_LCLICKED,
};

class PTTreeworkQA : public PTTreeworkBase
{
public:
	PTTreeworkQA();
	virtual ~PTTreeworkQA();

	virtual void Create(const char* workdir, CDragDropTreeCtrl* treeCtrl);
	virtual void Destroy();

	void SetState(QA_STATE state)	{ m_state = state; }
	QA_STATE GetState() const		{ return m_state;  }

	// 이벤트 처리
	void LClicked(NMHDR *nmhdr);
	void OpenWorkingFolder(CDragDropTreeCtrl* treeCtrl);
	void Expand(const PTFile* file);

private:
	QA_STATE m_state;
};

#endif