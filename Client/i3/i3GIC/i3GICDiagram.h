#if !defined( __I3_GIC_DIAGRAM_H)
#define __I3_GIC_DIAGRAM_H

#if defined( I3_DEBUG)

#include "i3GICObject.h"
#include "i3GICShape.h"
#include "i3GICShapeTemplate.h"

class i3GICDiagramContext;
class i3GICDiagramView;

class I3_EXPORT_GIC i3GICDiagram : public i3GICObject
{
	I3_EXPORT_CLASS_DEFINE( i3GICDiagram, i3GICObject);
protected:
	i3::vector<i3GICShape*>				m_ShapeList;
	i3::vector<i3PersistantElement*> m_TemplateList;

	bool			m_bBindContext = false;

protected:
	void			_Rec_FindSelectedShape( i3GICShape * pShape, i3::vector<i3PersistantElement*>& List);

public:
	i3GICDiagram(void) {}
	virtual ~i3GICDiagram(void);

	void			BindContext( bool bBind);

	INT32			AddShape( i3GICShape * pParent, i3GICShape * pShape);
	void			RemoveShape( i3GICShape * pShape);
	void			RemoveAllShape(void);
	void			RemoveSelectedShapes(void);

	/** \brief Shape을 선택합니다. 
		\param [in] pShape 선택할 i3GICShape객체의 포인터
		\return none		*/
	void			SelectShape( i3GICShape * pShape);

	/** \brief 리스트에 있는 Shape을 선택합니다. 
		\param [in] pList i3GICShape을 갖는 리스트의 포인터
		\return none		*/
	void			SelectShape( const i3::vector<i3GICShape*>& List);

	/** \brief Shape리스트에 있는 모든 Shape을 선택합니다. 
		\return none		*/
	void			SelectAllShapes(void);

	/** \brief Shape을 선택해제합니다. 
		\param [in] pShape 선택해제할 i3GICShape객체의 포인터
		\return none		*/
	void			UnselectShape( i3GICShape * pShape);

	/** \brief 리스트에 있는 Shape을 선택해제합니다. 
		\param [in] pList i3GICShape을 갖는 리스트의 포인터
		\return none		*/
	void			UnselectShape( const i3::vector<i3GICShape*>& List );

	/** \brief Shape리스트에 있는 모든 Shape을 선택해제합니다. 
		\return none		*/
	void			UnselectAllShapes(void);

	/** \brief Shape리스트에 있는 Shape중 선택된 Shape을 반환합니다. 
		\param [out] pList 선택된 Shape을 받아올 리스트의 포인터
		\return none		*/
	void			GetSelectedShapes(i3::vector<i3PersistantElement*>& List);

	/** \brief 드래그를 시작합니다.
		\return none		*/
	void			BeginDrag(void);

	/** \brief 드래그를 종료합니다.
		\return none		*/
	void			EndDrag(void);

	/** \brief 드래그를 취소합니다.
		\return none		*/
	void			CancelDrag(void);

	/** \brief Control Point를 Picking한 Dragging
		\param[in] pCtx i3GICDiagramContext 객체 포인터
		\return N/A	*/
	void			DragCP( i3GICDiagramContext * pCtx);

	/** \brief Shape의 Dragging Movement
		\param[in] pCtx i3GICDiagramContext * pCtx
		\return N/A */
	void			DragMove( i3GICDiagramContext * pCtx);

	/** \brief 특정 Shape의 Control Value.. 마우스 이동값을 특정 control에 사용
	*/
	void			DragCV( i3GICDiagramContext * pCtx);

	void			LinkSnap( i3GICShape * pShape, REAL32 * pX, REAL32 * pY);

	i3GICShape *	FindShapeByName( const char * pszName);

	/** \brief Shape리스트에 있는 Shape의 개수를 반환합니다. 
		\return Shape의 개수		*/
	INT32			getShapeCount(void)									{ return (INT32)m_ShapeList.size(); }

	/** \brief Shape리스트에 있는 n번째 Shape의 포인터를 반환합니다. 
		\param idx n
		\return Shape의 포인터		*/
	i3GICShape *	getShape( INT32 idx)								{ return m_ShapeList[idx]; }

	void			SortByDepth( void);

	bool			SaveTemplate( const char * pszPath);
	bool			LoadTemplate( const char * pszPath);
	
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	/////////////////////////////////////////////////////////////////////////////////////////////
	//						for template function
public:
	INT32					getTemplateCount( void)						{ return INT32(m_TemplateList.size()); }
	i3::vector<i3PersistantElement*>*	getTemplateList( void)			{ return &m_TemplateList; }

	i3GICShapeTemplate *	getTemplate( INT32 idx)
	{
		I3_BOUNDCHK( idx, INT32(m_TemplateList.size()));
		return (i3GICShapeTemplate*) m_TemplateList[ idx];
	}

	void			setTemplate( INT32 idx, i3GICShapeTemplate * pTemplate)
	{
		I3_BOUNDCHK( idx, INT32(m_TemplateList.size()));
		m_TemplateList[ idx] = pTemplate;
		I3_SAFE_ADDREF( pTemplate);
	}

	void			addTemplate( i3GICShapeTemplate * pTemplate)
	{
		m_TemplateList.push_back( pTemplate);
		I3_SAFE_ADDREF( pTemplate);
	}

	void			removeTemplate( i3GICShapeTemplate * pTemplate)
	{
//		m_TemplateList.Remove( pTemplate);
		i3::vector<i3PersistantElement*>::iterator it = i3::find(m_TemplateList.begin(), m_TemplateList.end(), pTemplate);
		if (it != m_TemplateList.end()) m_TemplateList.erase(it);
		I3_SAFE_RELEASE( pTemplate);
	}

	void			removeAllTemplate( void)
	{
		for(size_t i = 0; i < m_TemplateList.size(); i++)
		{
			i3GICShapeTemplate * pTemplate = getTemplate( i);
			I3_SAFE_RELEASE( pTemplate);
		}
		m_TemplateList.clear();
	}

	i3GICShapeTemplate *	FindTemplate( const char * pszName);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//					For Debug	
#if defined( I3_DEBUG)
public:
	void			Dump(void);
#endif


	// Version 1.0용 2.0에서는 사용하지 않는다.
private:
	i3::vector<i3GICDiagramView*>	m_ViewList;

public:
	void			AddView( i3GICDiagramView * pView);
	void			RemoveAllView(void);
	void			UpdateAllViews(void);
};

#endif	// I3_DEBUG

#endif
