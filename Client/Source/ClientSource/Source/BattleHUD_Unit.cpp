#include "pch.h"
#include "BattleHUD_Unit.h"

#include "UI/Unit.h"
#include "ConfigRenderBattle.h"

using namespace minimap_old;
//----------------------------------------------
// Units
//----------------------------------------------
Unit::Unit()
{
	m_unit = new GameObject; 
	MEMDUMP_NEW( m_unit, sizeof(GameObject));
}

Unit::~Unit() 
{ 
	I3_SAFE_DELETE( m_unit);
}

UnitDomiDino::UnitDomiDino(i3AttrSet* attrSet, INT32 slotIdx)
{
	ImagesDomiDino * pImages = new ImagesDomiDino(attrSet);
	MEMDUMP_NEW( pImages, sizeof( ImagesDomiDino));
	m_unit->SetComponent( pImages);

	RenderMoveByModeDomiDino * pRender = new RenderMoveByModeDomiDino;
	MEMDUMP_NEW( pRender, sizeof( RenderMoveByModeDomiDino));
	Component* c = m_unit->SetComponent( pRender);
	static_cast<RenderMoveByMode*>(c)->Register(m_unit, slotIdx);

	minimap_new::JudgeUpdatableDomiDino * pJudge = new minimap_new::JudgeUpdatableDomiDino(slotIdx);
	MEMDUMP_NEW( pJudge, sizeof( minimap_new::JudgeUpdatableDomiDino));
	m_unit->SetComponent( pJudge);
}

void UnitDomiDino::Update(REAL32 tm)
{

}
//----------------------------------------------
// Images
//----------------------------------------------
Images::Images() : m_sprite(0), m_tex(0) {}
Images::~Images() 
{
	I3_SAFE_RELEASE(m_tex);
	I3_SAFE_RELEASE(m_sprite);
}

ImagesDomiDino::ImagesDomiDino(i3AttrSet* attrSet) : m_attrSet(attrSet) {}
i3Sprite2D* ImagesDomiDino::GetImages()
{
	if (m_sprite) return m_sprite;

	i3Sprite2D* spr = i3Sprite2D::new_object();
	spr->Create(1, true, true);
	spr->SetEnable(0, true);
	m_tex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);
	spr->SetTexture(m_tex);
	I3_MUST_RELEASE(m_tex);
	spr->SetTextureCoord(0, 156, 97, 10, 22);
	spr->SetColor(9, 255, 255, 255, 255);
	//spr->SetRect(0, (g_pViewer->GetViewWidth()*0.5f) - ());
	m_attrSet->AddChild(spr);

	m_sprite = spr;
	return m_sprite;
}
//----------------------------------------------
// Move
//----------------------------------------------
void MoveDomiDinoScrollMode::Render(REAL32 tm)
{
	i3Sprite2D* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	REAL32 x = 0.0f;
	REAL32 y = 0.0f;
	REAL32 w = 0.0f;
	REAL32 h = 0.0f;
	image->SetRect(0, x, y, w, h);
}

void MoveDomiDinoFullMode::Render(REAL32 tm)
{
	i3Sprite2D* image = 
		static_cast<Images*>(GetOwner()->GetComponent("Images"))->GetImages();

	REAL32 x = 0.0f;
	REAL32 y = 0.0f;
	REAL32 w = 0.0f;
	REAL32 h = 0.0f;
	image->SetRect(0, x, y, w, h);
}

//----------------------------------------------
// RenderMoveByMode
//----------------------------------------------
RenderMoveByMode::~RenderMoveByMode()
{
	lookupTable::iterator it = m_moves.begin();
	for (; it != m_moves.end(); it++)
	{
		I3_SAFE_DELETE( it->second);
	}
	m_moves.clear(); 
}
void RenderMoveByMode::Render(REAL32 tm, MINIMAP_MODE mode)
{
	if (m_moves.empty()) return;

	lookupTable::iterator it = m_moves.find(mode);
	if (it != m_moves.end())
	{
		Move* move = it->second;
		move->Render(tm);
	}
}

void RenderMoveByModeDomiDino::Register(GameObject* o, INT32 slotIdx)
{
	Move* scrollMode = new MoveDomiDinoScrollMode(slotIdx);
	MEMDUMP_NEW( scrollMode, sizeof(MoveDomiDinoScrollMode));
	static_cast<Component*>(scrollMode)->SetOwner(o);

	Move* fullMode = new MoveDomiDinoFullMode(slotIdx);
	MEMDUMP_NEW( fullMode, sizeof(MoveDomiDinoFullMode));
	static_cast<Component*>(fullMode)->SetOwner(o);

	m_moves.insert(lookupTable::value_type(MINIMAP_SCROLL_MODE, scrollMode));
	m_moves.insert(lookupTable::value_type(MINIMAP_FULL_MODE, fullMode));
}
//----------------------------------------------
// Action
//----------------------------------------------

//----------------------------------------------
// PlayState
//----------------------------------------------

//----------------------------------------------
// AutoAction
//----------------------------------------------

//----------------------------------------------
// CharaUpdatable
//----------------------------------------------