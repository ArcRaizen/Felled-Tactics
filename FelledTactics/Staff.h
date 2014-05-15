#pragma once
#ifndef STAFF_H
#define STAFF_H
#include "Item.h"

class Staff : public Item
{
public:
	Staff(void);
	~Staff(void);

	int		CalculateBaseDamage();
};
#endif

