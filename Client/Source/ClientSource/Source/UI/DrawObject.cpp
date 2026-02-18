#include "pch.h"
#include "DrawObject.h"
#include "UIClanFunction.h"
#include "StageBattle.h"


DrawObject::DrawObject() :
m_pImageBox(0),
m_nCurImageIdx(0)
{
}

DrawObject::~DrawObject()
{
	struct fn
	{
		void operator()(ImageInfo& ii) {  if (ii.tex) ii.tex->Release(); }
	};
	i3::cu::for_each_container(m_vecImageInfo, fn());
}

void DrawObject::Create(i3UIFrameWnd* frame)
{
	m_pImageBox = i3UIImageBoxEx::new_object();

	for (UINT32 i = 0; i < m_vecImageInfo.size(); i++)
	{
		m_pImageBox->AddTexture(m_vecImageInfo[i].tex, 
							    m_vecImageInfo[i].x, 
							    m_vecImageInfo[i].y, 
							    m_vecImageInfo[i].w, 
							    m_vecImageInfo[i].h);
	}

	m_pImageBox->CreateBaseSceneObjects();
	frame->AddControl(m_pImageBox);
	m_pImageBox->setPos(0, 0, 0);
	I3_MUST_RELEASE(m_pImageBox);
	m_pImageBox->SetCurrentImage(0);
	ucf::AdjustControlPosToParentCenter(m_pImageBox);
	
}

void DrawObject::AddImage(i3Texture* tex, INT32 x, INT32 y, INT32 w, INT32 h)
{
	ImageInfo image;

	image.tex = tex;
	I3_SAFE_ADDREF(tex);
	image.x = x;
	image.y = y;
	image.w = w;
	image.h = h;
	image.halfW = w * 0.5f;
	image.halfH = h * 0.5f;

	m_vecImageInfo.push_back(image);
}

void DrawObject::SetPosition(REAL32 posX, REAL32 posY)
{
	ImageInfo& image = GetCurrentImageInfo();
	m_pImageBox->setPos(posX-image.halfW, posY-image.halfH);
}

void DrawObject::SetRotate(REAL32 rotation)
{
	m_pImageBox->SetRotationRad(rotation);
}

void DrawObject::SetScale(REAL32 scale)
{
	ImageInfo& image = GetCurrentImageInfo();

	REAL32 sizeX  = image.w * scale;
	REAL32 sizeY  = image.h * scale;
	VEC3D* curPos = m_pImageBox->getPos();

	m_pImageBox->SetShapeSize(sizeX, sizeY);
	m_pImageBox->setPos(curPos->x-(sizeX*0.5f)+image.halfW, curPos->y-(sizeY*0.5f)+image.halfH);
}

void DrawObject::SetColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	COLOR* color = m_pImageBox->getColor();

	if (color->r != r || color->g != g || color->b != b || color->a != a)
		i3Color::Set(color, r, g, b, a);

	m_pImageBox->setColor(color);
}

void DrawObject::SetColor(COLOR* color)
{
	m_pImageBox->setColor(color);
}

void DrawObject::SetAlpha(UINT8 a)
{
	COLOR* color = m_pImageBox->getColor();
	i3Color::SetA(color, a);
	m_pImageBox->setColor(color);
}

UINT8 DrawObject::GetAlpha()
{
	COLOR* color = m_pImageBox->getColor();
	return i3Color::GetA(color);
}

void DrawObject::SetCurrentImage(UINT32 index)
{
	I3ASSERT_RETURN(index < m_vecImageInfo.size());
	if (m_nCurImageIdx == index) return;

	m_pImageBox->SetCurrentImage(index);
	m_nCurImageIdx = index;
}

INT32 DrawObject::GetCurrentImage() const { return m_nCurImageIdx; }

ImageInfo& DrawObject::GetCurrentImageInfo()
{
	I3ASSERT(m_nCurImageIdx < m_vecImageInfo.size());

	return m_vecImageInfo[m_nCurImageIdx];
}

void DrawObject::Show()
{
	//if (m_pImageBox->isVisible())
		m_pImageBox->HideShapes(false);
}

void DrawObject::Hide()
{
	//if (m_pImageBox->isVisible() == false)
		m_pImageBox->HideShapes(true);
}