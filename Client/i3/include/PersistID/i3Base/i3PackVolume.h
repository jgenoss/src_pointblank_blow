#if !defined( __I3_PACK_VOLUME_H)
#define __I3_PACK_VOLUME_H

#include "i3PackNode.h"

class I3_EXPORT_BASE i3PackVolume : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PackVolume);
protected:
	i3PackNode *		m_pRoot;
	i3FileStream 		m_File;

public:
	i3PackVolume(void);
	virtual ~i3PackVolume(void);

	i3PackNode *		getRoot(void)						{ return m_pRoot; }
	void				setRoot( i3PackNode * pRoot)		{ I3_REF_CHANGE( m_pRoot, pRoot); }

	i3PackNode *		FindPath( char * pszPath, I3_PACK_FILE_REF ** ppFileInfo)			
	{ 
		return m_pRoot->FindPath( pszPath, ppFileInfo); 
	}

	i3FileStream *		getStream(void)						{ return &m_File; }

	bool				SaveToFile( const char * pszPath);
	bool				LoadFromFile( const char * pszPath);
};

#endif
