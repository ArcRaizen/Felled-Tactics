#include "StdAfx.h"
#include "CombatCalculator.h"


CombatCalculator::CombatCalculator(Unit* a, Unit* d, int r)
{
	attacker = a;
	defender = d;
	range = r;
}

CombatCalculator::CombatCalculator(void)
{
	attacker = NULL;
	defender = NULL;
	range = 0;
}

CombatCalculator::~CombatCalculator(void)
{
}

// Calculate statistics for Combat
void CombatCalculator::CalculateCombat()
{
	if(attacker == NULL || defender == NULL)
		return;

	// Calculate damage delt by attacker
	attacker->CalculateCombatDamage(basePhysicalDamageA, baseMagicalDamageA, range);
	physicalDamageA = basePhysicalDamageA - defender->Defence;
	magicalDamageA = baseMagicalDamageA - defender->Resistance;

	// Calculate if attacker will hit defender
	hitA = (float)(attacker->Skill * 2) + ((float)attacker->Agility * 0.5f) + /*WEAPON HIT*/60;
	avoidA = (float)(defender->Agility * 2) + ((float)defender->Skill * 0.5f);
	accuracyA = hitA > avoidA ? hitA - avoidA : 0;

	// Calculate damage delt by defender
	defender->CalculateCombatDamage(basePhysicalDamageD, baseMagicalDamageD, range);
	physicalDamageD = basePhysicalDamageD - defender->Defence;
	magicalDamageD = baseMagicalDamageD - defender->Resistance;

	// Calculate if defender will hit attacker
	hitD = (float)(defender->Skill * 2) + ((float)defender->Agility * 0.5f) + /*WEAPON HIT*/60;
	avoidD = (float)(attacker->Agility *2) + ((float)attacker->Skill * 0.5f);
	accuracyD = hitD > avoidD ? hitD - avoidD : 0;
}

// Do combat between 2 units. Return is who died: 0 = Neither, 1 = Attacker, 2 = Defender
int CombatCalculator::DoCombat()
{
	if(attacker == NULL || defender == NULL)
		return -1;
	
	// Attacker hits defender
	RNG = rand() % 100;
	if(RNG < accuracyA)
	{
		// Defender takes damage
		defender->Health -= (physicalDamageA - defender->Defence) + (magicalDamageA - defender->Resistance);

		// Defender killed
		if(defender->Health == 0)
			return 2;
	}
	else
	{
		// WHIFF
	}

	// Defender retaliates against attacker
	RNG = rand() % 100;
	if(RNG < accuracyD)
	{
		// Attacker takes damage
		attacker->Health -= (physicalDamageD - attacker->Defence) + (magicalDamageD - attacker->Resistance);

		// Attacker killed
		if(attacker->Health == 0)
			return 1;
	}
	else
	{
		// WHIFF
	}

	// No one died
	return 0;
}

void CombatCalculator::Reset()
{
	attacker = NULL;
	defender = NULL;
	basePhysicalDamageA = basePhysicalDamageD = 0;
	baseMagicalDamageA = baseMagicalDamageD = 0;
	physicalDamageA = physicalDamageD = 0;
	magicalDamageA = magicalDamageD = 0;
	hitA = hitD = 0.0f;
	avoidA = avoidD = 0.0f;
	accuracyA = accuracyD = 0;
	RNG = -1;
}

#pragma region Properties
void CombatCalculator::SetAttacker(Unit* a) { attacker = a; }
void CombatCalculator::SetDefender(Unit* d) { defender = d; }
void CombatCalculator::SetRange(int r) { range = r; }
int CombatCalculator::GetDamageA() { return physicalDamageA + magicalDamageA; }
int CombatCalculator::GetDamageD() { return physicalDamageD + magicalDamageD; }
float CombatCalculator::GetAccuracyA() { return accuracyA; }
float CombatCalculator::GetAccuracyD() { return accuracyD; }
#pragma endregion