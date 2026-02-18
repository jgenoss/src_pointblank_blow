#if !defined( __PROPERTY_SHAPESET_H)
#define __PROPERTY_SHAPESET_H

#include "i3treePropertyBase.h"

///////////////////////////////////////////////////////////////////////////////////////////
/////// Skin Skeleton Property 
class CPropertyShapeSet : public CpropertyBase
{
//For Value
private:	

public:
	i3PhysixShapeSet * m_pShapeSet;
	
//For Function
private:	
	
public:
	CPropertyShapeSet(); 
	~CPropertyShapeSet(); 

	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr);		
	virtual void PopUpDialog(void); 
}; 
typedef class CPropertyShapeSet		I3_P_SHAPESET;

#endif
