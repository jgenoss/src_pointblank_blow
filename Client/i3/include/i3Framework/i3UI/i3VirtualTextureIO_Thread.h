#if !defined( __I3_VIRTUAL_TEXTURE_IO_THREAD_H)
#define __I3_VIRTUAL_TEXTURE_IO_THREAD_H

class i3VirtualTextureIO;

class I3_EXPORT_FRAMEWORK i3VirtualTextureIO_Thread : public i3Thread
{
	I3_EXPORT_CLASS_DEFINE( i3VirtualTextureIO_Thread, i3Thread);
protected:
	i3VirtualTextureIO *		m_pIOManager = nullptr;
	i3Signal *					m_pSignal_Complete = nullptr;
	i3Signal *					m_pSignal_Queue = nullptr;

	volatile bool				m_bExitRequest = false;			// 시그널에 의해 보호되기 때문에,
	volatile bool				m_bExitResult = true;			// 캐시미스같은 것이 일어나지 않다고 보아도 좋다..

public:
	virtual ~i3VirtualTextureIO_Thread(void);

	void				setup( i3VirtualTextureIO * pManager, i3Signal * pSig_Complete, i3Signal * pSig_Queue)
	{
		m_pIOManager = pManager;
		m_pSignal_Complete = pSig_Complete;
		m_pSignal_Queue = pSig_Queue;
	}

	virtual UINT32		OnRunning( void * pData) override;
};

#endif
