#pragma once
#ifndef WEAPON_H
#define WEAPON_H
#include "Item.h"

class Weapon : public Item
{
public:
	enum WeaponClass {Sword, Lance, Axe, Bow, Shield, Scythe};

	Weapon(WeaponClass c, int s, int cr, int r);
	Weapon(void);
	~Weapon(void);

	int			CalculateBaseDamage();
	WeaponClass	GetWeaponClass();

private:
	WeaponClass		weaponClass;
	int				strength;
	int				crit;
	int				range;
};
#endif
