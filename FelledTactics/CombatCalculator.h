#pragma once
#ifndef COMBATCALC_H
#define COMBATCALC_H
#ifndef UNIT_H
#include "Unit.h"
#endif

class CombatCalculator
{
public:
	CombatCalculator(Unit* a, Unit* d, int ra);
	CombatCalculator(void);
	~CombatCalculator(void);

	void	CalculateCombat();
	int		DoCombat();
	void	Reset();

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
	Unit*	attacker;
	Unit*	defender;

	// Battle Statistics
	int		basePhysicalDamageA, basePhysicalDamageD;		// Physical Damage delt by attacker
	int		baseMagicalDamageA, baseMagicalDamageD;			// Magical damage delt by attacker
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
