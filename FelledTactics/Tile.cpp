#include "StdAfx.h"
#include "Tile.h"

#pragma region Highlight Color Initializations
D3DXVECTOR4 Tile::highlightNone = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
D3DXVECTOR4 Tile::highlightAllyMove = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
D3DXVECTOR4 Tile::highlightAllyMoveAlly = D3DXVECTOR4(0.0f, 1.0f, 1.0f, 1.0f);
D3DXVECTOR4 Tile::highlightAllySkillRange = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
D3DXVECTOR4 Tile::highlightAllySkillAoE = D3DXVECTOR4(1.0f, 0.647f, 0.0f, 1.0f);
D3DXVECTOR4 Tile::highlightEnemyMove = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
#pragma endregion

Tile::Tile(WCHAR* filename, int layer, int width, int height, int posX, int posY, Level* l, Position gp) :
	VisualElement(filename, layer, width, height, posX, posY), level(l), gridPosition(gp)
{
	
	status = Empty;
	mark = None;
}


Tile::~Tile(void)
{
}

bool Tile::IsObstructed()
{
	return !status == Empty /*|| mark == None*/;
}

bool Tile::IsObstructedPlayer()
{
	/*if(IsObstructed())
		return true;
	else*/
		return status == EnemyUnit || status == AllyFelled;
}

bool Tile::IsObstructedEnemy()
{
	/*if(IsObstructed())
		return true;
	else*/
		return status == AllyUnit || status == AllyFelled;
}

#pragma region MouseEvents
void Tile::MouseUp()
{
	// Only do MouseUp event if a down even already occured
	if(mouseDown)
	{
		mouseDown = false;
		level->SetSelectedTile(gridPosition);
	}
}
void Tile::MouseOver() 
{ 
	mouseEntered = true;
	level->SetHoveredTile(gridPosition);
}
void Tile::MouseOut() 
{ 
	mouseEntered = false; 
	mouseDown = false;
}
#pragma endregion

#pragma region Properties
void Tile::SetStatus(Tile::Status s) { status = s; }
Tile::Status Tile::GetStatus() { return status; }
Tile::Mark Tile::GetMark() { return mark; }
void Tile::SetPrevMark(Tile::Mark s) { prevMark = s; }
Tile::Mark Tile::GetPrevMark() { return prevMark; }
void Tile::SetMark(Tile::Mark m)
{
	mark = m; 

	// Change how the tile is being drawn by the shader (just a highlight color at this point)
	switch(mark)
	{
		case Tile::Mark::AllyMove:
			highlightColor = highlightAllyMove;

			if(status == Status::AllyUnit)
				highlightColor = highlightAllyMoveAlly;
			break;
		case Tile::Mark::AllyMovePath:
			highlightColor = highlightAllySkillRange;
			break;
		case Tile::Mark::AllyMovePathFail:
			highlightColor = highlightEnemyMove;
			break;
		case Tile::Mark::AllySkillAoE:
			highlightColor = highlightAllySkillAoE;
			break;
		case Tile::Mark::AllySkillRange:
			highlightColor = highlightAllySkillRange;
			break;
		case Tile::Mark::EnemyMove:
			highlightColor = highlightEnemyMove;
			break;
		case Tile::Mark::None:
			highlightColor = highlightNone;
			break;
	}
}
#pragma endregion