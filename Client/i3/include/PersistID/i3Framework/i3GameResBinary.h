#if !defined ( __I3_GAME_RES_BINARY_H_)
#define __I3_GAME_RES_BINARY_H_

#include "i3GameRes.h"

class i3GameResBinary : public i3GameRes
{
	I3_CLASS_DEFINE( i3GameResBinary);

protected:
	i3PersistantElement * m_pKeyObject;

public:
	i3GameResBinary( void);
	virtual ~i3GameResBinary( void);

	BOOL			Load( const char * pszFile);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif