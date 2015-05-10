#ifndef UNITSTATUSCODES_C
#define UNITSTATUSCODES_C
/*
	Unit Status Codes
	Represents the current statuses a unit has
*/

#define UNIT_STATUS_KILLED_IN_COMBAT	1 << 0	// Unit killed during combat, has not yet switched to "dying"
#define UNIT_STATUS_DYING				1 << 1	// Going through death animation(s)
#define UNIT_STATUS_FELLED				1 << 2	// Ally unit finished with their death animation(s). Waiting to be revived
#define UNIT_STATUS_ALLY				1 << 3	// Unit controlled by player
#define UNIT_STATUS_ENEMY				1 << 4	// Unit controlled by AI
#define UNIT_STATUS_MOVING				1 << 5	// Unit is currently moving on the map
#define UNIT_STATUS_FORCED_MOVING		1 << 6	// Unit is being pushed in some manner on the map
#define UNIT_STATUS_BOSS				1 << 7	// Unit is an enemy boss the player must defeat
#endif