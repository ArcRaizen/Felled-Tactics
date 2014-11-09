#pragma once
#ifndef LEVELENTITY_H
#define LEVELENTITY_H

class LevelEntity
{
public:
	virtual ~LevelEntity(){};
	virtual void NewTurn(lua_State* L) = 0;
};
#endif