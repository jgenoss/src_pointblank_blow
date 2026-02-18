#if !defined( __I3_EXP_IMAGE_H)
#define __I3_EXP_IMAGE_H

class i3ImageEXP : public BitmapIO 
{
protected:
	DWORD	_getFlag( I3G_IMAGE_FORMAT format);
	int		_getType( I3G_IMAGE_FORMAT format);

public:
	i3ImageEXP(void);
	virtual ~i3ImageEXP(void) override;

	virtual int ExtCount() override;
	virtual const TCHAR *Ext( int n ) override;
	virtual const TCHAR *LongDesc( ) override;
	virtual const TCHAR *ShortDesc() override;
	virtual const TCHAR *AuthorName() override;
	virtual const TCHAR *CopyrightMessage() override;
	virtual UINT Version() override;
	virtual int Capability() override;
	virtual void ShowAbout( HWND hWnd ) override;
	virtual DWORD EvaluateConfigure( ) override;
	virtual BOOL LoadConfigure( void *ptr ) override;
	virtual BOOL SaveConfigure( void *ptr ) override;
	virtual DWORD ChannelsRequired() override;
	virtual BMMRES GetImageInfo( BitmapInfo *bi ) override;
	virtual BMMRES GetImageName( BitmapInfo *bi, TCHAR *filename) override;
	//virtual void EvalMatch(TCHAR *matchString); 
 
	virtual BitmapStorage *Load( BitmapInfo *bi, Bitmap *map, BMMRES *status ) override;
	virtual BMMRES OpenOutput(BitmapInfo *bi, Bitmap *map) override;
	virtual BMMRES Write(int frame) override;
	virtual int Close(int flag) override;
};

#endif
