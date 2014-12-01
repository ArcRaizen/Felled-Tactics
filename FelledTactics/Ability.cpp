#include "stdafx.h"
#include "Ability.h"


Ability::Ability(const char* name) : rank(1)
{
	// Temporary variables that will be needed
	int a, b, i, j, x, y, num;
	float f;
	string temp, aoe;
	stringstream stream;

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

	// 4) Max Rank
	element = element->NextSiblingElement();	// move to next element
	stream << element->GetText();				// get maximum rank
	stream >> maxRank;							// convert to int and save
	stream.clear();

	// Create arrays for this abilities parameters now that we know its maximum rank
	apCosts = new int[maxRank];
	ranges = new int[maxRank];
	areasOfEffect = new vector<Position>[maxRank];

	// 5) AP Cost
	element = element->NextSiblingElement();	// move to next element
	stream << element->GetText();				// get ap cost
	stream >> temp;
	stream.clear();
	for(i = 0; i < maxRank; i++)
	{
		x = temp.find("/");
		apCosts[i] = atoi(temp.substr(0, x).c_str());
		temp = temp.substr(x+1);
	}
	

	// 6) Range
	element = element->NextSiblingElement();	// move to next element
	stream << element->GetText();				// get range		
	stream >> temp;								
	stream.clear();
	for(i = 0; i < maxRank; i++)
	{
		x = temp.find("/");
		ranges[i] = atoi(temp.substr(0, x).c_str());
		temp = temp.substr(x+1);
	}

	// 7) AoE
	element = element->NextSiblingElement();	// move to next element
	stream << element->GetText();				// get aoe string
	stream >> temp;								// convert to string
	stream.clear();
	
	for(i = 0; i < maxRank; i++)
	{
		y = temp.find("/");
		aoe = temp.substr(0, y);				// find next set of AoEs (a set for each rank of the ability)
		stream << aoe.substr(0, aoe.find("|"));	// find first number (number of Positions in the AoE)
		stream >> num;								// conver to int and save
		stream.clear();

		if(num > 0)
		{
			aoe = aoe.substr(aoe.find("|") + 1).c_str();	// Get list of positions
			for(j = 0; j < num; j++)
			{
				x = aoe.find("|");											// Find index of first delimeter
				a = atoi(aoe.substr(0, x).c_str());							// Get and save x-coordinate of AoE (string before delimeter)
				aoe = aoe.substr(x+1);										// Remove x-coordinate from string
				x = aoe.find("|");											// Find index of next delimeter
				b = atoi(x > 0 ? aoe.substr(0, x).c_str() : aoe.c_str());	// Get and save y-coordinate of AoE (string before delimeter)
				aoe = aoe.substr(x+1);										// Remove y-coordinate from delimeter
				areasOfEffect[i].push_back(Position(a,b));					// Add new Position(x,y) to AoE list - Move to next entry in AoE
			}
		}
		else
			areasOfEffect[i].push_back(Position(0,0));

		temp = temp.substr(y+1);
	}

	// 8) Cast Timers
	string timers;
	element = element->NextSiblingElement();	// move to next element
	stream << element->GetText();				// get timer string
	stream >> timers;							// convert to string
	stream.clear();
	
	for(i = 0; i < 4; i++)
	{
		 x = timers.find("|");						// Find index of first delimeter
		 f = atof(timers.substr(0, x).c_str());		// Get and save cast timer (string before delimeter)
		 timers = timers.substr(x+1);				// Remove timer from string
		 castTimers[i] = f;							// Save timer
	}

	// 9) Script
	element = element->NextSiblingElement();	// move to next element
	value = element->GetText();					// get script
	script.clear();
	if(value != NULL)
		script = value;
}


Ability::~Ability(void)
{
	delete [] apCosts;
	delete [] ranges;
	delete [] areasOfEffect;
}

void Ability::Activate(lua_State* L, Position target, Position source)
{
	// Tell the script the current rank of the ability
	lua_pushinteger(L, rank);
	lua_setglobal(L, "Rank");

	// Create table to tell script where skill was cast from
	lua_createtable(L, 2, 0);
	lua_pushinteger(L, source.x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, source.y);
	lua_setfield(L, -2, "y");
	lua_setglobal(L, "Source");

	// Create tables of tiles where skill  has an effect
	if(areasOfEffect[rank-1].size() > 0)
	{
		stringstream ss(stringstream::in | stringstream::out);
		lua_createtable(L, areasOfEffect[rank-1].size(), 0);		// Create AreaOfEffect
		for(int i = 0; i < areasOfEffect[rank-1].size(); i++)
		{
			ss << i;
			lua_createtable(L, 2, 0);					// Create next entry in AoE
			lua_pushinteger(L, target.x + areasOfEffect[rank-1][i].x);
			lua_setfield(L, -2, "x");					// Set and name x-coordinate in entry
			lua_pushinteger(L, target.y + areasOfEffect[rank-1][i].y);
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
#ifdef DEBUG
	int test = luaL_dofile(L, script.c_str());
	if(test == 1)
	{
		std::string error = lua_tostring(L, -1);
		test++;
	}
#else
	luaL_dofile(L, script.c_str());
#endif
}

bool Ability::RankUp()
{
	if(rank < maxRank)
	{
		rank++;
		return true;
	}

	return false;
}

char* Ability::GetName() { return name; }
int Ability::GetRank() { return rank; }
Ability::Type Ability::GetType() { return type; }
int Ability::GetCost() { return apCosts[rank-1]; }
Ability::CastType Ability::GetCastType() { return castType; }
int Ability::GetRange() { return ranges[rank-1]; }
vector<Position> Ability::GetAOE() { return areasOfEffect[rank-1]; }
const float* Ability::GetTimers() const { return castTimers; }