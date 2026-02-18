#if !defined( __PROPERTY_SKIN_SKELETON_H)
#define __PROPERTY_SKIN_SKELETON_H

#include "i3treePropertyBase.h"

///////////////////////////////////////////////////////////////////////////////////////////
/////// Skin Skeleton Property 
class CpropertySkeleton : public CpropertyBase
{
//For Value
private:	

public:
	i3Skin * m_pSkin; 
//For Function
private:	
	
public:
	CpropertySkeleton(); 
	~CpropertySkeleton(); 

	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr);		
	virtual void PopUpDialog(void); 
}; 
typedef class CpropertySkeleton		I3_P_SKELETON;

#endif
