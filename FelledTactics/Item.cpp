#include "StdAfx.h"
#include "Item.h"


Item::Item(void)
{
}


Item::~Item(void)
{
}

#pragma region Properties
char* Item::GetName() { return name; }
void  Item::SetName(char* n) { strcpy(name, n); }
Item::ItemClass Item::GetClass() { return itemClass; }
#pragma endregion