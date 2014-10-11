#include "stdafx.h"
#include "Ability.h"


Ability::Ability(const char* name)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("../FelledTactics/Abilities.xml");
	tinyxml2::XMLElement* abilityList = doc.FirstChildElement();
	tinyxml2::XMLElement* ability = abilityList->FirstChildElement();
	const char* value = ability->Attribute("Name");

	// Loop through all skills until correct skill is located
	while(strcmp(value, name) && ability != abilityList->LastChildElement())
	{ 
		ability = ability->NextSiblingElement();
		value = ability->Attribute("Name");
	}

	// Save name
	strcpy(this->name, name);

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
	string aoe;
	element = element->NextSiblingElement();	// move to next element
	stream << element->GetText();				// get aoe string
	stream >> aoe;								// convert to string
	stream.clear();
	stream << aoe.substr(0, aoe.find("|"));		// find first number (number of Positions in the element)
	stream >> num;								// conver to int and save
	stream.clear();

	if(num > 0)
	{
		aoe = aoe.substr(aoe.find("|") + 1).c_str();	// Get list of positions
		int a, b, x;
		for(int i = 0; i < num; i++)
		{
			x = aoe.find("|");											// Find index of first delimeter
			a = atoi(aoe.substr(0, x).c_str());							// Get and save x-coordinate of AoE (string before delimeter)
			aoe = aoe.substr(x+1);										// Remove x-coordinate from string
			x = aoe.find("|");											// Find index of next delimeter
			b = atoi(x > 0 ? aoe.substr(0, x).c_str() : aoe.c_str());	// Get and save y-coordinate of AoE (string before delimeter)
			aoe = aoe.substr(x+1);										// Remove y-coordinate from delimeter
			areaOfEffect.push_back(Position(a,b));						// Add new Position(x,y) to AoE list - Move to next entry in AoE
		}
	}
	else
		areaOfEffect.push_back(Position(0,0));


	// 7) Script
	element = element->NextSiblingElement();	// move to next element
	value = element->GetText();					// get script
	script.clear();
	if(value != NULL)
		script = value;
}


Ability::~Ability(void)
{
}

void Ability::Activate(lua_State* L, Position target, Position source)
{
	// Create table to tell script where skill was cast from
	lua_createtable(L, 2, 0);
	lua_pushinteger(L, source.x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, source.y);
	lua_setfield(L, -2, "y");
	lua_setglobal(L, "Source");

	// Create tables of tiles where skill  has an effect
	if(areaOfEffect.size() > 0)
	{
		stringstream ss(stringstream::in | stringstream::out);
		lua_createtable(L, areaOfEffect.size(), 0);		// Create AreaOfEffect
		for(int i = 0; i < areaOfEffect.size(); i++)
		{
			ss << i;
			lua_createtable(L, 2, 0);					// Create next entry in AoE
			lua_pushinteger(L, target.x + areaOfEffect[i].x);
			lua_setfield(L, -2, "x");					// Set and name x-coordinate in entry
			lua_pushinteger(L, target.y + areaOfEffect[i].y);
			lua_setfield(L, -2, "y");					// Set and name y-coordinate in entry 
			lua_setfield(L, -2, ss.str().c_str());		// Name entry (string of entries index in AoE)
		}

		lua_setglobal(L, "AreaOfEffect");				// Name AreaOfEffect
	}
	else	// Self Cast or Single Target
	{
		lua_createtable(L, 1, 0);			// Create AreaOfEffect
		lua_createtable(L, 2, 0);			// Create first entry in AoE
		lua_pushinteger(L, target.x);		
		lua_setfield(L, -2, "x");			// Set and name x-coordinate in first entry
		lua_pushinteger(L, target.y);
		lua_setfield(L, -2, "y");			// Set and name y-coordinate in first entry
		lua_setfield(L, -2, "0");			// Name first entry
		lua_setglobal(L, "AreaOfEffect");	// Name AreaOfEffect
	}

	// Run script
	luaL_dofile(L, script.c_str());
}

char* Ability::GetName() { return name; }
Ability::Type Ability::GetType() { return type; }
int Ability::GetCost() { return apCost; }
Ability::CastType Ability::GetCastType() { return castType; }
int Ability::GetRange() { return range; }
void Ability::SetRange(int r) { range = r; }
vector<Position> Ability::GetAOE() { return areaOfEffect; }