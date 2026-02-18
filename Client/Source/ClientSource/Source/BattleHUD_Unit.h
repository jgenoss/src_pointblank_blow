#pragma once
#include "ComponentBase.h"

namespace minimap_old
{
	//----------------------------------------------
	// Units
	//----------------------------------------------
	class Unit
	{
	public:
		Unit();
		virtual ~Unit();
		virtual void Update(REAL32 tm) {}

	protected:
		GameObject* m_unit;
	};

	class UnitDomiDino : public Unit
	{
	public:
		UnitDomiDino(i3AttrSet* attrSet, INT32 slotIdx);
		virtual ~UnitDomiDino() {}
		virtual void Update(REAL32 tm);
	};

	//----------------------------------------------
	// Images
	//----------------------------------------------
	class Images : public Component
	{
	public:
		DECLARE_FAMILY_ID("Images");
		Images();
		virtual ~Images();
		virtual i3Sprite2D* GetImages() = 0;

	protected:
		i3Sprite2D* m_sprite;
		i3Texture*  m_tex;
	};

	class ImagesDomiDino : public Images
	{
	public:
		DECLARE_COMPONENT_ID("ImagesDomiDino");
		ImagesDomiDino(i3AttrSet* attrSet);
		virtual ~ImagesDomiDino() {}
		virtual i3Sprite2D* GetImages();

	private:
		i3AttrSet* m_attrSet;
	};

	//----------------------------------------------
	// Move
	//----------------------------------------------
	class Move : public Component
	{
	public:
		DECLARE_FAMILY_ID("Move");
		Move() {}
		virtual ~Move() {}
		virtual void Render(REAL32 tm) = 0;
	};

	class MoveDomiDinoScrollMode : public Move
	{
	public:
		DECLARE_COMPONENT_ID("MoveDomiDinoScrollMode");
		MoveDomiDinoScrollMode(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~MoveDomiDinoScrollMode() {}
		virtual void Render(REAL32 tm);

	private:
		INT32 m_slotIdx;
	};

	class MoveDomiDinoFullMode : public Move
	{
	public:
		DECLARE_COMPONENT_ID("MoveDomiDinoFullMode");
		MoveDomiDinoFullMode(INT32 slotIdx) : m_slotIdx(slotIdx) {}
		virtual ~MoveDomiDinoFullMode() {}
		virtual void Render(REAL32 tm);

	private:
		INT32 m_slotIdx;
	};

	//----------------------------------------------
	// RenderMoveByMode
	//----------------------------------------------
	enum MINIMAP_MODE { MINIMAP_SCROLL_MODE, MINIMAP_FULL_MODE };

	class RenderMoveByMode : public Component
	{
	public:
		DECLARE_FAMILY_ID("RenderMoveByMode");
		RenderMoveByMode() {}
		virtual ~RenderMoveByMode();
		virtual void Render(REAL32 tm, MINIMAP_MODE mode);
		virtual void Register(GameObject* o, INT32 slotIdx) = 0;

	protected:
		typedef i3::unordered_map<INT32, Move*> lookupTable;
		lookupTable m_moves;
	};

	class RenderMoveByModeDomiDino : public RenderMoveByMode
	{
	public:
		DECLARE_COMPONENT_ID("RenderMoveByMode");
		RenderMoveByModeDomiDino() {}
		virtual ~RenderMoveByModeDomiDino() {}
		virtual void Register(GameObject* o, INT32 slotIdx);
	};

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
}