#include "StdAfx.h"
#include "CombatCalculator.h"

const float CombatCalculator::COMBAT_TEXT_LIFE = 1.0f;
const float CombatCalculator::PRE_COMBAT_WAIT = 0.5f;
const float CombatCalculator::MID_COMBAT_WAIT = 1.0f;
const float CombatCalculator::POST_COMBAT_WAIT = 2.0f;
const D3DXVECTOR3 CombatCalculator::COMBAT_TEXT_MOVE = D3DXVECTOR3(25.0f, -25.0f, 0.0f);

CombatCalculator::CombatCalculator(void)
{
	attacker = NULL;
	defender = NULL;
	doCombat = false;
	range = 0;
	combatTimer = 0.0f;
}

CombatCalculator::~CombatCalculator(void)
{
}

// Calculate statistics for Combat
void CombatCalculator::CalculateCombat()
{
	if(attacker == NULL || defender == NULL)
		return;

	// Get range between combaters
	range = attacker->UnitPosition.DistanceTo(defender->UnitPosition);

	// Calculate damage delt by attacker
	attacker->CalculateCombatDamage(basePhysicalDamageA, baseMagicalDamageA, range);
	physicalDamageA = defender->Defense > basePhysicalDamageA ? 0 : basePhysicalDamageA - defender->Defense;
	magicalDamageA = defender->Resistance > baseMagicalDamageA ? 0 : baseMagicalDamageA - defender->Resistance;

	// Calculate if attacker will hit defender
	hitA = (float)(attacker->Skill * 2) + ((float)attacker->Agility * 0.5f) + /*WEAPON HIT*/60;
	avoidA = (float)(defender->Agility * 2) + ((float)defender->Skill * 0.5f);
	accuracyA = hitA > avoidA ? hitA - avoidA : 0;

	// Calculate damage delt by defender
	defender->CalculateCombatDamage(basePhysicalDamageD, baseMagicalDamageD, range);
	physicalDamageD = defender->Defense > basePhysicalDamageD ? 0 : basePhysicalDamageD - defender->Defense;
	magicalDamageD = defender->Resistance > baseMagicalDamageD ? 0 : baseMagicalDamageD - defender->Resistance;

	// Calculate if defender will hit attacker
	hitD = (float)(defender->Skill * 2) + ((float)defender->Agility * 0.5f) + /*WEAPON HIT*/60;
	avoidD = (float)(attacker->Agility *2) + ((float)attacker->Skill * 0.5f);
	accuracyD = hitD > avoidD ? hitD - avoidD : 0;
}

// Do combat between 2 units. Return is who died: 0 = Neither, 1 = Attacker, 2 = Defender
void CombatCalculator::DoCombat()
{
	if(attacker == NULL || defender == NULL)
		return;
	
	doCombat = true;
	combatTimer = 0.0f;
	combatPhase = 1;
	defenderDied = attackerDied = false;
}

void CombatCalculator::Reset(bool onlyDefender/* = false*/)
{
	if(!onlyDefender)
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

	doCombat = false;
}

void CombatCalculator::ResetDefender()
{
	Reset(true);
}

// Complete a phase of combat between 2 units and return result
// 0 = no one died (yet), 1 = attacker died, 2 = defender died, 3 = no deaths
int CombatCalculator::Update(float dt)
{
	// Calc updates to each combat text entry now
	combatTextMove = COMBAT_TEXT_MOVE * (dt/COMBAT_TEXT_LIFE);
	alphaChange = -dt / COMBAT_TEXT_LIFE;

	if(!doCombat)
		return -1;

	switch(combatPhase)
	{
		case 1:
			// Wait period before combat occurs
			if(combatTimer < PRE_COMBAT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}

			// Attacker hits defender
			int damage;
			RNG = rand() % 100;
			if(RNG < accuracyA)
			{
				// Defender takes damage
				damage = physicalDamageA + magicalDamageA;
				defender->Health -= damage;
				itoa(damage, battleText, 10);
				combatText.push_back(new TextElement(0, 100, 100, defender->GetCorner(), battleText, D3DXCOLOR(1,0,0,1)));

				// Defender killed
				if(defender->Health == 0)
					defenderDied = true;
			}
			else
			{
				// WHIFF
				combatText.push_back(new TextElement(0, 100, 100, defender->GetCorner(), "MISS!", D3DXCOLOR(1,0,0,1)));
			}

			// Proceed to next combat phase
			combatTimer = 0.0f;
			combatPhase = 2;
			return 0;
		case 2:
			// Wait period before defender retaliates
			if(combatTimer < MID_COMBAT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}

			// Check if defender died
			if(defenderDied)
				return 2;

			// Defender retaliates against attacker
			if(defender->AttackRange >= range) // Defender might not be able to retaliate
			{
				RNG = rand() % 100;
				if(RNG < accuracyD)
				{
					// Attacker takes damage
					damage = physicalDamageD + magicalDamageD;
					attacker->Health -= damage;
					itoa(damage, battleText, 10);
					combatText.push_back(new TextElement(0, 100, 100, attacker->GetCorner(), battleText, D3DXCOLOR(1,0,0,1)));

					// Attacker killed
					if(attacker->Health == 0)
						attackerDied = true;
				}
				else
				{
					// WHIFF
					combatText.push_back(new TextElement(0, 100, 100, attacker->GetCorner(), "MISS!", D3DXCOLOR(1,0,0,1)));
				}
			}

			combatTimer = 0.0f;
			combatPhase = 3;
			return 0;
		case 3:
			// Wait period before combat finishes
			if(combatTimer < POST_COMBAT_WAIT)
			{
				combatTimer += dt;
				return 0;
			}

			// Combat has ended
			doCombat = false;
			combatTimer = 0.0f;
			combatPhase = 0;

			// Check if attacker died
			if(attackerDied)
				return 1;

			// No one died
			return 3;
		default: return 0;
	}
}

void CombatCalculator::Draw()
{
	for(int i = 0; i < combatText.size(); i++)
	{
		combatText[i]->Draw();
		combatText[i]->Translate(combatTextMove);
		combatText[i]->ChangeAlpha(alphaChange);

		if(combatText[i]->NoAlpha())
			combatText.erase(combatText.begin() + i--);
	}

	Direct3D::DrawTextReset();
}

#pragma region Properties
void CombatCalculator::SetAttacker(Unit* a) { attacker = a; }
void CombatCalculator::SetDefender(Unit* d) { defender = d; }
int CombatCalculator::GetDamageA() { return physicalDamageA + magicalDamageA; }
int CombatCalculator::GetDamageD() { return physicalDamageD + magicalDamageD; }
float CombatCalculator::GetAccuracyA() { return accuracyA; }
float CombatCalculator::GetAccuracyD() { return accuracyD; }
#pragma endregion