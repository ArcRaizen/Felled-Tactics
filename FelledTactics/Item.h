#pragma once
#ifndef ITEM_H
#define ITEM_H

#ifndef CORE_H
#include "Core.h"
#endif

class Item
{
public:
	Item(void);
	~Item(void);

	enum ItemClass {Weapon, Staff, Consumable};

	virtual int		CalculateBaseDamage() = 0;

	__declspec(property(put=SetName, get=GetName)) char* Name;	void SetName(char* n);	char* GetName();
	__declspec(property(get=GetClass)) ItemClass Class;			ItemClass GetClass();

protected:
	char		name[15];
	ItemClass	itemClass;
};
#endif
