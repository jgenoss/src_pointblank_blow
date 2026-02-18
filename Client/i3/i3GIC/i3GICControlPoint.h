#if !defined( __I3_GIC_CONTROL_POINT_H)
#define __I3_GIC_CONTROL_POINT_H

#include "i3GICShape.h"
#include "i3GICDefine.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICControlPoint : public i3GICShape
{
	I3_EXPORT_CLASS_DEFINE( i3GICControlPoint, i3GICShape);
protected:
	I3GIC_CP_STYLE	m_CPStyle = 0;

	
public:
	i3GICControlPoint(void);
	virtual ~i3GICControlPoint(void) {}

	/** \brief Control Point의 스타일을 반환합니다. 
		\return I3GIC_CP에 정의된 스타일		*/
	I3GIC_CP_STYLE	getCPStyle(void)			{ return m_CPStyle; }

	/** \brief Control Point의 스타일을 설정합니다. 
		\param style I3GIC_CP에 정의된 스타일
		\return none		*/
	void			setCPStyle( I3GIC_CP_STYLE style)	{ m_CPStyle = style; }

	/** \brief Control Point의 스타일을 검사합니다. 
		\param mask I3GIC_CP에 정의된 스타일
		\return mask와 같은 스타일이면 true, 아니면 false	*/
	bool			isCPStyle( I3GIC_CP_STYLE mask)		{ return (m_CPStyle & mask) == mask; }

	virtual bool	OnSelect( bool bFlag);

	/** \brief Control point 자신은 Drag를 하지 않는다. */
	virtual	bool	OnPreSize( REAL32 cx, REAL32 cy)	{ return false; }

	/** \brief HitTest By Point
		\param[in] pCtx i3GICDiagramContext 객체 포인터
		\param[in] x 좌표
		\param[in] y 좌표
		\return Hit 여부 */
	virtual	bool	OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y);

	virtual void	OnDraw( i3GICDiagramContext * pCtx);

	/** \brief 인스턴스 정보를 pDest로 복사합니다. 
		\sa i3TreeNode::CopyTo		*/	
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
};

#endif	// I3_DEBUG

#endif
