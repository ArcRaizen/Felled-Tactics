#pragma once
#ifndef WEAPON_H
#define WEAPON_H
#include "Item.h"

class Weapon : public Item
{
public:
	Weapon(void);
	~Weapon(void);

	enum WeaponClass {Sword, Lance, Axe, Bow, Shield, Scythe};

	int			CalculateBaseDamage();
	WeaponClass	GetWeaponClass();

private:
	WeaponClass		weaponClass;
	int				strength;
	int				crit;
	int				range;
};
#endif
