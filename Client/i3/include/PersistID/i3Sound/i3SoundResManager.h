#if !defined( __I3_SOUND_RES_MANAGER_H)
#define __I3_SOUND_RES_MANAGER_H

class i3SoundResManager : public i3NamedResourceManager
{
	I3_CLASS_DEFINE( i3SoundResManager);
protected:

public:
	i3SoundResManager(void);
	virtual ~i3SoundResManager(void);

	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName );
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj);

	void							FlushBuffers(void);
};

#endif
