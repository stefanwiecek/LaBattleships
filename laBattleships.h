#ifndef laBattleships
#define laBattleships

#include "os.h"

// Additional colour definitions
#define C_BLUE 0x3C1E
#define C_LIGHT_GREY 0xD69A

// Colours
#define GridLineColor C_BLUE
#define BackgroundColor WHITE
#define MissColor C_LIGHT_GREY
#define DestroyedColor RED
#define CursorColour MAGENTA
#define ShipColor GREEN
#define ErrorColor RED

// Drawing info
#define LineThickness 2
#define NoRowColDef 10

#define HIT_MN 120
#define MISS_MN 121
#define WIN_MN 122

#define NO_BOATS 3

#define PLAYER 0

int curPlayer = PLAYER;

enum orientation
{
    NORTH,
    EAST,
    SOUTH,
    WEST
};

enum orientation curOrientation = EAST;

typedef struct boat
{
    int length;
} boat;

boat boats[NO_BOATS] = {
    {.length = 2},
    {.length = 3},
    {.length = 4}};

int position = 0;

const int cellHeight = (LCDWIDTH - 50) / NoRowColDef;
const int cellWidth = (LCDWIDTH - 50) / NoRowColDef;
const int NoRowCols = 10;
int gridTotalWidth;
int gridTotalHeight;
int gridStartLeftPos;
int gridStartTopPos;

int cursorX = 0;
int cursorY = 0;

int8_t encoderPosition = 0;

#endif
