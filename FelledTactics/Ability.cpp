#include "stdafx.h"
#include "Ability.h"


Ability::Ability(const char* name)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("../FelledTactics/Skills.xml");
	tinyxml2::XMLElement* abilityList = doc.FirstChildElement();
	tinyxml2::XMLElement* ability = abilityList->FirstChildElement();
	const char* value = ability->Attribute("Name");

	// Loop through all skills until correct skill is located
	while(strcmp(value, name) && ability != abilityList->LastChildElement())
	{
		ability = ability->NextSiblingElement();
		value = ability->Attribute("Name");
	}

	// Set Skill from XML file
	// 1) Skill Type
	tinyxml2::XMLElement* element = ability->FirstChildElement();
	value = element->GetText();
	if(!strcmp(value, "Action"))
		type = Action;
	else if(!strcmp(value, "Battle"))
		type = Battle;
	else	// Passive
		type = Passive;

	// 2) Effect Type
	element = element->NextSiblingElement();
	value = element->GetText();
	if(!strcmp(value, "Physical"))
		effect = Physical;
	else if(!strcmp(value, "Magical"))
		effect = Magical;
	else if(!strcmp(value, "Heal"))
		effect = Heal;
	else if(!strcmp(value, "Status"))
		effect = Status;
	else	// None
		effect = None;


	// 3) Cast Type
	element = element->NextSiblingElement();
	value = element->GetText();
	if(!strcmp(value, "SelfCast"))
		castType = SelfCast;
	else if(!strcmp(value, "Ally"))
		castType = Ally;
	else if(!strcmp(value, "Enemy"))
		castType = Enemy;
	else	// None
		castType = Free;

	// 4) AP Cost
	element = element->NextSiblingElement();	// move to next element
	stringstream stream;
	stream << element->GetText();				// get ap cost			
	stream >> apCost;							// convert to int and save
	stream.clear();

	// 5) Range
	element = element->NextSiblingElement();	// move to next element
	stream << element->GetText();				// get range		
	stream >> range;							// convert to int and save
	stream.clear();

	// 6) AoE
	int num;
	string len, aoe;
	element = element->NextSiblingElement();	// move to next element
	stream << element->GetText();				// get aoe string
	stream >> len;								// convert to string
	stream.clear();
	stream << len.substr(0, len.find("|"));		// find first number (number of Positions in the element)
	stream >> num;								// conver to int and save
	stream.clear();

	if(num > 0)
	{
		value = len.substr(len.find("|") + 1).c_str();	// Get list of positions
		int a, b, i;
		for(i = 0; i < num*4; i+=4)
		{
			a = value[i] - 48;		// Convert to int
			b = value[i+2] - 48;	//	but 0 makes it tricky --V
			areaOfEffect.push_back(Position(a == -48 ? 0 : a, b == -48 ? 0 : b));
		}
	}


	// 7) Script
	element = element->NextSiblingElement();	// move to next element
	value = element->GetText();					// get script
	script.clear();
	if(value != NULL)
		script = value;

	int x = 5;
	x++;
}


Ability::~Ability(void)
{
}

void Ability::Activate(lua_State* L)
{
	// Create tables of tiles where skill  has an effect
	if(areaOfEffect.size() > 0)
	{
	/*	stringstream ss(stringstream::in | stringstream::out);
		lua_createtable(L, areaOfEffect.size(), 0);	// AoE
		for(int i = 0; i < areaOfEffect.size(); i++)
		{
			ss << i;
			lua_createtable(L, 2, 0);
			lua_pushinteger(L, target.x + areaOfEffect[i].x);
			lua_setfield(L, -2, "x");
			lua_pushinteger(L, target.y + areaOfEffect[i].y);
			lua_setfield(L, -2, "y");
			lua_setfield(L, -2, ss.str().c_str());
		}

		lua_setglobal(L, "AreaOfEffect");*/
	}
	else	// Self Cast or Single Target
	{
	/*	lua_createtable(L, 2, 0);
		lua_pushinteger(L, target.x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, target.y);
		lua_setfield(L, -2, "y");
		lua_setfield(L, -2, "0");
		lua_setglobal(L, "AreaOfEffect");*/
	}

	lua_settop(L, 0);
	int x = lua_gettop(L);
	luaL_dofile(L, script.c_str());
}

int Ability::GetCost() { return apCost; }
Ability::CastType Ability::GetType() { return castType; }
int Ability::GetRange() { return range; }                                          