#if !defined( __I3_TDK_CLIPBOARD_H)
#define __I3_TDK_CLIPBOARD_H

class I3_EXPORT_TDK i3TDKClipboard 
{
public:
	i3TDKClipboard(void) {}
	virtual ~i3TDKClipboard(void) {}

	void					Copy( HWND hwnd, i3PersistantElement * pObj);
	bool					IsObject( i3ClassMeta * pMeta);
	i3PersistantElement *	Paste( HWND hwnd);
};

#endif
