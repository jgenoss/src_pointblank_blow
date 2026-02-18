#if !defined( __I3_SCENE_IO_THREAD_H)
#define __I3_SCENE_IO_THREAD_H

#include "i3SceneIOQueue.h"
// 익스포트추가->다시 제거. (12.09.19.수빈)

class i3SceneIOThread : public i3Thread
{
	I3_CLASS_DEFINE( i3SceneIOThread, i3Thread);
protected:
	i3SceneIOQueue *		m_pQueue = nullptr;
	volatile bool			m_bExit = false;
	volatile bool			m_bExitResult = true;
public:
	virtual ~i3SceneIOThread(void);

	void		SetQueue( i3SceneIOQueue * pQueue);

	virtual UINT32		OnRunning( void * pData) override;
};

#endif
