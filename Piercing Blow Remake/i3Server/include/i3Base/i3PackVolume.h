#if !defined( __I3_PACK_VOLUME_H)
#define __I3_PACK_VOLUME_H

#include "i3PackNode.h"
#include "i3StreamFile.h"

typedef struct PackVolumePreview
{	
	char						filename[MAX_PATH];
	I3_RESOURCE_FILE_HEADER2	header;
}PACK_VOLUME_PREVIEW;

class I3_EXPORT_BASE i3PackVolume : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PackVolume);
protected:
	i3PackNode *			m_pRoot;
	i3FileStream 			m_File;

	PACK_VOLUME_PREVIEW		m_Preview;
public:
	i3PackVolume(void);
	virtual ~i3PackVolume(void);

	i3PackNode *			getRoot(void)						{ return m_pRoot; }
	void					setRoot( i3PackNode * pRoot)		{ I3_REF_CHANGE( m_pRoot, pRoot); }

	i3PackNode *			FindPath( char * pszPath, I3_PACK_FILE_REF ** ppFileInfo)			
	{ 
		if( m_pRoot == NULL) return NULL;
		return m_pRoot->FindPath( pszPath, ppFileInfo); 
	}

	i3FileStream *			getStream(void)						{ return &m_File; }
	PACK_VOLUME_PREVIEW *	getPreview(void)					{ return &m_Preview; }


	bool					SaveToFile( const char * pszPath);
	bool					Export( const char * pszDirPath);
	bool					LoadFromFile( const char * pszPath);
	UINT32					Preview( const char* pszPath);

	void					DumpVolume();
private:
	INT32					Export_Rec( i3PackNode * pNode, const char * pszDirPath );
};

#endif
