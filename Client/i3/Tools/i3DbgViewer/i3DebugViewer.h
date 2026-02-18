#if !defined( __I3_DEBUG_VIEWER_H)
#define __I3_DEBUG_VIEWER_H

class i3DebugViewer : public i3Viewer
{
	I3_CLASS_DEFINE( i3DebugViewer);
protected:
	char *			m_pCmdBuffer;

public:
	i3DebugViewer(void);
	virtual ~i3DebugViewer(void);

	void			setCmdBuffer( char * pBuff)			{ m_pCmdBuffer = pBuff; }

	virtual void	Render(void);
};

#endif
