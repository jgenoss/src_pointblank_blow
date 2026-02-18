#ifndef __INIPARSER_H__
#define __INIPARSER_H__

#define MAX_LABELVALUE		256
typedef struct _tagLabelData
{
	char	_strLabel[ MAX_LABELVALUE ];
	char	_strValue[ MAX_LABELVALUE ];
}LABEL_DATA;

class EXPORT_BASE NSM_cIniParser
{
	UINT32					m_ui32LabelCount;
	LABEL_DATA*				m_pLabelData;

	void					_SetLabelSize( char* pBuffer );
	BOOL					_SetLabelBuffer( char* pBuffer );
	void					_ReadLine( char* strLine, UINT32 ui32LineSize, char* pBuffer, UINT32 *pui32ReturnSize );
	BOOL					_GetLineCheck( char* strLine );
	BOOL					_GetLineValue( char* strLine, LABEL_DATA* pLabelData );

public:
	NSM_cIniParser();
	virtual ~NSM_cIniParser();

	BOOL					Create( const char* strFileName );
	void					Close();

	char*					GetValue( const char* strLabel );
	BOOL					GetValue( const char* strLabel, char* strValue );
};

#endif