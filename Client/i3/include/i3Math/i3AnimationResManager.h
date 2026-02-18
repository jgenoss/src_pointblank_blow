#if !defined( __I3_ANIMATION_RES_MANAGER_H)
#define __I3_ANIMATION_RES_MANAGER_H

#include "i3Animation.h"
#include "i3Base/itl/vector_map.h"

class I3_EXPORT_MATH i3AnimationResManager : public i3NamedResourceManager
{
	I3_EXPORT_CLASS_DEFINE( i3AnimationResManager, i3NamedResourceManager);
protected:
	i3::vector<std::string>				m_PackPathList;
	i3::vector_map<UINT128,UINT32>		m_LinkedPackList;		// Animation Path hash, Pack Path

	bool		_FindFromPack( char * pszOut, const char * pszName);
	UINT32		_FindPackPathString( const char * pszPackPath);

public:
	virtual i3ResourceObject*		GetResource( const char * pszResFile, const char * pszBasePath, const char *szName, bool bBackgroundLoad = false) override;
	virtual UINT32					SaveResource( const char * pszBasePath, i3ResourceObject * pObj) override;

	void		AddResourcePack( const char * pszAnimPath, const char * pszPackPath);
};

#endif
