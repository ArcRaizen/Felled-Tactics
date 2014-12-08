#include "stdafx.h"
#include "Ability.h"

Ability::Ability(const char* name) : rank(1)
{
	// Pair is a struct that holds a string and a Value
	// Value is a class that can be: string/int/double/bool/null/Array/Object
	// Object is a vector or Pairs - mObject is a map of of Pairs
	// Array is a vector of Values

	// Temporary variables that will be needed
	int a, b, i, j, x, y, num;
	float f;
	string temp, aoe;

	// Open and read the json document of all the abilities
	ifstream is("../FelledTactics/Abilities.json");
	json_spirit::mValue value;
	json_spirit::read(is, value);

	// Get the ability we're looking for (as a special std::map)
	json_spirit::mObject abilityMap = value.get_obj();
	json_spirit::mObject ability = abilityMap.find(name)->second.get_obj();
	strcpy(this->name, name);

	// ~~Get the ability information out of the map~~
	// 1) Ability Type
	temp = ability["type"].get_str();
	if(!temp.compare("Action"))
		type = Action;
	else if(!temp.compare("Battle"))
		type = Battle;
	else	// Passive
		type = Passive;

	// 2) Effect Type
	temp = ability["effect_type"].get_str();
	if(!temp.compare("Physical"))
		effect = Physical;
	else if(!temp.compare("Magical"))
		effect = Magical;
	else if(!temp.compare("Heal"))
		effect = Heal;
	else if(!temp.compare("Status"))
		effect = Status;
	else	// None
		effect = None;

	// 3) Cast Type
	temp = ability["cast_type"].get_str();
	if(!temp.compare("SelfCast"))
		castType = SelfCast;
	else if(!temp.compare("Ally"))
		castType = Ally;
	else if(!temp.compare("Enemy"))
		castType = Enemy;
	else	// None
		castType = Free;

	// 4) Max Rank
	maxRank = ability["max_rank"].get_int();
	apCosts = new int[maxRank];						// ~~
	ranges = new int[maxRank];						// Create arrays for this abilities parameters now that we know its maximum rank
	areasOfEffect = new vector<Position>[maxRank];	// ~~

	// 5) AP Cost
	temp = ability["ap_cost"].get_str();
	for(i = 0; i < maxRank; i++)
	{
		x = temp.find("/");
		apCosts[i] = atoi(temp.substr(0, x).c_str());
		temp = temp.substr(x+1);
	}

	// 6) Range
	temp = ability["range"].get_str();
	for(i = 0; i < maxRank; i++)
	{
		x = temp.find("/");
		ranges[i] = atoi(temp.substr(0, x).c_str());
		temp = temp.substr(x+1);
	}

	// 7) Area of Effect
	temp = ability["aoe"].get_str();
	for(i = 0; i < maxRank; i++)
	{
		y = temp.find("/");
		aoe = temp.substr(0, y);				// find next set of AoEs (a set for each rank of the ability)
		num = atoi(aoe.substr(0, aoe.find("|")).c_str());	// find first number (number of Positions in the AoE) convert to int and save

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
	temp = ability["cast_timers"].get_str();
	for(i = 0; i < 4; i++)
	{
		 x = temp.find("|");					// Find index of first delimeter
		 f = atof(temp.substr(0, x).c_str());	// Get and save cast timer (string before delimeter)
		 temp = temp.substr(x+1);				// Remove timer from string
		 castTimers[i] = f;						// Save timer
	}

	// 9) Script
	script = ability["script"].get_str();
}

Ability::Ability(const char* name, json_spirit::mObject abilityMap) : rank(1)
{
	// Temporary variables that will be needed
	int a, b, i, j, x, y, num;
	float f;
	string temp, aoe;

	// Save name
	strcpy(this->name, name);

	// ~~Get the ability information out of the ability map~~
	// 1) Ability Type
	temp = abilityMap["type"].get_str();
	if(!temp.compare("Action"))
		type = Action;
	else if(!temp.compare("Battle"))
		type = Battle;
	else	// Passive
		type = Passive;

	// 2) Effect Type
	temp = abilityMap["effect_type"].get_str();
	if(!temp.compare("Physical"))
		effect = Physical;
	else if(!temp.compare("Magical"))
		effect = Magical;
	else if(!temp.compare("Heal"))
		effect = Heal;
	else if(!temp.compare("Status"))
		effect = Status;
	else	// None
		effect = None;

	// 3) Cast Type
	temp = abilityMap["cast_type"].get_str();
	if(!temp.compare("SelfCast"))
		castType = SelfCast;
	else if(!temp.compare("Ally"))
		castType = Ally;
	else if(!temp.compare("Enemy"))
		castType = Enemy;
	else	// None
		castType = Free;

	// 4) Max Rank
	maxRank = abilityMap["max_rank"].get_int();
	apCosts = new int[maxRank];						// ~~
	ranges = new int[maxRank];						// Create arrays for this abilities parameters now that we know its maximum rank
	areasOfEffect = new vector<Position>[maxRank];	// ~~

	// 5) AP Cost
	temp = abilityMap["ap_cost"].get_str();
	for(i = 0; i < maxRank; i++)
	{
		x = temp.find("/");
		apCosts[i] = atoi(temp.substr(0, x).c_str());
		temp = temp.substr(x+1);
	}

	// 6) Range
	temp = abilityMap["range"].get_str();
	for(i = 0; i < maxRank; i++)
	{
		x = temp.find("/");
		ranges[i] = atoi(temp.substr(0, x).c_str());
		temp = temp.substr(x+1);
	}

	// 7) Area of Effect
	temp = abilityMap["aoe"].get_str();
	for(i = 0; i < maxRank; i++)
	{
		y = temp.find("/");
		aoe = temp.substr(0, y);				// find next set of AoEs (a set for each rank of the ability)
		num = atoi(aoe.substr(0, aoe.find("|")).c_str());	// find first number (number of Positions in the AoE) convert to int and save

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
	temp = abilityMap["cast_timers"].get_str();
	for(i = 0; i < 4; i++)
	{
		 x = temp.find("|");					// Find index of first delimeter
		 f = atof(temp.substr(0, x).c_str());	// Get and save cast timer (string before delimeter)
		 temp = temp.substr(x+1);				// Remove timer from string
		 castTimers[i] = f;						// Save timer
	}

	// 9) Script
	script = abilityMap["script"].get_str();
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
	if(test)
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