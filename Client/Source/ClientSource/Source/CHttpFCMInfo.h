#ifndef __CHTTPFCMINFO_H__
#define __CHTTPFCMINFO_H__

struct RETURN_VALUE {
	char token[100] = { 0 };
	char appId[20] = { 0 };
	char appArea[20] = { 0 };
	char username[50] = { 0 };
	char idCard[100] = { 0 };
	char Email[100] = { 0 };
	char IP[100] = { 0 };
} ;

class CHttpFCMInfo : public i3NetworkHTTP
{
	I3_CLASS_DEFINE( CHttpFCMInfo, i3NetworkHTTP );
private:
	i3NetworkHTTP	*	m_pHttpFCMInfo = nullptr;
	UINT32				m_nFCMInfoConnectHTTPRetryCount = 0;

protected:
	bool _ParsingFCMInfo(char* pszString);
	
public:
	CHttpFCMInfo();
	virtual ~CHttpFCMInfo();

	INT32 _WorkProcess(char* URL,RETURN_VALUE *); 
	bool	ConnectionCreate(char *);
	bool	ConnectionDestroy();
};
#endif