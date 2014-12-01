#pragma once
#ifndef SKILL_H
#define SKILL_H

#pragma region Includes
#ifndef VISUAL_H
#include "VisualElement.h"
#endif
#ifndef POSITION_S
#include "Position.h"
#endif
#ifndef CORE_H
#include "Core.h"
#endif

#include <stdio.h>
#include "tinyxml2.h"
#include <vector>
#pragma endregion

class Ability
{
public:
	Ability(const char* name);
	~Ability(void);

	enum	Type {Action, Battle, Passive};						// Skills activated in place of combat, skills that boost regular combat, or skills that provide a constant passive bonus
	enum	EffectType {Physical, Magical, Heal, Status, None};	// Skills do Physical damage, Magical Damage, Heal a unit, Apply a Status or do no damage
	enum	CastType {SelfCast, Ally, Enemy, Free};				// Type of units the skill can be used on

	void	Activate(lua_State* L, Position target, Position source);
	bool	RankUp();

	__declspec(property(get=GetName)) char* Name;					char* GetName();
	__declspec(property(get=GetRank)) int Rank;						int GetRank();
	__declspec(property(get=GetType)) Type AbilityType;				Type GetType();
	__declspec(property(get=GetCost)) int APCost;					int GetCost();
	__declspec(property(get=GetCastType)) CastType AbilityCastType;	CastType GetCastType();
	__declspec(property(get=GetRange,put=SetRange))int Range;		int	GetRange();
	__declspec(property(get=GetAOE)) vector<Position> AoE;			vector<Position> GetAOE();
	const float* GetTimers() const;

private:
	char				name[15];
	int					rank, maxRank;
	Type				type;
	EffectType			effect;
	CastType			castType;
	int*				apCosts;		// array of the AP Costs for this ability at each rank
	int*				ranges;			// array of the Cast Ranges for this at each rank
	vector<Position>*	areasOfEffect;	// array of [lists(std::vectors) of all tiles the spell is cast] on at each rank
	float				castTimers[4];	//
	float				proficiency;
	std::string			script;			// script file to run for this ability
};
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    