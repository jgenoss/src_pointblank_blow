#if !defined(_I3REG_KEY_H__)
#define _I3REG_KEY_H__

#include "i3RegINT32.h"
#include "i3RegREAL32.h"
#include "i3RegString.h"
#include "i3RegVEC2D.h"
#include "i3RegVEC3D.h"
#include "i3RegVEC4D.h"
#include "i3RegCOLOR.h"
#include "i3RegMATRIX.h"
#include "i3RegData.h"

class I3_EXPORT_SCENE i3RegKey :	public i3TreeNode
{
	I3_CLASS_DEFINE( i3RegKey);

protected:
	i3BinList	*	m_pValueList;

public:
	i3RegKey(void);
	virtual ~i3RegKey(void);

	void		AddData( const char* szName, INT32 val);
	void		AddData( const char* szName, REAL32 val);
	void		AddData( const char* szName, char* val);
	void		AddData( const char* szName, VEC2D val);
	void		AddData( const char* szName, VEC3D val);
	void		AddData( const char* szName, VEC4D val);
	void		AddData( const char* szName, COLOR val);
//	void		AddData( const char* szName, MATRIX val);

	void		AddData( i3RegData * p);
	void		SetData( i3RegData * p, INT32 nIdx);
	i3RegData *	GetData( INT32 nIdx);

	void		RemoveData( i3RegData * p);
	void		RemoveData( INT32	nIdx);

	i3RegData * FindDataByName( const char * pszName);

	bool GetData( const char * pszName, INT32* val);
	bool GetData( const char * pszName, REAL32* val);
	bool GetData( const char * pszName, char* val);
	bool GetData( const char * pszName, VEC2D* val);
	bool GetData( const char * pszName, VEC3D* val);
	bool GetData( const char * pszName, VEC4D* val);
	bool GetData( const char * pszName, COLOR* val);
	bool GetData( const char * pszName, MATRIX* val);

	i3List	*	getDataList()	{	return m_pValueList;}
	INT32		GetDataCount();

public:
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32  OnLoad( i3ResourceFile * pResFile);
};

class I3_EXPORT_SCENE i3RegKeyFile : public i3NamedElement
{
	I3_CLASS_DEFINE( i3RegKeyFile);
protected:
	i3RegKey	*	m_pRootKey;

public:
	i3RegKeyFile();
	virtual ~i3RegKeyFile();

	i3RegKey	* getRoot()	{	return m_pRootKey;}
	void		setRoot( i3RegKey * pKey);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual UINT32	OnExportSQLCreate( const char * pFileName ); 
	virtual UINT32	OnExportSQLUpdate( const char * pFileName ); 
};
#endif // _I3REGKEY_H__