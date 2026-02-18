#if !defined( _I3_ACTION_DISABLE_H__)
#define _I3_ACTION_DISABLE_H__

class I3_EXPORT_FRAMEWORK i3ActionDisable : public i3Action
{
	I3_CLASS_DEFINE( i3ActionDisable);
protected:

public:
	virtual void	GetInfoString( char * pszStr, bool bShort, INT32 len);
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};


#endif //#define _I3_ACTION_DISABLE_H__