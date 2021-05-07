/*
LaBattleships

Developed by 
- Will Kelly - wdk1g19
- Stefan Wiecek- saw1g19
*/

#include "os.h"
#include "usart.h"
#include <math.h>
#include <string.h>

// Additional colour definitions
#define C_BLUE 0x3C1E
#define C_LIGHT_GREY  0xD69A 

// Colours
#define GridLineColor C_BLUE
#define BackgroundColor WHITE
#define MissColor C_LIGHT_GREY
#define DestroyedColor RED
#define CursorColour MAGENTA

// Drawing info
#define LineThickness 2
#define NoRowColDef 10

#define HIT_MN 120
#define MISS_MN 121
#define WIN_MN 122

#define PLAYER 1

int curPlayer = PLAYER;

enum orientation
{
	NORTH,
	EAST,
	SOUTH,
	WEST
};

enum orientation curOrientation;

int update_dial(int);
int collect_delta(int);
int check_switches(int);
int show_ram(int);
void initializeGrid();
void updateDisplayCoords();
void drawFilledRectangle(rectangle r, uint16_t fillCol, uint16_t lineCol);
void init();
void add_miss(uint8_t res);
void add_hit(uint8_t res);
int getHitOrMissOrWon(uint8_t cellIndex);
void show_win();


int position = 0;

const int cellHeight = (LCDWIDTH - 50) / NoRowColDef;
const int cellWidth = (LCDWIDTH - 50) / NoRowColDef;
const int NoRowCols = 10;
int gridTotalWidth;
int gridTotalHeight;
int gridStartLeftPos;
int gridStartTopPos;

// This is your starting grid - you wont see this
// 0 for blank, 1 for boat, 2 for hit
int player1Grid[NoRowColDef][NoRowColDef] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 1, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};


// This is the other players grid which gets displayed on your laFortuna
// 0 for blank, 1 for ship not hit, 2 for ship hit
int player2Grid[NoRowColDef][NoRowColDef] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int cursorX = 0;
int cursorY = 0;

int8_t encoderPosition = 0;

char* prevMsgTop = "";
char* prevMsgBottom = "";

void main(void)
{
	os_init();
	// Splash screen here


	USART_Init(9600);
	os_add_task(collect_delta, 500, 1);
	os_add_task(check_switches, 100, 1);

	init();
}

void init()
{
	gridTotalWidth = cellWidth * NoRowColDef;
	gridTotalHeight = cellHeight * NoRowColDef;
	gridStartLeftPos = (LCDHEIGHT - gridTotalWidth) / 2;
	gridStartTopPos = (LCDWIDTH - gridTotalWidth);

	initializeGrid();
	
	if (PLAYER == 0){
		display_string_xy("Play 0" , 5,35);
	} else {
		display_string_xy("Play 1" , 5, 35);
	}

	sei();
	for (;;)
	{
	}
}

void displayMessageTop(char* msg){
	if (strcmp(prevMsgTop, msg) != 0){
		rectangle r = {gridStartLeftPos, gridStartLeftPos + gridTotalWidth, 0, gridStartTopPos/2};
		drawFilledRectangle(r, BLACK, BLACK);
		display_string_xy(msg , gridStartLeftPos, gridStartTopPos /4);
		prevMsgTop = msg;
	}
}

void displayMessageBottom(char* msg){
	if (strcmp(prevMsgBottom, msg) != 0){
		rectangle r = {gridStartLeftPos, gridStartLeftPos + gridTotalWidth, gridStartTopPos/2, gridStartTopPos};
		drawFilledRectangle(r, BLACK, BLACK);
		display_string_xy(msg , gridStartLeftPos, (gridStartTopPos /4)*3);
		prevMsgBottom = msg;
	}
}

void decrementOrientation()
{
	if (curOrientation == NORTH)
	{
		curOrientation = WEST;
	}
	else if (curOrientation == EAST)
	{
		curOrientation = NORTH;
	}
	else if (curOrientation == SOUTH)
	{
		curOrientation = EAST;
	}
	else if (curOrientation == WEST)
	{
		curOrientation = SOUTH;
	}
	updateDisplayCoords();
}

void incrementOrientation()
{
	if (curOrientation == NORTH)
	{
		curOrientation = EAST;
	}
	else if (curOrientation == EAST)
	{
		curOrientation = SOUTH;
	}
	else if (curOrientation == SOUTH)
	{
		curOrientation = WEST;
	}
	else if (curOrientation == WEST)
	{
		curOrientation = NORTH;
	}
	updateDisplayCoords();
}

