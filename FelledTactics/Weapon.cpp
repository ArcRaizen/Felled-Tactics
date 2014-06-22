#include "StdAfx.h"
#include "Weapon.h"


Weapon::Weapon(WeaponClass c, int s, int cr, int r)
{
	weaponClass = c;
	strength = s;
	crit = cr;
	range = r;
	itemClass = ItemClass::Weapon;
}

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