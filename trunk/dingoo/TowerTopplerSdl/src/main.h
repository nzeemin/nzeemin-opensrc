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
const float ANGLE_270           = 270.0f;
const float ANGLE_90            = 90.0f;
const float ANGLE_BLOCK         = 22.5f;    // 16 blocks
const float ANGLE_HALFBLOCK     = 11.25f;
const float ANGLE_ROTATION      = 2.5f;     // Rotation quant: tower moves for this quant every frame

#define TOWER_ANGLECOUNT        144         // Total rotation quants for one round: 360.0 / 2.5 = 144

#define TOWER_RADIUS            75
#define TOWER_STEPS_RADIUS      (TOWER_RADIUS + 30)
#define TOWER_ELEVC_RADIUS      (TOWER_RADIUS + 15)     // Distance between tower center and elevator center
#define TOWER_ELEV_RADIUS       14                      // Radius of elevator platform
#define TOWER_STICKC_RADIUS     (TOWER_RADIUS + 15)     // From tower center to stick center
#define TOWER_STICK_RADIUS      6
#define TOWER_SNOWBALLC_RADIUS  (TOWER_RADIUS + 15)     // From tower center to snowball center
#define TOWER_ROBOTC_RADIUS     (TOWER_RADIUS + 15)     // From tower center to robot center
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
#define STARS_BACK_WIDTH        (TOWER_ANGLECOUNT * STARS_MULTIPLIER)
#define STARCOUNT               50

enum PogoStateEnum
{
    // States
    POGO_STAY       = 0x0001,
    POGO_WALK       = 0x0002,
    POGO_DOOR       = 0x0004,  // Door and 180 degree rotation
    POGO_JUMP       = 0x0008,
    POGO_ELEV       = 0x0010,  // Moving up or down on elevator
    POGO_FALL       = 0x0020,
    POGO_DROWN      = 0x0040,
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

// Get data of the selected tower; row = 0..GetTowerHeight()-1, col = 0..TOWERWID-1
Uint8 Level_GetTowerBlock(int row, int col);

// Data qualification functions
int Level_IsPlatform(Uint8 data);
int Level_IsEmpty(Uint8 data);
int Level_IsStation(Uint8 data);
int Level_IsRobot(Uint8 data);
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
inline int Level_IsBottomStation(Uint8 data)
{
    return (data == TB_ELEV_BOTTOM);
}
inline int Level_IsStick(Uint8 data) {
    return ((data == TB_STICK) || (data == TB_STICK_TOP) || (data == TB_STICK_MIDDLE) || (data == TB_STICK_BOTTOM) ||
            (data == TB_STICK_DOOR) || (data == TB_STICK_DOOR_TARGET));
}

void Level_Platform2Stick(int row, int col);
void Level_Stick2Platform(int row, int col);
void Level_Stick2Empty(int row, int col);
void Level_Empty2Stick(int row, int col);
void Level_Platform2Empty(int row, int col);

void Level_RemoveVanishStep(int row, int col);
void Level_ClearBlock(int row, int col);
Uint8 Level_PutPlatform(int row, int col);
void Level_Restore(int row, int col, Uint8 data);


/////////////////////////////////////////////////////////////////////////////
// Robots

#define MAX_OBJECTS 4

#define TIMEOUT_ROBOT_CROSS         125

// Values for kinds of robots
typedef enum RobotKindEnum
{
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

void Robot_Initialize();
void Robot_New(int toplevel);
void Robot_Update();

RobotKindEnum Robot_GetKind(int rob);
float Robot_GetAngle(int rob);
int Robot_GetLevel(int rob);
int Robot_GetTime(int rob);


/////////////////////////////////////////////////////////////////////////////
// Elevators

#define MAX_ELE 10

#define TIMEOUT_ELEVATOR_RETURN     125

void Elevator_Initialize();
void Elevator_Select(int row, int col);
void Elevator_Activate(int direction);
void Elevator_Move();
int Elevator_IsAtStop();
void Elevator_Deactivate();
void Elevator_Update();


/////////////////////////////////////////////////////////////////////////////
#endif
