#if !defined( __I3_PACK_VOLUME_H)
#define __I3_PACK_VOLUME_H

#include "i3PackNode.h"
#include "i3StreamFile.h"

#if !defined( I3_DEBUG)
#include "itl/vector_map.h"
#endif


struct I3_EXPORT_BASE i3PackFileProgressCallback 
{
	virtual ~i3PackFileProgressCallback() {}
	virtual void OnProgress_Start(int total_file) {}
	virtual void OnProgress_BeforePack( const char* szResFilePath, bool is_enc, int curr_file_count, int total_file) {}
	virtual void OnProgress_AfterPack(const char* szResFilePath, bool is_enc, bool is_success, int curr_file_count, int success_count, int total_file) {}
	virtual void OnProgress_End(int curr_file_count, int success_count, int total_file) {}
};

struct PACK_VOLUME_PREVIEW
{	
	i3::rc_string				filename;
	i3::pack::RESOURCE_FILE_HEADER2	header;
};

class I3_EXPORT_BASE i3PackVolume : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3PackVolume, i3ElementBase);
protected:
	i3PackNode *				m_pRoot = nullptr;
	i3::rc_string				m_strVolumeFilename;

	// i3FileStream경우는 WIN API파일 핸들 자체가 분리되어야 하므로...
	// 완벽하게 tls_ptr로 치환되어야 한다.
	i3::tls_ptr<i3FileStream>	m_tlsFileStream;	
													
	PACK_VOLUME_PREVIEW		m_Preview;
	
public:
	virtual ~i3PackVolume(void);

	i3PackNode *			getRoot(void)						{ return m_pRoot; }
	void					setRoot( i3PackNode * pRoot)		{ I3_REF_CHANGE( m_pRoot, pRoot); }

	i3PackNode *			FindPath( char * pszPath, I3_PACK_FILE_REF ** ppFileInfo)			
	{ 
		if( m_pRoot == nullptr) return nullptr;
		return m_pRoot->FindPath( pszPath, ppFileInfo); 
	}

	i3FileStream*			getCurrThreadStream(void) const;
	i3FileStream*			getMainThreadStream() const;

	const i3::rc_string&	getVolumeFilename() const { return m_strVolumeFilename;  }
	
	PACK_VOLUME_PREVIEW *	getPreview(void)					{ return &m_Preview; }
	
	bool					SaveToFile( const char* pszPath);
	bool					SaveToFile( const char* pszPath, i3PackFileProgressCallback* cb);

	bool					Export( const char * pszDirPath);
	bool					LoadFromFile( const char * pszPath);
	UINT32					Preview( const char* pszPath);

	void					DumpVolume();

private:
	INT32					Export_Rec(i3FileStream* currThreadStream, i3PackNode * pNode, const char * pszDirPath );
};

#endif
