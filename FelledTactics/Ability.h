#pragma once
#ifndef ABILITY_H
#define ABILITY_H

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

#include <fstream>
#include <stdio.h>
#include <vector>
#pragma endregion

class Ability
{
protected:
	Ability(const char* name, int rank);
	Ability(const char* name, json_spirit::mObject abilityMap, int rank);

public:
	static SmartPointer<Ability> Create(const char* name, int rank);
	static SmartPointer<Ability> Create(const char* name, json_spirit::mObject abilityMap, int rank);
	~Ability(void);

	enum class	Type {Action, Battle, Passive};						// Skills activated in place of combat, skills that boost regular combat, or skills that provide a constant passive bonus
	enum class	EffectType {Physical, Magical, Heal, Status, None};	// Skills do Physical damage, Magical Damage, Heal a unit, Apply a Status or do no damage
	enum class	CastType {SelfCast, Ally, Enemy, Free};				// Type of units the skill can be used on

	void	Activate(lua_State* L, Position target, Position source);
	bool	RankUp();
	void	SetAPCost(json_spirit::mObject abilityMap);
	void	SetRange(json_spirit::mObject abilityMap);
	void	SetAoE(json_spirit::mObject abilityMap);

	__declspec(property(get=GetName)) char* Name;					char* GetName();
	__declspec(property(get=GetRank)) int Rank;						int GetRank();
	__declspec(property(get=GetType)) Type AbilityType;				Type GetType();
	__declspec(property(get=GetCost)) int APCost;					int GetCost();
	__declspec(property(get=GetCastType)) CastType AbilityCastType;	CastType GetCastType();
	__declspec(property(get=GetRange,put=SetRange))int Range;		int	GetRange();
	const float* GetTimers() const;
	bool HasDynamicAoE() const;
	vector<Position> GetAoE(Position p = Position(0,0));
	vector<Position> GetDynamicAoERange();

private:
	template <typename T> friend class SmartPointer;
	unsigned int		pointerCount;	// Running count of copies of a pointer of this class. Used in conjunction with SmartPointer

	char				name[15];
	int					rank, maxRank;
	Type				type;
	EffectType			effect;
	CastType			castType;
	int					apCost;			// array of the AP Costs for this ability at each rank
	int					range;			// array of the Cast Ranges for this at each rank
	float				castTimers[4];	//
	float				proficiency;
	std::string			script;			// script file to run for this ability
	map<Position, vector<Position>>	areaOfEffect;	// Map of all possible Areas of Effect for this ability
													// If an Ability has different AoE's based on where it is cast (usually rotational around the caster),
														// each AoE is saved in the map with the index being the cast position relative to the caster
													// An ability with a static AoE that doesn't change depending on cast location is at index [Position(0,0)]
													// Each AoE is a list(std::vector) of Positions relative to the casting location of tiles the ability will affect
};

inline SmartPointer<Ability> Ability::Create(const char* name, int rank) { return new Ability(name, rank); }
inline SmartPointer<Ability> Ability::Create(const char* name, json_spirit::mObject abilityMap, int rank) { return new Ability(name, abilityMap, rank); }
typedef SmartPointer<Ability> AbilityPtr;
#endif
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    