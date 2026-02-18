#if !defined( __DLG_MAPNAME_H)
#define __DLG_MAPNAME_H

class CDlgMapName : public i3UIModalDriver
{
	I3_CLASS_DEFINE( CDlgMapName);

protected:

public:
	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);	
};

#endif
