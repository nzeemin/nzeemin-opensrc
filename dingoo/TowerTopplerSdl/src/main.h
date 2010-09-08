// main.h

#ifndef _MAIN_H_
#define _MAIN_H_

/////////////////////////////////////////////////////////////////////////////


#define TOWERWID 16


typedef enum TowerBlockEnum {
    TB_EMPTY,
    TB_STATION_TOP,
    TB_STATION_MIDDLE,
    TB_STATION_BOTTOM,
    TB_ROBOT1,
    TB_ROBOT2,
    TB_ROBOT3,
    TB_ROBOT4,
    TB_ROBOT5,
    TB_ROBOT6,
    TB_ROBOT7,
    TB_STICK,
    TB_STEP,
    TB_STEP_VANISHER,
    TB_STEP_LSLIDER,
    TB_STEP_RSLIDER,
    TB_BOX,
    TB_DOOR,
    TB_DOOR_TARGET,
    TB_STICK_TOP,
    TB_STICK_MIDDLE,
    TB_STICK_BOTTOM,
    TB_ELEV_TOP,
    TB_ELEV_MIDDLE,
    TB_ELEV_BOTTOM,
    TB_STICK_DOOR,
    TB_STICK_DOOR_TARGET,
    TB_ELEV_DOOR,
    TB_ELEV_DOOR_TARGET,
    NUM_TBLOCKS
};


// Get total count of towers
int Level_GetTowerCount();

// Select a tower -- make it current
void Level_SelectTower(int tower);

// Get height of the selected tower
int Level_GetTowerSize();

// Color of the selected tower
Uint32 Level_GetTowerColor();

// Get data of the selected tower
Uint8 Level_GetTowerData(int row, int col);

// Data qualification functions
int Level_IsPlatform(Uint8 data);
inline int Level_IsDoor(Uint8 data)
{
    return (data == TB_DOOR || data == TB_DOOR_TARGET || data == TB_STICK_DOOR);
}

void Level_RemoveVanishStep(int row, int col);


/////////////////////////////////////////////////////////////////////////////
#endif
