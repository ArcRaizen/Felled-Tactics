#pragma once
#ifndef COMBATCALC_H
#define COMBATCALC_H

#ifndef LEVEL_H
//#include "Level.h"
#endif
#ifndef UNIT_H
#include "Unit.h"
#endif

#include "Luna.h"
#include <vector>

class Level;
class CombatCalculator
{
public:
	CombatCalculator(void);
	~CombatCalculator(void);

	void	CalculateCombat();
	void	CalculateCombat(lua_State* L);
	void	DoCombat();
	void	DefenderDied();
	void	AttackerDied();
	void	Reset(bool onlyDefender = false);
	void	ResetDefender();
	void	SetCombatParametersAttacker(int attPhys, int attMag, int attHit, int attAvoid, int attNumHits);
	void	SetCombatParametersDefender(int defPhys, int defMag, int defHit, int defAvoid, int defNumHits);
	void	SetCombatTextCallback(Level* l, void (Level::*func)(Position, Position, const char*, int));

	int		Update(float dt);
	int		Update2(float dt, lua_State* L);

#pragma region Properties
	__declspec(property(get=GetAttacker, put=SetAttacker)) Unit* Attacker;	Unit* GetAttacker();	void SetAttacker(Unit* a);
	__declspec(property(get=GetDefender, put=SetDefender)) Unit* Defender;	Unit* GetDefender();	void SetDefender(Unit* d);
	__declspec(property(get=GetDamageA)) int DamageA;		int		GetDamageA();
	__declspec(property(get=GetDaamgeD)) int DamageD;		int		GetDamageD();
	__declspec(property(get=GetAccuracyA)) float AccuracyA;	float	GetAccuracyA();
	__declspec(property(get=GetAccuracyD)) float AccuracyD; float	GetAccuracyD();
#pragma endregion

private:
	Unit*	attacker;		// attacking unit
	Unit*	defender;		// defending unit
	bool	doCombat;		// is combat occuring right now? (do update function?)

	// Combat Text displays
	Level*	level;
	char	battleText[10];				// buffer to hold text form of damage numbers during combat
	void	(Level::*CreateCombatText)(Position, Position, const char*, int);

	// Combat timer values
	static const float  COMBAT_TEXT_LIFE;	// how long each combat text entry lasts / is displayed
	static const float  PRE_COMBAT_WAIT;	// time after combat is initiated before attacker first strikes
	static const float	MID_COMBAT_WAIT;	// time after attacker first strikes before defender retaliates
	static const float	POST_COMBAT_WAIT;	// time after final attack before combat completely ends
	static const float  MULTI_HIT_WAIT;		// time between multiple hits from a single combatant

	// Default Combat scripts
	static const std::string BASE_CALC_COMBAT_SCRIPT;
	static const std::string BASE_COMBAT_ATTACKER_STRIKES_SCRIPT;
	static const std::string BASE_COMBAT_DEFENDER_STRIKES_SCRIPT;

	// Combat Phase + timer
	float combatTimer;	// timer to keep track of when each combat phase initiates/ends
	int	  combatPhase;	// keep track of which combat phase we're in during update (for a switch statement)
	
	// Combat results (for return values)
	bool defenderDied;
	bool attackerDied;
	
	// Battle Statistics
	int		basePhysicalDamageA, basePhysicalDamageD;		// Physical Damage delt by attacker
	int		baseMagicalDamageA, baseMagicalDamageD;			// Magical damage delt by attacker
	float	physicalModifier, magicalModifier;				// Damage modifiers during combat
	int		numAttackerHits, numDefenderHits;				// Num of times each combatant attacks (default 1)
	int		attHitCount, defHitCount;						// Counter to track number of attacks on each side
	float	hitA, hitD;										// Percent chance to hit with attack
	float	avoidA, avoidD;									// Perecent chance to dodge attack
	int		range;

	// Derived Statistics
	int		physicalDamageA, physicalDamageD;			// Physical Damage after factoring defence
	int		magicalDamageA, magicalDamageD;				// Magical Damage after factoring resistance
	float	accuracyA, accuracyD;						// Overall accuracy of attack (hit - avoid)
	int		RNG;										// Number 1-100 rolled for hit chance
};
#endif
