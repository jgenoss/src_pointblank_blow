#if !defined( __I3_LevelL_PATHPOINT_H)
#define __I3_LevelL_PATHPOINT_H

#include "i3LevelControl.h"

class i3LevelPath;

class I3_EXPORT_TDK i3LevelPathPoint : public i3LevelControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelPathPoint, i3LevelControl);

protected:
	UINT32			m_PointStyle = 0;
	INT32			m_Priority = 0;

	// Volatile
	INT32			m_Depth = 0;

public:
	i3LevelPathPoint(void);

	void			Create(void);

	UINT32			getPointStyle(void)					{ return m_PointStyle; }
	void			setPointStyle( UINT32 val)			{ m_PointStyle = val; }

	INT32			getPriority(void)					{ return m_Priority; }
	void			setPriority( INT32 val)				{ m_Priority = val; }

	INT32			getDepth(void)					{ return m_Depth; }
	void			setDepth( INT32 depth)			{ m_Depth = depth; }
	void			addDepth(void)					{ m_Depth++; }
	void			decDepth(void)					{ m_Depth--; }

	virtual void	OnTargeted( BOOL bTargeted) override;

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif