// ResTreeCtrl.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "ResTreeCtrl.h"

enum	I3LVRES_ICON
{
	I3LVRES_ICON_PRIM_PRIMITIVE = 0,
	I3LVRES_ICON_PRIM_BOX,
	I3LVRES_ICON_PRIM_SPHERE,
	I3LVRES_ICON_PRIM_CAPSULE,

	I3LVRES_ICON_CHARA_GROUP,
	I3LVRES_ICON_CHARA,

	I3LVRES_ICON_WORLD_GROUP,
	I3LVRES_ICON_WORLD,

	I3LVRES_ICON_OBJECT_GROUP,
	I3LVRES_ICON_OBJECT,

	I3LVRES_ICON_EFFECT_GROUP,
	I3LVRES_ICON_EFFECT,

	I3LVRES_ICON_LIGHT_GROUP,
	I3LVRES_ICON_LIGHT_DIRECTIONAL,
	I3LVRES_ICON_LIGHT_SPOT,
	I3LVRES_ICON_LIGHT_POINT,

	I3LVRES_ICON_CAMERA_GROUP,
	I3LVRES_ICON_CAMERA,

	I3LVRES_ICON_TEXTURE_GROUP,
	I3LVRES_ICON_TEXTURE,

	I3LVRES_ICON_ANIMATION_GROUP,
	I3LVRES_ICON_ANIMATION,

	I3LVRES_ICON_SOUND_GROUP,
	I3LVRES_ICON_SOUND,

	I3LVRES_ICON_SCENEGRAPH_GROUP,
	I3LVRES_ICON_SCENEGRAPH,

	I3LVRES_ICON_RESOURCE,

	I3LVRES_ICON_MAX,
};

static INT32 addIcon( CImageList * pList, INT32 id)
{
	HICON hIcon	= ::LoadIcon( g_hInstTDK, MAKEINTRESOURCE( id));

	INT32 nIndex = pList->Add( hIcon);

	::DestroyIcon( hIcon);

	return nIndex;
}

// CResTreeCtrl
IMPLEMENT_DYNAMIC(CResTreeCtrl, CTreeCtrl)
CResTreeCtrl::CResTreeCtrl()
{
	m_dwStyle			= 0;
	m_pImageList		= NULL;


	m_pCurItem			= NULL;
}

CResTreeCtrl::~CResTreeCtrl()
{
	I3_SAFE_DELETE( m_pImageList);
}

BEGIN_MESSAGE_MAP(CResTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()

// CResTreeCtrl message handlers
void	CResTreeCtrl::InitImageList(void)
{
	I3ASSERT( m_pImageList == NULL);

	SetBkColor( g_pTDKGlobalRes->getColor( I3TDK_COLOR_FORE_APP_BKGND));
	SetTextColor( g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_NORMAL));

	m_pImageList	= new CImageList;
	m_pImageList->Create( 16, 16, ILC_COLOR32 | ILC_MASK, 32, 0);

	addIcon( m_pImageList, IDI_TDK_BOX);
	addIcon( m_pImageList, IDI_TDK_BOX);
	addIcon( m_pImageList, IDI_TDK_SPHERE);
	addIcon( m_pImageList, IDI_TDK_CYLINDER);
	addIcon( m_pImageList, IDI_TDK_CHARACTER_GROUP);
	addIcon( m_pImageList, IDI_TDK_CHARACTER);
	addIcon( m_pImageList, IDI_TDK_WORLD);
	addIcon( m_pImageList, IDI_TDK_WORLD_PARTITION);
	addIcon( m_pImageList, IDI_TDK_OBJECT_GROUP);
	addIcon( m_pImageList, IDI_TDK_OBJECT);
	addIcon( m_pImageList, IDI_TDK_EFFECT_GROUP);
	addIcon( m_pImageList, IDI_TDK_EFFECT);

	addIcon( m_pImageList, IDI_TDK_LIGHT_GROUP);
	addIcon( m_pImageList, IDI_TDK_LIGHT_DIRECTIONAL);
	addIcon( m_pImageList, IDI_TDK_LIGHT_SPOT);
	addIcon( m_pImageList, IDI_TDK_LIGHT_POINT);
	addIcon( m_pImageList, IDI_CAMERA);
	addIcon( m_pImageList, IDI_CAMERA);
	addIcon( m_pImageList, IDI_TEXTURE);
	addIcon( m_pImageList, IDI_TEXTURE);

	addIcon( m_pImageList, IDI_ANIM);
	addIcon( m_pImageList, IDI_ANIM);

	addIcon( m_pImageList, IDI_SOUND);
	addIcon( m_pImageList, IDI_SOUND);

	addIcon( m_pImageList, IDI_NODE);
	addIcon( m_pImageList, IDI_NODE);

	addIcon( m_pImageList, IDI_RESOURCE);

	HICON hIcon	=  AfxGetApp()->LoadStandardIcon( IDI_QUESTION);
	INT32 nIndex = m_pImageList->Add( hIcon);

	if( nIndex != -1)
	{
		m_pImageList->SetOverlayImage( nIndex, 1);
	}

	SetImageList( m_pImageList, TVSIL_NORMAL);
}

HTREEITEM CResTreeCtrl::AddTreeItem( HTREEITEM hParent, i3LevelResRef * pResRef)
{
	TVINSERTSTRUCT	data;
	HTREEITEM		hItem;

	data.hParent		= hParent;
	data.hInsertAfter	= TVI_LAST;

	SetItemInfo( &data.item, pResRef);

	hItem	= InsertItem( &data);

	if( hParent != TVI_ROOT)
	{
		TVITEM item;
		item.hItem	= hParent;
		item.mask	= TVIF_CHILDREN;

		if( GetItem( &item))
		{
			i3LevelResRef * pParent = (i3LevelResRef*)pResRef->getParent();
			if( pParent != NULL)
			{
				item.cChildren = pParent->getChildCount();
			}
			else
			{
				item.cChildren = 0;
			}

			SetItem(&item);
		}
	}

	UpdateData();

	return hItem;
}

HTREEITEM CResTreeCtrl::Rec_AddTreeItem( HTREEITEM hParent, i3LevelResRef * pResRef)
{
	HTREEITEM hItem;

	hItem = AddTreeItem( hParent, pResRef);

	for( INT32 i = 0; i < pResRef->getChildCount(); ++i)
	{
		i3LevelResRef * pChild = (i3LevelResRef *)pResRef->getChild( i);

		if( pChild != NULL)
		{
			Rec_AddTreeItem( hItem, pChild);
		}
	}

	return hItem;
}

HTREEITEM CResTreeCtrl::AddRes( i3LevelRes * pRes)
{
	i3::vector<i3LevelResRef*> list;

	HTREEITEM hCur, hParent;
	i3LevelResRef * pRef;

	g_pResDB->GetHierarchyPath( pRes, list);

	hParent = TVI_ROOT;

	for( size_t i = 1; i < list.size(); i++)
	{
		pRef = list[i];

		hCur = i3TDK::FindTreeItemByData( this, pRef, hParent, FALSE);
		
		if( hCur == NULL)
		{
			hCur = AddTreeItem( hParent, pRef);
		}

		hParent = hCur;
	}

	return hParent;
}

void	CResTreeCtrl::DeleteRes( i3LevelRes * pRes )
{
	if( NULL == pRes )
	{
		return;
	}

	i3LevelResRef * pRef = g_pResDB->GetResRef( pRes );

	if( NULL != pRef )
	{
		HTREEITEM hCur = i3TDK::FindTreeItemByData( this, pRef, TVI_ROOT, TRUE );
		DeleteTreeItem( hCur );
	}
}

void CResTreeCtrl::DeleteTreeItem( HTREEITEM hItem)
{
	DeleteItem( hItem);
}

static char	s_szTypeRootName[I3LV_RESDB_TYPE_COUNT][128] =
{
	"Level Primitive",
	"Character",
	"Object",
	"World",
	"Light",
	"Camera",
	"Scene-Graph",
	"Effect",
	"Animation",	
	"Sound",		
	"Texture",
	"Event",
	"Path",
};

static UINT32 GetIconIndex( i3LevelResRef * pRef)
{
	I3ASSERT( pRef != NULL);

	i3LevelRes	*	pRes	= pRef->getRes();
	INT32			nIdx	= 0;

	if( pRes != NULL)
	{
		INT32 nResType = pRes->getResType();
		switch( nResType)
		{
			case I3LV_TYPE_PRIMITIVE :
				{
					if( i3::same_of<i3LevelResPrimitive_Box*>(pRes)) //->IsExactTypeOf( i3LevelResPrimitive_Box::static_meta()) )
						nIdx = I3LVRES_ICON_PRIM_BOX;
					else if( i3::same_of<i3LevelResPrimitive_Sphere*>(pRes)) //->IsExactTypeOf( i3LevelResPrimitive_Sphere::static_meta()) )
						nIdx = I3LVRES_ICON_PRIM_SPHERE;
					else if( i3::same_of<i3LevelResPrimitive_Capsule*>(pRes)) //->IsExactTypeOf( i3LevelResPrimitive_Cylinder::static_meta()) )
						nIdx = I3LVRES_ICON_PRIM_CAPSULE;
					else if( i3::same_of<i3LevelResPrimitive_Respawn*>(pRes)) //->IsExactTypeOf( i3LevelResPrimitive_Respawn::static_meta()) )
						nIdx = I3LVRES_ICON_CHARA;
				}
				break;
			case I3LV_TYPE_CHARA :			nIdx = I3LVRES_ICON_CHARA;			break;
			case I3LV_TYPE_WORLD :			nIdx = I3LVRES_ICON_WORLD;			break;
			case I3LV_TYPE_OBJECT :			nIdx = I3LVRES_ICON_OBJECT;			break;
			case I3LV_TYPE_EFFECT :			nIdx = I3LVRES_ICON_EFFECT;			break;
			case I3LV_TYPE_LIGHT :			nIdx = I3LVRES_ICON_LIGHT_GROUP;	break;
			case I3LV_TYPE_CAMERA :			nIdx = I3LVRES_ICON_CAMERA;			break;
			case I3LV_TYPE_TEXTURE :		nIdx = I3LVRES_ICON_TEXTURE;		break;
			case I3LV_TYPE_ANIMATION :		nIdx = I3LVRES_ICON_ANIMATION;		break;
			case I3LV_TYPE_SOUND :			nIdx = I3LVRES_ICON_SOUND;			break;
			case I3LV_TYPE_SCENEGRAPH :		nIdx = I3LVRES_ICON_SCENEGRAPH;		break;
		}
	}
	else
	{
		const char * pTypeName = pRef->getName();

		nIdx = I3LVRES_ICON_RESOURCE;

		if( pRef->getIconStyle() & I3LV_RESREF_ICON_GROUPROOT)
		{
			if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_PRIMITIVE], pTypeName) )
			{
				nIdx = I3LVRES_ICON_PRIM_PRIMITIVE;					
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_CHARA], pTypeName) )
			{
				nIdx = I3LVRES_ICON_CHARA_GROUP;			
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_OBJECT], pTypeName) )
			{
				nIdx = I3LVRES_ICON_OBJECT_GROUP;			
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_WORLD], pTypeName) )
			{
				nIdx = I3LVRES_ICON_WORLD_GROUP;			
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_LIGHT], pTypeName) )
			{
				nIdx = I3LVRES_ICON_LIGHT_GROUP;			
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_CAMERA], pTypeName) )
			{
				nIdx = I3LVRES_ICON_CAMERA_GROUP;			
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_SCENEGRAPH], pTypeName) )
			{
				nIdx = I3LVRES_ICON_SCENEGRAPH_GROUP;		
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_EFFECT], pTypeName) )
			{
				nIdx = I3LVRES_ICON_EFFECT_GROUP;			
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_ANIMATION], pTypeName) )
			{
				nIdx = I3LVRES_ICON_ANIMATION_GROUP;		
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_SOUND], pTypeName) )
			{
				nIdx = I3LVRES_ICON_SOUND_GROUP;			
			}
			else if( i3::generic_is_iequal( s_szTypeRootName[ I3LV_RESDB_TYPE_TEXTURE], pTypeName) )
			{
				nIdx = I3LVRES_ICON_TEXTURE_GROUP;			
			}
		}
	}

	return nIdx;
}

void CResTreeCtrl::SetItemInfo( TVITEM * pInfo, i3LevelResRef * pResRef)
{
	I3ASSERT( pResRef != NULL);

	memset( pInfo, 0, sizeof( TVITEM));

	pInfo->mask				= TVIF_CHILDREN | TVIF_PARAM | TVIF_IMAGE | TVIF_TEXT;
	pInfo->cChildren		= 0;
	pInfo->hItem			= 0;
	pInfo->lParam			= (LPARAM) pResRef;

	pInfo->pszText			= (LPSTR)pResRef->getName();
	pInfo->iImage			= GetIconIndex( pResRef);
}