void updateDisplayCoords()
{
	char xValue[10];
	sprintf(xValue, "(%d,%d)", cursorX, cursorY);
	display_string_xy(xValue, 5, 5);

	if (curOrientation == NORTH)
	{
		display_string_xy("North", 5, 20);
	}
	else if (curOrientation == EAST)
	{

		display_string_xy("East", 5, 20);
	}
	else if (curOrientation == SOUTH)
	{

		display_string_xy("South", 5, 20);
	}
	else if (curOrientation == WEST)
	{

		display_string_xy("West", 5, 20);
	}
}

void initializeGrid()
{
	clear_screen();
	updateDisplayCoords();
	int left = gridStartLeftPos;
	int right = gridStartLeftPos + cellWidth;
	int top = gridStartTopPos;
	int bottom = gridStartTopPos + cellHeight;
	int i, j = 0;
	for (i = 0; i < NoRowCols; i++)
	{
		top = gridStartTopPos;
		bottom = gridStartTopPos + cellHeight;
		for (j = 0; j < NoRowCols; j++)
		{
			rectangle r = {left, right, top, bottom};
			uint16_t outline = GridLineColor;
			if (i == cursorX && j == cursorY)
			{
				// drawFilledRectangle(r, CursorColour, CursorColour);
				outline = CursorColour;
			}
			
			// 0 for blank, 1 for ship not hit, 2 for ship hit
			if (player2Grid[j][i] == 0)
			{
				drawFilledRectangle(r, BackgroundColor, outline);
			}
			else if (player2Grid[j][i] == 1)
			{
				drawFilledRectangle(r, MissColor, outline);
			}
			else if (player2Grid[j][i] == 2)
			{
				drawFilledRectangle(r, DestroyedColor, outline);
			} 
			
			top += cellHeight;
			bottom += cellHeight;
		}
		left += cellWidth;
		right += cellWidth;
	}
}

void reDrawCell(int x, int y)
{
	int left = gridStartLeftPos + (x * cellWidth);
	int right = gridStartLeftPos + (x * cellWidth) + cellWidth;
	int top = gridStartTopPos + (y * cellHeight);
	int bottom = gridStartTopPos + (y * cellHeight) + cellHeight;
	rectangle r = {left, right, top, bottom};


 	uint16_t outline = GridLineColor;
	

	if (x == cursorX && y == cursorY)
	{
		// drawFilledRectangle(r, CursorColour, CursorColour);
		outline = CursorColour;
	}
	
	// 0 for blank, 1 for ship not hit, 2 for ship hit
	if (player2Grid[y][x] == 0)
	{
		drawFilledRectangle(r, BackgroundColor, outline);
	}
	else if (player2Grid[y][x] == 1)
	{
		drawFilledRectangle(r, MissColor, outline);
	}
	else if (player2Grid[y][x] == 2)
	{
		drawFilledRectangle(r, DestroyedColor, outline);
	} 
		
	
}

void drawFilledRectangle(rectangle r, uint16_t fillCol, uint16_t lineCol)
{
	fill_rectangle(r, lineCol);
	rectangle innerRect = {r.left + LineThickness, r.right - LineThickness, r.top + LineThickness, r.bottom - LineThickness};
	fill_rectangle(innerRect, fillCol);
}

int collect_delta(int state)
{
	position += os_enc_delta();
	return state;
}

uint8_t indexToX(uint8_t cellIndex){
	return cellIndex % NoRowColDef;
}

uint8_t indexToY(uint8_t cellIndex){
	return (uint8_t) floor( (double) cellIndex / (double) NoRowColDef);
}

uint8_t coordsToIndex(uint8_t x, uint8_t y) {
	return x + (y * NoRowColDef);
}

void add_miss(uint8_t res){
	player2Grid[cursorY][cursorX] = 1;
	reDrawCell(cursorX, cursorY);

}

void add_hit(uint8_t res) {
	player2Grid[cursorY][cursorX] = 2;
	reDrawCell(cursorX, cursorY);
}

int getHitOrMissOrWon(uint8_t cellIndex) {
	uint8_t x = indexToX(cellIndex);
	uint8_t y = indexToY(cellIndex);

	int won = 1;
	int i,j;
	for (i = 0; i < NoRowCols; i++)
	{
		for (j = 0; j < NoRowCols; j++)
		{
			if (player1Grid[j][i] == 1){
				won= 0;
				break;
			}
		}
	}
	
	if (won == 1) {
		clear_screen();
		displayMessageTop("You lost");
		return WIN_MN;
	}
	if (player1Grid[y][x] == 1) return HIT_MN; 
	if (player1Grid[y][x] == 0) return MISS_MN;
	return MISS_MN;
}

uint8_t getValueAtCoords(uint8_t x, uint8_t y) {
	return player2Grid[y][x];
}

