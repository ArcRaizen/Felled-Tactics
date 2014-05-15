/*
	Every Unit's set of 3 items.
*/
#pragma once
#ifndef INVENTORY_H
#define INVENTORY_H
#include "Weapon.h"
#include "Staff.h"
#include "Consumable.h"

class Inventory
{
public:
	Inventory(void);
	~Inventory(void);

	// Inventory Management
	Item*		operator[](int index);		// Access an item in the inventory
	Inventory&	operator+=(Item* i);		// Add item to inventory
	Inventory&	operator-=(Item* i);		// Remove item from inventory
	Inventory&	operator-=(int index);		// Remove item from inventory slot

	bool	HasConsumable();

private:
	Item*	inventory[3];
	int		numInventory;
};
#endif

