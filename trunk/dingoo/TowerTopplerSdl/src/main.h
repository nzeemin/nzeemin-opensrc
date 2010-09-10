// main.h

#ifndef _MAIN_H_
#define _MAIN_H_


/////////////////////////////////////////////////////////////////////////////
// General

#define VERSION_MAJOR           0
#define VERSION_MINOR           1

#define SCREEN_WIDTH            320
#define SCREEN_HEIGHT           240

const float PI                  = 3.14159265358979323f;
const float ANGLE_360           = 360.0f;
const float ANGLE_180           = 180.0f;
const float ANGLE_BLOCK         = 22.5f;  // 16 blocks
const float ANGLE_HALFBLOCK     = 11.25f;

#define TOWER_RADIUS            75
#define TOWER_STEPS_RADIUS      (TOWER_RADIUS + 30)
#define TOWER_LIFTC_RADIUS      (TOWER_RADIUS + 15)     // Distance between tower center and lift center
#define TOWER_LIFT_RADIUS       14                      // Radius of lift platform
#define TOWER_STICKC_RADIUS     (TOWER_RADIUS + 15)
#define TOWER_STICK_RADIUS      6
#define TOWER_SNOWBALLC_RADIUS  (TOWER_RADIUS + 15)
#define POSY_BRICK_HEIGHT       8
#define POSX_VIEWPORT_MARGIN    6
#define POSY_VIEWPORT_TOP       44
#define POSY_VIEWPORT_BOTTOM    SCREEN_HEIGHT - 16
#define POSY_VIEWPORT_HEIGHT    180
#define POSX_VIEWPORT_WIDTH     (SCREEN_WIDTH - 2 * POSX_VIEWPORT_MARGIN)
#define POSX_VIEWPORT_CENTER    (SCREEN_WIDTH / 2)
#define POSX_TOWER_CENTER       POSX_VIEWPORT_CENTER
#define POSY_POGO_BASE          (POSY_VIEWPORT_TOP + 110)
#define POSX_POGO_HALFWIDTH     12
#define POSY_POGO_HEIGHT        20

#define STARS_MULTIPLIER        3
#define STARS_BACK_HEIGHT       (POSY_VIEWPORT_HEIGHT * 2)
#define STARS_BACK_WIDTH        (360 * STARS_MULTIPLIER + SCREEN_WIDTH)
#define STARCOUNT               50

enum PogoStateEnum
{
    // Masks and masked values
    POGO_MASK_DIR   = 0x1000,  // Direction mask
    POGO_L          = 0x0000,
    POGO_R          = 0x1000,
    POGO_MASK_STATE = 0x00ff,  // State mask
    // States
    POGO_STAY       = 0x0001,
    POGO_WALK       = 0x0002,
    POGO_DOOR       = 0x0004,  // Door and 180 degree rotation
    POGO_JUMP       = 0x0008,
    POGO_LIFT       = 0x0010,  // Lifting on elevator
    POGO_FALL       = 0x0020,
    POGO_DROWN      = 0x0040,
    // Combined values
    POGO_L_STAY     = POGO_STAY | POGO_L,
    POGO_L_WALK     = POGO_WALK | POGO_L,
    POGO_L_DOOR     = POGO_DOOR | POGO_L,
    POGO_L_JUMP     = POGO_JUMP | POGO_L,
    POGO_L_LIFT     = POGO_LIFT | POGO_L,
    POGO_L_FALL     = POGO_FALL | POGO_L,
    POGO_L_DROWN    = POGO_DROWN | POGO_L,
    POGO_R_STAY     = POGO_STAY | POGO_R,
    POGO_R_WALK     = POGO_WALK | POGO_R,
    POGO_R_DOOR     = POGO_DOOR | POGO_R,
    POGO_R_JUMP     = POGO_JUMP | POGO_R,
    POGO_R_LIFT     = POGO_LIFT | POGO_R,
    POGO_R_FALL     = POGO_FALL | POGO_R,
    POGO_R_DROWN    = POGO_DROWN | POGO_R,
};


/////////////////////////////////////////////////////////////////////////////
// Levels

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
int Level_IsEmpty(Uint8 data);
int Level_IsStation(Uint8 data);
inline int Level_IsDoor(Uint8 data)
{
    return (data == TB_DOOR || data == TB_DOOR_TARGET || data == TB_STICK_DOOR);
}
inline int Level_IsSliding(Uint8 data)
{
    return ((data == TB_STEP_LSLIDER) ? 1 : (data == TB_STEP_RSLIDER) ? -1 : 0);
}
inline int Level_IsUpStation(Uint8 data)
{
    return (data == TB_ELEV_BOTTOM || data == TB_ELEV_MIDDLE);
}
inline int Level_IsDownStation(Uint8 data)
{
    return (data == TB_ELEV_TOP || data == TB_ELEV_MIDDLE);
}

void Level_RemoveVanishStep(int row, int col);


/////////////////////////////////////////////////////////////////////////////
// Robots

// Values for kinds of robots
typedef enum RobotKinds {
  OBJ_KIND_NOTHING,
  OBJ_KIND_JUMPBALL,
  OBJ_KIND_FREEZEBALL,
  OBJ_KIND_FREEZEBALL_FALLING,
  OBJ_KIND_DISAPPEAR,
  OBJ_KIND_FREEZEBALL_FROZEN,
  OBJ_KIND_APPEAR,
  OBJ_KIND_CROSS,
  OBJ_KIND_ROBOT_VERT,
  OBJ_KIND_ROBOT_HORIZ
};


/////////////////////////////////////////////////////////////////////////////
#endif
