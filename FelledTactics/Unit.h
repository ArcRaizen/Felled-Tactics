#pragma once
#ifndef UNIT_H
#define UNIT_H

#ifndef VISUAL_H
#include "VisualElement.h"
#endif
#ifndef LEVELENTITY_H
#include "LevelEntity.h"
#endif
#ifndef INVENTORY_H
#include "Inventory.h"
#endif
#ifndef POSITION_S
#include "Position.h"
#endif
#ifndef ABILITY_H
#include "Ability.h"
#endif
#ifndef AUGMENT_H
#include "Augment.h"
#endif

// Codes
#ifndef UNITSTATUSCODES_C
#include "UnitStatusCodes.h"
#endif
#ifndef UNITUPDATECODES_C
#include "UnitUpdateCodes.h"
#endif

#include <list>

class Unit : public VisualElement, public LevelEntity
{
public:
	Unit(WCHAR* filename, int layer, int width, int height, int posX, int posY, bool ally=true);
	Unit(int layer, int width, int height, int posX, int posY, const char* name, json_spirit::mObject unitMap, json_spirit::mObject abilityMap);
	virtual ~Unit(void);

	static int		unitCounter;

	virtual void	CalculateCombatDamage(int& physicalDamage, int& magicalDamage, int range);	// Damage done by unit to enemy before enemy defences are factored in
	int				TakeUnscaledDamage(int dmg);
	int				TakeDamage(int physDamage, int magDamage);
	int				Heal(int hp);
	virtual void	Revive(int health);						// Revived by an ally
	virtual float	Die();									// Defeated in combat - Felled
	void			GainExperience();
	virtual void	LevelUp();

	// Movement
	void			SetMovePath(list<Position> path, float moveTime=0);
	void			ForceMovement(Position p, float moveTime);
	void			ForceEndMovement();
	void			FinishTurn();
	void			NewTurn(lua_State* L);

	void			LearnAbility(const char* name, int forceRank=1);	// learn ability with given name (specify a rank of the ability with forceRank)
	void			LearnAbility(const char* name, json_spirit::mObject abilityMap, int forceRank = 1);
	bool			SelectedBattleAbility() const;
	bool			SelectedAbilityHasDynamicAoE() const;
	void			SetSelectedAbility(int index);
	char*			GetSelectedAbilityName() const;
	int				GetSelectedAbilityCost() const;
	int				GetSelectedAbilityRange() const;
	void			SetSelectedAbilityRange(int r);
	Ability::CastType GetSelectedAbilityCastType() const;
	vector<Position> GetSelectedAbilityAoE(Position p = Position(0,0)) const;
	vector<Position> GetSelectedAbilityDynamicAoERange() const;
	const float*    GetSelectedAbilityTimers() const;
	void			ActivateAbility(lua_State* L, Position target);
	void			RefundAP();
	void			ClearBattleScripts();
	bool			HasAbility(const char* name);
	char*			GetAbilityName(int index) const;
	int				GetNumActiveAbilities() const;
	int				GetNumAbilities() const;

	bool			InitializeHPAPBuffers();

	virtual int		Update(float dt);		// returns used to pass info to caller (level)
	virtual bool	Draw();
	bool			UpdateHPAPBuffers();

	// Statuses
	void			ApplyStatus(int s);
	void			RemoveStatus(int s);
	bool			CheckStatus(int s);
	bool			IsAlly();
	bool			IsEnemy();

	json_spirit::Object Serialize();

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
	__declspec(property(get=GetMovementFinished)) bool MovementFinished;													bool GetMovementFinished();
	__declspec(property(put=SetFinished, get=GetFinished)) bool FinishedTurn;			void SetFinished(bool f);			bool GetFinished();
	__declspec(property(put=SetCombatCalcScript, get=GetCombatCalcScript)) std::string CombatCalcScript;
																						void SetCombatCalcScript(std::string s); std::string GetCombatCalcScript();
	__declspec(property(put=SetCombatExecuteScript, get=GetCombatExecuteScript)) std::string CombatExecuteScript;
																						void SetCombatExecuteScript(std::string s); std::string GetCombatExecuteScript();
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
	static float	BASE_MOVE_TIME;

	// Abilities
	int					numAbilities;
	int					numActionAbilities;
	int					numBattleAbilities;
	int					selectedAbility;
	vector<Ability*>	passiveAbilityList;
	vector<Ability*>	activeAbilityList;
	std::string			combatCalculationAbilityScript;
	std::string			combatExecutionAbilityScript;

	// Items
	Inventory					inventory;

	// DirectX Stuff / Health + Ability Power Bar Stuff
	ID3D10Buffer				*hpVertexBuffer, *apVertexBuffer;	// Vertex buffers for HP/AP Bars
	ID3D10ShaderResourceView	*hpBarTexture, *apBarTexture;		// Textures for HP/AP Bars
	bool						updateHPAPBuffers;					// Do the buffers need to be updated for drawing?
	D3DXMATRIX					hpapWorld;
	float						hpapHeight;
	bool						drawBars;

	static D3DXVECTOR4 highlightFinishedTurn;
};
#endif
