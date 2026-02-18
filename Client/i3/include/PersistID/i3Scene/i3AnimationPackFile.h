#if !defined( __I3_ANIMATION_PACK_FILE_H)
#define __I3_ANIMATION_PACK_FILE_H

class I3_EXPORT_SCENE i3AnimationPackFile : public i3ElementBase
{
	I3_CLASS_DEFINE( i3AnimationPackFile);
protected:

public:
	i3AnimationPackFile(void);
	virtual ~i3AnimationPackFile(void);

	UINT32		SaveToFile( char * pszPath, i3List * pAnimList);
	UINT32		LoadFromFile( char * pszPath);
};

#endif
