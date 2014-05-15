#pragma once
#ifndef CONSUMABLE_H
#define CONSUMABLE_H
#include "Item.h"

class Consumable : public Item
{
public:
	Consumable(void);
	~Consumable(void);

	int		CalculateBaseDamage();
};
#endif

