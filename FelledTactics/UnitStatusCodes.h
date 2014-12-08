#ifndef UNITSTATUSCODES_C
#define UNITSTATUSCODES_C
/*
	Unit Status Codes
	Represents the current statuses a unit has
*/

#define UNIT_STATUS_DYING			1 << 0
#define UNIT_STATUS_FELLED			1 << 1
#define UNIT_STATUS_ALLY			1 << 2
#define UNIT_STATUS_ENEMY			1 << 3
#define UNIT_STATUS_MOVING			1 << 4
#define UNIT_STATUS_FORCED_MOVING	1 << 5
#define UNIT_STATUS_BOSS			1 << 6
#endif