#include "StdAfx.h"
#include "Skill.h"


Skill::Skill(const char* name)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("../FelledTactics/Skills.xml");
	tinyxml2::XMLElement* skillList = doc.FirstChildElement();
	tinyxml2::XMLElement* skill = skillList->FirstChildElement();
	const char* value = skill->Attribute("Name");

	// Loop through all skills until correct skill is located
	while(strcmp(value, name) && skill != skillList->LastChildElement())
	{
		skill = skill->NextSiblingElement();
		value = skill->Attribute("Name");
	}

	// Set Skill from XML file
	// 1) Skill Type
	tinyxml2::XMLElement* element = skill->FirstChildElement();
	value = element->GetText();
	if(!strcmp(value, "Action"))
		type = Action;
	else if(!strcmp(value, "Battle"))
		type = Battle;
	else	// Passive
		type = Passive;

	// 2) Affect Type
	element = element->NextSiblingElement();
	value = element->GetText();
	if(!strcmp(value, "Physical"))
		affect = Physical;
	else if(!strcmp(value, "Magical"))
		affect = Magical;
	else if(!strcmp(value, "Heal"))
		affect = Heal;
	else if(!strcmp(value, "Status"))
		affect = Status;
	else	// None
		affect = None;


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
	script = NULL;
	if(value != NULL)
		script = value;
}


Skill::~Skill(void)
{
}

int Skill::GetCost() { return apCost; }
Skill::CastType Skill::GetType() { return castType; }
int Skill::GetRange() { return range; }