void show_win(){
	// draw big red rectange with white text centre of screen
	// clear screen
	// play again?
	
	clear_screen();
	displayMessageTop("You won");
}

int check_switches(int state)
{
	if (curPlayer == 0){
		displayMessageTop("It is your turn!");
		LED_OFF;

		// Selecting a square and sending command
		if (get_switch_press(_BV(SWN)))
		{
			// display_string("North\n");
			int prevCursorX = cursorX;
			int prevCursorY = cursorY;
			cursorY -= 1;
			if (cursorY < 0)
			{
				cursorY = 0;
			}
			updateDisplayCoords();
			reDrawCell(prevCursorX, prevCursorY);
			reDrawCell(cursorX, cursorY);
		}

		if (get_switch_press(_BV(SWE)))
		{
			// display_string("East\n");
			int prevCursorX = cursorX;
			int prevCursorY = cursorY;
			cursorX += 1;
			if (cursorX >= NoRowCols)
			{
				cursorX = NoRowCols - 1;
			}
			updateDisplayCoords();
			reDrawCell(prevCursorX, prevCursorY);
			reDrawCell(cursorX, cursorY);
		}

		if (get_switch_press(_BV(SWS)))
		{
			// display_string("South\n");

			int prevCursorX = cursorX;
			int prevCursorY = cursorY;
			cursorY += 1;
			if (cursorY >= NoRowCols)
			{
				cursorY = NoRowCols - 1;
			}
			updateDisplayCoords();
			reDrawCell(prevCursorX, prevCursorY);
			reDrawCell(cursorX, cursorY);
		}

		if (get_switch_press(_BV(SWW)))
		{
			// display_string("West\n");
			int prevCursorX = cursorX;
			int prevCursorY = cursorY;
			cursorX -= 1;
			if (cursorX < 0)
			{
				cursorX = 0;
			}
			updateDisplayCoords();
			reDrawCell(prevCursorX, prevCursorY);
			reDrawCell(cursorX, cursorY);
		}

		// if (get_switch_long(v_BV(SWC)))
		// {
		// 	display_string("[L] center");
		// }

		if (get_switch_short(_BV(SWC)))
		{
			USART_Transmit(cursorX + (10 * cursorY));
			// display_string("Sent chosen position");

			int waitForMsg = 1;
			while (waitForMsg){
				uint8_t res = USART_Wait_And_Receive();

				if (res == HIT_MN){
					// display_string("Got a hit message");
					// display_string("GOT HIT");
					displayMessageBottom("Got hit!");
					add_hit(res);
					curPlayer = 1;
					waitForMsg = 0;
				} else if (res == MISS_MN){
					// display_string("Got a miss message");
					// display_string("GOT MISS");
					displayMessageBottom("Got miss!");
					add_miss(res);
					curPlayer = 1;
					waitForMsg = 0;
				} else if (res == WIN_MN){
					show_win();
				} else {
					// display_string("Got a dead message");
					// Something went wrong with communication :/
				}
			}
			
		}

		// if (get_switch_rpt(_BV(SWN)))
		// {
		// 	display_string("[R] North\n");
		// }

		// if (get_switch_rpt(_BV(SWE)))
		// {
		// 	display_string("[R] East\n");
		// }

		// if (get_switch_rpt(_BV(SWS)))
		// {
		// 	display_string("[R] South\n");
		// }

		// if (get_switch_rpt(_BV(SWW)))
		// {
		// 	display_string("[R] West\n");
		// }

		// if (get_switch_rpt(SWN))
		// {
		// 	display_string("[R] North\n");
		// }

		int8_t tempPos = os_enc_delta();
		if (tempPos > encoderPosition)
		{
			encoderPosition = os_enc_delta();
			incrementOrientation();
		}
		else if (tempPos < encoderPosition)
		{
			encoderPosition = os_enc_delta();
			decrementOrientation();
		}

	} else if (curPlayer == 1) {
		// Waiting to recieve a square from the other player
		LED_ON;	
		displayMessageTop("Waiting for other player...");
		uint8_t res = USART_Wait_And_Receive();
		int waitForMsg = 1;
		while (waitForMsg){
			if (res <= 99){
				int val = getHitOrMissOrWon(res);
				player1Grid[indexToY(res)][indexToX(res)] = 2;
				// display_string("Other player sent position");
				USART_Transmit(val);
				// display_string("Sending back HIT");
				if (val == WIN_MN){
					curPlayer = 2;
				} else {
					curPlayer = 0;
				}
				waitForMsg = 0;

			}
		}

		// Reply whether hit or miss or won
	}
	return state;
}
