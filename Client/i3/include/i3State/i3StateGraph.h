#if !defined( __I3_STATE_GRAPH_H)
#define __I3_STATE_GRAPH_H

#include "i3Base.h"
#include "i3State.h"

class i3StateGraph : public i3NamedElement
{
	I3_CLASS_DEFINE( i3StateGraph);

protected:
	i3State *			m_pStateInit;
	i3State *			m_pOldState;
	i3State *			m_pCurState;

public:
	i3StateGraph(void);
	virtual ~i3StateGraph(void);

	void			SetStartState( i3State * pState);
	i3State *		GetStartState(void)								{ return m_pStateInit; }

	void			SetCurrentState( i3State * pState);
	i3State *		GetCurrentState(void)							{ return m_pCurState; }

	i3State *		GetPreviousState(void)							{ return m_pOldState; }
	
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif