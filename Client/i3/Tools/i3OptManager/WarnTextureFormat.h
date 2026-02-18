#if !defined( __WARN_TEXTURE_FORMAT_H)
#define __WARN_TEXTURE_FORMAT_H

#include "Warn.h"

class WarnTextureFormat : public Warn
{
	I3_CLASS_DEFINE( WarnTextureFormat);
protected:
	bool		_CheckTexture( i3Texture * pTex, i3StringList * pMsgList);

	virtual bool	_OnCheckNode( i3Node * pNode, i3StringList * pMsgList, void * pUserData = NULL);

public:
	WarnTextureFormat(void);
	virtual ~WarnTextureFormat(void);

	virtual const char *	GetTitle(void);
	virtual bool			IsInterested( char * pszName, char * pszExt);
	virtual bool			Check( char * pszName, char * pszExt, i3ElementBase * pObj, i3StringList * pMsgList);
	virtual i3ElementBase *	Solve( i3ElementBase * pObj, i3StringList * pMsgList);
};

#endif
