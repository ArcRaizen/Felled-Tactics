#include "StdAfx.h"
#include "Weapon.h"


Weapon::Weapon(void)
{
}


Weapon::~Weapon(void)
{
}

int Weapon::CalculateBaseDamage()
{
	return strength;
}

Weapon::WeaponClass Weapon::GetWeaponClass()
{
	return weaponClass;
}