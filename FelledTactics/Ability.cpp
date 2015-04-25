#include "stdafx.h"
#include "Ability.h"

Ability::Ability(const char* name, int r) : rank(r)
{
	// Pair is a struct that holds a string and a Value
	// Value is a class that can be: string/int/double/bool/null/Array/Object
	// Object is a vector or Pairs - mObject is a map of of Pairs
	// Array is a vector of Values

	// Temporary variables that will be needed
	int i, x;
	float f;
	string temp, aoe;
	string numbers[] = {"1", "2", "3", "4", "5", "6"};

	// Open and read the json document of all the abilities
	ifstream is(ABILITY_JSON);
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
		type = Type::Action;
	else if(!temp.compare("Battle"))
		type = Type::Battle;
	else	// Passive
		type = Type::Passive;

	// 2) Effect Type
	temp = ability["effect_type"].get_str();
	if(!temp.compare("Physical"))
		effect = EffectType::Physical;
	else if(!temp.compare("Magical"))
		effect = EffectType::Magical;
	else if(!temp.compare("Heal"))
		effect = EffectType::Heal;
	else if(!temp.compare("Status"))
		effect = EffectType::Status;
	else	// None
		effect = EffectType::None;

	// 3) Cast Type
	temp = ability["cast_type"].get_str();
	if(!temp.compare("SelfCast"))
		castType = CastType::SelfCast;
	else if(!temp.compare("Ally"))
		castType = CastType::Ally;
	else if(!temp.compare("Enemy"))
		castType = CastType::Enemy;
	else	// None
		castType = CastType::Free;

	// 4) Max Rank
	maxRank = ability["max_rank"].get_int();

	// 5) AP Cost
	SetAPCost(ability);
	
	// 6) Range
	SetRange(ability);

	// 7) Area of Effect
	SetAoE(ability);

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

Ability::Ability(const char* name, json_spirit::mObject abilityMap, int r) : rank(r)
{
	// Temporary variables that will be needed
	int i, x;
	float f;
	string temp;

	// Save name
	strcpy(this->name, name);

	// ~~Get the ability information out of the ability map~~
	// 1) Ability Type
	temp = abilityMap["type"].get_str();
	if(temp == "Action")
		type = Type::Action;
	else if(temp == "Battle")
		type = Type::Battle;
	else	// Passive
		type = Type::Passive;

	// 2) Effect Type
	temp = abilityMap["effect_type"].get_str();
	if(temp == "Physical")
		effect = EffectType::Physical;
	else if(temp == "Magical")
		effect = EffectType::Magical;
	else if(temp == "Heal")
		effect = EffectType::Heal;
	else if(temp == "Status")
		effect = EffectType::Status;
	else	// None
		effect = EffectType::None;

	// 3) Cast Type
	temp = abilityMap["cast_type"].get_str();
	if(temp == "SelfCast")
		castType = CastType::SelfCast;
	else if(temp == "Ally")
		castType = CastType::Ally;
	else if(temp == "Enemy")
		castType = CastType::Enemy;
	else	// None
		castType = CastType::Free;

	// 4) Max Rank
	maxRank = abilityMap["max_rank"].get_int();

	// 5) AP Cost
	SetAPCost(abilityMap);

	// 6) Range
	SetRange(abilityMap);
	
	// 7) Area of Effect
	SetAoE(abilityMap);

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
	if(areaOfEffect.size() == 1)
	{
		stringstream ss(stringstream::in | stringstream::out);
		lua_createtable(L, areaOfEffect[Position(0,0)].size(), 0);		// Create AreaOfEffect
		for(int i = 0; i < areaOfEffect[Position(0,0)].size(); i++)
		{
			ss << i;
			lua_createtable(L, 2, 0);					// Create next entry in AoE
			lua_pushinteger(L, target.x + areaOfEffect[Position(0,0)][i].x);
			lua_setfield(L, -2, "x");					// Set and name x-coordinate in entry
			lua_pushinteger(L, target.y + areaOfEffect[Position(0,0)][i].y);
			lua_setfield(L, -2, "y");					// Set and name y-coordinate in entry 
			lua_setfield(L, -2, ss.str().c_str());		// Name entry (string of entries index in AoE)
		}

		lua_setglobal(L, "AreaOfEffect");				// Name AreaOfEffect
	}
	else if(areaOfEffect.size() > 1)
	{
		Position index = target - source;
		stringstream ss(stringstream::in | stringstream::out);
		lua_createtable(L, areaOfEffect[index].size(), 0);
		for(int i = 0; i < areaOfEffect[index].size(); i++)
		{
			ss << i;
			lua_createtable(L, 2, 0);					// Create next entry in AoE
			lua_pushinteger(L, target.x + areaOfEffect[index][i].x);
			lua_setfield(L, -2, "x");					// Set name and x-coordinate in entry
			lua_pushinteger(L, target.y + areaOfEffect[index][i].y);
			lua_setfield(L, -2, "y");					// Set name and y-coordinate in entry
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
		ifstream is(ABILITY_JSON);
		json_spirit::mValue value;
		json_spirit::read(is, value);
		json_spirit::mObject abilityMap = value.get_obj().find(name)->second.get_obj();
		SetAPCost(abilityMap);
		SetRange(abilityMap);
		SetAoE(abilityMap);
		return true;
	}

	return false;
}

void Ability::SetAPCost(json_spirit::mObject abilityMap)
{
	const string numbers[] = {"1", "2", "3", "4", "5"};

	if(maxRank > 1)
		apCost = abilityMap["ap_cost"].get_obj().find(numbers[rank-1])->second.get_int();
	else
		apCost = abilityMap["ap_cost"].get_int();
}

void Ability::SetRange(json_spirit::mObject abilityMap)
{
	const string numbers[] = {"1", "2", "3", "4", "5"};

	if(maxRank > 1)
		range = abilityMap["range"].get_obj().find(numbers[rank-1])->second.get_int();
	else
		range = abilityMap["range"].get_int();
}

void Ability::SetAoE(json_spirit::mObject abilityMap)
{
	const string numbers[] = {"1", "2", "3", "4", "5"};
	string temp, aoe;
	int i, j, a, b, x, y, num, numDynamic=1;

	areaOfEffect.clear();

	bool dynamic = abilityMap["aoe_type"].get_str() == "Dynamic";
	if(maxRank > 1)
		temp = dynamic ? abilityMap["dynamic_aoe"].get_obj().find(numbers[rank-1])->second.get_str() : abilityMap["static_aoe"].get_obj().find(numbers[rank-1])->second.get_str();
	else
		temp = dynamic ? abilityMap["dynamic_aoe"].get_str() : abilityMap["static_aoe"].get_str();

	if(dynamic)
	{
		y = temp.find("|");
		numDynamic = atoi(temp.substr(0, y).c_str());
		temp = temp.substr(y+1);
	}

	y = temp.find("/");
	aoe = temp.substr(0, y);

	Position index(0,0);
	for(i = 0; i < numDynamic; i++)
	{
		num = atoi(aoe.substr(0, aoe.find("|")).c_str());	// find first number (number of Positions in the AoE) convert to int and save
		aoe = aoe.substr(aoe.find("|") + 1).c_str();		// Get list of positions
		
		if(num > 0)
		{
			for(j = 0; j < num; j++)	
			{
				x = aoe.find("|");											// Find index of first delimeter
				a = atoi(aoe.substr(0, x).c_str());							// Get and save x-coordinate of AoE (string before delimeter)
				aoe = aoe.substr(x+1);										// Remove x-coordinate from string
				x = aoe.find("|");											// Find index of next delimeter
				b = atoi(x > 0 ? aoe.substr(0, x).c_str() : aoe.c_str());	// Get and save y-coordinate of AoE (string before delimeter)
				aoe = aoe.substr(x+1);										// Remove y-coordinate from delimeter

				if(!j)	// if new dynamic AoE (1st iteration of j-loop for each i-loop iteration)
				{
					if(dynamic)	// Create a new index in the map
						index = Position(a, b);

					areaOfEffect.insert(std::make_pair(index, vector<Position>()));

					// Static AoE 1st entry is part of AoE, where dynamic is just the index in AoE map. Add now
					if(!dynamic)
						areaOfEffect[index].push_back(Position(a,b));
				}
				else	// Add new entry to the AoE
					areaOfEffect[index].push_back(Position(a,b));
			}
		}
		else	// No AoE, add blank entry
		{
			areaOfEffect.insert(make_pair(index, vector<Position>()));
			areaOfEffect[index].push_back(Position(0,0));
		}

		temp = temp.substr(y+1);
		y = temp.find("/");
		aoe = temp.substr(0, y);
	}
}

char* Ability::GetName() { return name; }
int Ability::GetRank() { return rank; }
Ability::Type Ability::GetType() { return type; }
int Ability::GetCost() { return apCost; }
Ability::CastType Ability::GetCastType() { return castType; }
int Ability::GetRange() { return range; }
vector<Position> Ability::GetAoE(Position p/*=Position(0,0)*/) 
{ 
	if(areaOfEffect.size() == 1) 
	{ return areaOfEffect[Position(0,0)]; } 
	
	auto i = areaOfEffect.find(p);
	if(i != areaOfEffect.end())
		return areaOfEffect[p];

	return vector<Position>();
}
const float* Ability::GetTimers() const { return castTimers; }
bool Ability::HasDynamicAoE() const { return areaOfEffect.size() > 1; }
vector<Position> Ability::GetDynamicAoERange() 
{ 
	vector<Position> v;
	for(auto i = areaOfEffect.begin(); i != areaOfEffect.end(); i++)
		v.push_back(i->first);

	return v;
}