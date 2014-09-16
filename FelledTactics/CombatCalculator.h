#pragma once
#ifndef COMBATCALC_H
#define COMBATCALC_H

#ifndef LEVEL_H
//#include "Level.h"
#endif
#ifndef UNIT_H
#include "Unit.h"
#endif
#ifndef TEXTELEMENT_H
#include "TextElement.h"
#endif

#include <vector>

class Level;
class CombatCalculator
{
public:
	CombatCalculator(void);
	~CombatCalculator(void);

	void	CalculateCombat();
	void	DoCombat();
	void	Reset(bool onlyDefender = false);
	void	ResetDefender();
	void	SetCombatModifiers(float physMod, float magMod, int numAttHits, int numDefHits);
	void	SetCombatTextCallback(Level* l, void (Level::*func)(int, int, Position, const char*, D3DXCOLOR, float, D3DXVECTOR3, float));

	int		Update(float dt);

#pragma region Properties
	__declspec(property(put=SetAttacker)) Unit* Attacker;	void	SetAttacker(Unit* a);
	__declspec(property(put=SetDefender)) Unit* Defender;	void	SetDefender(Unit* d);
	__declspec(property(put=SetRange)) int Range;			void	SetRange(int r);
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
	void	(Level::*CreateCombatText)(int, int, Position, const char*, D3DXCOLOR, float, D3DXVECTOR3, float);

	// Combat timer values
	static const float  COMBAT_TEXT_LIFE;	// how long each combat text entry lasts / is displayed
	static const float  PRE_COMBAT_WAIT;	// time after combat is initiated before attacker first strikes
	static const float	MID_COMBAT_WAIT;	// time after attacker first strikes before defender retaliates
	static const float	POST_COMBAT_WAIT;	// time after final attack before combat completely ends
	static const float  MULTI_HIT_WAIT;		// time between multiple hits from a single combatant
	static const D3DXVECTOR3 COMBAT_TEXT_MOVE;	// direction combat text moves during its life

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
