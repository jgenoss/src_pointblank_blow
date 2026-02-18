#ifndef __I3_USERNODE_H__
#define __I3_USERNODE_H__

#include "i3Base.h"
#include "i3Node.h"
#include "i3UserAttr.h"

typedef void (*I3USERNODEPROC)( i3SceneTracer * pTracer, void *pParam );

class I3_EXPORT_SCENE i3UserNode : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3UserNode, i3Node );

protected:
	I3USERNODEPROC		m_pFunc = nullptr;
	void				*m_pParam = nullptr;
	i3UserAttr			*m_pUserAttr;				// [initialize at constructor]

	bool				m_bNeedToFlush = false;

public:
	i3UserNode();
	~i3UserNode();

	void	SetNeedToFlush( bool bFlag)						{ m_bNeedToFlush = bFlag; }
	bool	GetNeedToFlush(void)							{ return m_bNeedToFlush; }

	void SetCallBack( I3USERNODEPROC pFunc, void *pParam );
	void SetAttrCallBack( I3USERATTRPROC pFunc, void *pParam )
	{
		m_pUserAttr->SetCallBack( pFunc, pParam );
	}
	void Drive( i3SceneTracer * pTracer );
};


#endif //__I3_USERNODE_H__
