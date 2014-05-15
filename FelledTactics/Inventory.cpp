#include "StdAfx.h"
#include "Inventory.h"


Inventory::Inventory(void) : numInventory(0)
{
	inventory[0] = NULL;
	inventory[1] = NULL;
	inventory[2] = NULL;
}


Inventory::~Inventory(void)
{
}

Item* Inventory::operator[](int index)
{
	return inventory[index];
}

Inventory& Inventory::operator+=(Item* i)
{
	if(numInventory < 3)
		inventory[numInventory++] = i;
	else
	{
		// Cannot add item
	}

	return *this;
}

Inventory& Inventory::operator-=(Item* i)
{
	// Attempt to locate item in Inventory
	int index = -1;
	if(inventory[0] == i)
		index = 0;
	else if(inventory[1] == i)
		index = 1;
	else if(inventory[2] == i)
		index = 2;

	// Remove item - uses fall-through
	switch(index)
	{
		case 0:			// Item found in front of inventory
			inventory[0] = inventory[1];
		case 1:			// Item found in middle of inventory
			inventory[1] = inventory[2];
		case 2:			// Item found at end of inventory
			inventory[2] = NULL;
			break;
		default:		// could not locate item
			// Do things because we cannot find it
			break;
	}

	return *this;
}

Inventory& Inventory::operator-=(int index)
{
	// If an item exists in this inventory slot
	if(index > -1 && index < 3 && inventory[index] != NULL)
	{
		// Remove item - uses fall-through
		switch(index)
		{
			case 0:			// Item found in front of inventory
				inventory[0] = inventory[1];
			case 1:			// Item found in middle of inventory
				inventory[1] = inventory[2];
			case 2:			// Item found at end of inventory
				inventory[2] = NULL;
				break;
			default:		// could not locate item
				// Do things because we cannot find it
				break;
		}
	}

	return *this;
}

// Determines if a Unit has a consumable Item in their inventory. Whether the item is usable or not is not considered.
bool Inventory::HasConsumable()
{
	if(inventory[0] == NULL || inventory[0]->Class != Item::Consumable)
		return false;
	if(inventory[1] == NULL || inventory[1]->Class != Item::Consumable)
		return false;
	if(inventory[2] == NULL || inventory[2]->Class != Item::Consumable)
		return false;

	return true;
}