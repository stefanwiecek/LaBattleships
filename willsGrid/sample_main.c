/* COMP2215 Task 5---SKELETON */

#include "os.h"
#include "usart.c"

// Additional colour definitions
#define C_BLUE 0x3C1E

// Colours
#define GridLineColor C_BLUE
#define BackgroundColor WHITE
#define BoatColor GREEN
#define DestroyedColor RED
#define CursorColour YELLOW

// Drawing info
#define LineThickness 2
#define NoRowColDef 10

#define HIT_MN 120
#define MISS_MN 121
#define WIN_MN 122

#define PLAYER 1

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

int position = 0;

const int cellHeight = (LCDWIDTH - 10) / NoRowColDef;
const int cellWidth = (LCDWIDTH - 10) / NoRowColDef;
const int NoRowCols = 10;
int gridTotalWidth;
int gridTotalHeight;
int gridStartLeftPos;
int gridStartTopPos;

// 0 for blank, 1 for miss, 2 for hit
int gridArea[NoRowColDef][NoRwowColDef] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 1, 2, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 2, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 2, 0, 0},
	{0, 0, 0, 0, 1, 0, 0, 2, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

int cursorX = 0;
int cursorY = 0;

int8_t encoderPosition = 0;

void main(void)
{
	os_init();
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
	gridStartTopPos = (LCDWIDTH - gridTotalWidth) / 2;

	initializeGrid();

	sei();
	for (;;)
	{
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
			if (i == cursorX && j == cursorY)
			{
				drawFilledRectangle(r, CursorColour, CursorColour);
			}
			else
			{

				if (gridArea[j][i] == 0)
				{
					drawFilledRectangle(r, BackgroundColor, GridLineColor);
				}
				else if (gridArea[j][i] == 1)
				{
					drawFilledRectangle(r, BoatColor, GridLineColor);
				}
				else
				{
					drawFilledRectangle(r, DestroyedColor, GridLineColor);
				}
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
	if (x == cursorX && y == cursorY)
	{
		drawFilledRectangle(r, CursorColour, CursorColour);
	}
	else
	{

		if (gridArea[y][x] == 0)
		{
			drawFilledRectangle(r, BackgroundColor, GridLineColor);
		}
		else if (gridArea[y][x] == 1)
		{
			drawFilledRectangle(r, BoatColor, GridLineColor);
		}
		else
		{
			drawFilledRectangle(r, DestroyedColor, GridLineColor);
		}
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

void add_miss(){
	gridArea[cursorX][cursorY] = 1;
	reDrawCell(cursorX, cursorY);

}

void add_hit() {
	gridArea[cursorX][cursorY] = 2;
	reDrawCell(cursorX, cursorY);
}

int getHitOrMissOrWon(uint8_t cellIndex) {
	uint8_t y = cellIndex % 10;
	uint8_t x = cellIndex  10; 
}

void show_win(){
	// draw big red rectange with white text centre of screen
	// clear screen
	// play again?
	clear_screen();
}

int check_switches(int state)
{

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

		// if (get_switch_long(_BV(SWC)))
		// {
		// 	display_string("[L] center");
		// }

		if (get_switch_short(_BV(SWC)))
		{
			if (PLAYER == 0){
				USART_Transmit(cursorX + (10 * cursorY));
				uint8_t res = USART_Wait_And_Receive();
				if (res == HIT_MN){
					addHit(res);
				} else if (res == MISS_MN){
					addMiss(res);
				} else if (res == WIN_MN){
					showWin();
				} else {
					// Something went wrong with communication :/
				}
				
				uint8_t res = USART_Wait_And_Receive();
				// Reply whether hit or miss or won
				USART_Transmit(getHitOrMissOrWon(res));
			} else {				
				uint8_t res = USART_Wait_And_Receive();
				// Reply whether hit or miss or won
				USART_Transmit(getHitOrMissOrWon(res));

				USART_Transmit(cursorX + (10 * cursorY));
				uint8_t res = USART_Wait_And_Receive();
				if (res == HIT_MN){
					addHit(res);
				} else if (res == MISS_MN){
					addMiss(res);
				} else if (res == WIN_MN){
					showWin();
				} else {
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
	
	return state;
}
