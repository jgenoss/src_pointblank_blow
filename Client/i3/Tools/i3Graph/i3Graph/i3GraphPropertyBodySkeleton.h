#if !defined( __PROPERTY_BODY_SKELETON_H)
#define __PROPERTY_BODY_SKELETON_H

#include "i3treePropertyBase.h"

///////////////////////////////////////////////////////////////////////////////////////////
/////// Skin Skeleton Property 
class CPropertyBodySkeleton : public CpropertyBase
{
//For Value
private:	

public:
	i3LOD *			m_pLOD; 
//For Function
private:	
	
public:
	CPropertyBodySkeleton(); 
	~CPropertyBodySkeleton(); 

	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr);		
	virtual void PopUpDialog(void); 
}; 
typedef class CPropertyBodySkeleton		I3_P_BODY_SKELETON;

#endif
