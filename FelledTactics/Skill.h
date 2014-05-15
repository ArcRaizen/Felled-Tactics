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

class Skill
{
public:
	Skill(const char* name);
	~Skill(void);

	enum	Type {Action, Battle, Passive};						// Skills activated in place of combat, skills that boost regular combat, or skills that provide a constant passive bonus
	enum	AffectType {Physical, Magical, Heal, Status, None};	// Skills do Physical damage, Magical Damage, Heal a unit, Apply a Status or do no damage
	enum	CastType {SelfCast, Ally, Enemy, Free};				// Type of units the skill can be used on

	__declspec(property(get=GetCost)) int APCost;				int GetCost();
	__declspec(property(get=GetType)) CastType SkillCastType;	CastType GetType();
	__declspec(property(get=GetRange))int Range;				int	GetRange();

private:
	Type				type;
	AffectType			affect;
	CastType			castType;
	Position			target;
	int					range;
	vector<Position>	areaOfEffect;	// List of all tiles the spell is cast on
											// Entries are a non-negative distance from initial target
	int					apCost;
	const char*			script;
};
#endif
