#pragma once
#ifndef UNIT_H
#define UNIT_H

#ifndef VISUAL_H
#include "VisualElement.h"
#endif
#ifndef INVENTORY_H
#include "Inventory.h"
#endif
#ifndef POSITION_S
#include "Position.h"
#endif
#ifndef SKILL_H
#include "Ability.h"
#endif

#include <list>

#pragma region Status Codes
#define FELLED		1 << 0
#define ALLY		1 << 1
#define ENEMY		1 << 2
#pragma endregion

class Unit : public VisualElement
{
public:
	Unit(WCHAR* filename, int layer, int width, int height, int posX, int posY, bool ally=true);
	virtual ~Unit(void);

	enum Phylum		{Martial, Mystical, Support};
	static int		unitCounter;

	virtual void	CalculateCombatDamage(int& physicalDamage, int& magicalDamage, int range);	// Damage done by unit to enemy before enemy defences are factored in
	int				TakeDamage(int physDamage, int magDamage);
	virtual void	Revive(int health);						// Revived by an ally
	virtual float	Die();									// Defeated in combat - Felled
	void			GainExperience();
	virtual void	LevelUp();
	void			SetMovePath(list<Position> path);

	// Get ability
	Ability*		GetSelectedAbility();
	int				ActivateAbility(lua_State* L, Position target);

	bool			InitializeHPAPBuffers();

	virtual int		Update(float dt);		// returns used to pass info to caller (level)
	virtual bool	Draw();
	bool			UpdateHPAPBuffers();

	void			ApplyStatus(int s);
	void			RemoveStatus(int s);
	bool			CheckStatus(int s);
	bool			IsAlly();
	bool			IsEnemy();

	void			FinishTurn();
	void			NewTurn();

#pragma region Property Declaration
	__declspec(property(put=SetPosition, get=GetPosition)) Position UnitPosition;		void SetPosition(Position p);		Position GetPosition();
	__declspec(property(put=SetName, get=GetName)) char* Name;							void SetName(char* n);				char* GetName();
	__declspec(property(put=SetExperience, get=GetExperience)) int Experience;			void SetExperience(int xp);			int	GetExperience();
	__declspec(property(put=SetHealth, get=GetHealth)) int Health;						void SetHealth(int h);				int GetHealth();
	__declspec(property(put=SetAbilityPoints, get=GetAbilityPoints)) int AbilityPoints;	void SetAbilityPoints(int ap);		int GetAbilityPoints();
	__declspec(property(put=SetStrength, get=GetStrength)) int Strength;				void SetStrength(int s);			int GetStrength();
	__declspec(property(put=SetMagic, get=GetMagic)) int Magic;							void SetMagic(int m);				int GetMagic();
	__declspec(property(put=SetAgility, get=GetAgility)) int Agility;					void SetAgility(int a);				int GetAgility();
	__declspec(property(put=SetSkill, get=GetSkill)) int Skill;							void SetSkill(int s);				int GetSkill();
	__declspec(property(put=SetDefense, get=GetDefense)) int Defense;					void SetDefense(int d);				int GetDefense();
	__declspec(property(put=SetResistance, get=GetResistance)) int Resistance;			void SetResistance(int r);			int GetResistance();
	__declspec(property(put=SetMovement, get=GetMovement)) int Movement;				void SetMovement(int m);			int GetMovement();
	__declspec(property(put=SetAttackRange, get=GetAttackRange)) int AttackRange;		void SetAttackRange(int r);			int GetAttackRange();
	__declspec(property(get=GetPhylum)) Phylum UnitPhylum;																	Phylum GetPhylum();
	__declspec(property(get=GetMovementFinished)) bool MovementFinished;													bool GetMovementFinished();
	__declspec(property(put=SetFinished, get=GetFinished)) bool FinishedTurn;			void SetFinished(bool f);			bool GetFinished();
	__declspec(property(put=SetDrawBars)) bool DrawBars;								void SetDrawBars(bool db);

	int GetUnitID() const;
#pragma endregion

	bool			operator==(const Unit& other) const;
	bool			operator==(const Unit* &other) const;

private:
	void			InitProficiency();

protected:
	Position	position;

#pragma region Stats and Stuff
	// Base Stats
	char	name[15];
	int		experience;
	int		health;
	int		maximumHealth;
	int		abilityPoints;
	int		maximumAbilityPoints;
	int		strength;
	int		magic;
	int		agility;
	int		skill;
	int		defense;
	int		resistance;
	int		movement;
	int		attackRange;
	Phylum	phylum;
	int		unitID;

	// Growth Rates
	float	hpGrowth;
	float	apGrowth;
	float	strGrowth;
	float	magGrowth;
	float	aglGrowth;
	float	sklGrowth;
	float	defGrowth;
	float	resGrowth;

	// Proficiency Values
	float	bladedgeProficiency;
	float	axereaverProficiency;
	float	pheylanceProficiency;
	float	rangerProficiency;
	float	reaperProficiency;
	float	riderProficiency;
	float	bruiserProficiency;
	float	elementalistProficiency;
	float	kinectorProficieny;
	float	arcaneweaverProficiency;
	float	blooddancerProficiency;
	float	shamanProficiency;
	float	_proficiency;
	float	healerProficiency;
	float	inflicterProficiency;
	float	enchanterProficiency;
	float	enforcerProficiency;

	// Ability Point regeneration rate (varies by class)
	float	apRegenRate;

	// Was a unit felled without resuscitation last level
	bool	felled;

	// Status of any buffs or debuffs
	int		status;
#pragma endregion

	// Has the unit already completed their actions for this turn?
	bool	finishedTurn;
	
	// Path the unit is moving along during moving phase
	list<Position>	movementPath;
	float			secBetweenTiles;
	float			movementTimer;
	bool			movementFinished;

	// DirectX Stuff / Health + Ability Power Bar Stuff
	ID3D10Buffer				*hpVertexBuffer, *apVertexBuffer;	// Vertex buffers for HP/AP Bars
	ID3D10ShaderResourceView	*hpBarTexture, *apBarTexture;		// Textures for HP/AP Bars
	bool						updateHPAPBuffers;					// Do the buffers need to be updated for drawing?
	D3DXMATRIX					hpapWorld;
	float						hpapHeight;
	bool						drawBars;

	Inventory					inventory;
	vector<Ability*>			abilityList;

	static D3DXVECTOR4 highlightFinishedTurn;
};
#endif
