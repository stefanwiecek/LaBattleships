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
#include <laBattleships.h>
#include <graphics.h>
#include <util/delay.h>

int update_dial(int);
int collect_delta(int);
int show_ram(int);
void initializeGrid(int grid[NoRowColDef][NoRowColDef]);
void updateDisplayCoords();
void drawFilledRectangle(rectangle r, uint16_t fillCol, uint16_t lineCol);
void init();
void add_miss(uint8_t res);
void add_hit(uint8_t res);
int getHitOrMissOrWon(uint8_t cellIndex);
void show_win();
int gameSwitchCheck(int);
int placementSwitchCheck(int state);

// This is your starting grid - you won't see this
// 0 for blank, 1 for boat, 2 for hit
int player1Grid[NoRowColDef][NoRowColDef] = {
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

int placementSwitchTask;
int gameSwitchTask;

int curShipPosX = 0;
int curShipPosY = 0;
int curPlacingShip = 0;
int shipPlacementError = 0;
void main(void)
{
	os_init();
	USART_Init(9600);
	init();
}

void init()
{
	gridTotalWidth = cellWidth * NoRowColDef;
	gridTotalHeight = cellHeight * NoRowColDef;
	gridStartLeftPos = (LCDHEIGHT - gridTotalWidth) / 2;
	gridStartTopPos = (LCDWIDTH - gridTotalWidth);

	os_add_task(collect_delta, 500, 1);

	display_string_xy("LaBattleships", (LCDHEIGHT / 2) - 35, (LCDWIDTH / 2) - 2.5); // Splash screen

	_delay_ms(500);

	initShipPlacement();

	sei();
	for (;;)
	{
	}
}

void initShipPlacement()
{
	clear_screen();

	os_led_brightness(255);

	initializePlacingGrid(player1Grid);
	drawBoat(0);
	placementSwitchTask = os_add_task(placementSwitchCheck, 100, 1);
}

void initGame()
{
	os_remove_task(placementSwitchTask);
	os_led_brightness(0);
	clear_screen();

	display_string_xy("Get ready!", (LCDHEIGHT / 2) - (11 * 5) / 2, (LCDWIDTH / 2) - 10.5); // Splash screen
	if (PLAYER == 0)
	{
		display_string_xy("Player 0", (LCDHEIGHT / 2) - (9 * 5) / 2, (LCDWIDTH / 2) + 2.5); // Splash screen
	}
	else
	{
		display_string_xy("Player 1", (LCDHEIGHT / 2) - (9 * 5) / 2, (LCDWIDTH / 2) + 2.5); // Splash screen
	}

	_delay_ms(3000);

	initializeGrid(player2Grid);

	if (PLAYER == 0)
	{
		display_string_xy("Play 0", 5, 35);
	}
	else
	{
		display_string_xy("Play 1", 5, 35);
	}

	placementSwitchTask = os_add_task(gameSwitchCheck, 100, 1);
}

void clearBoat(int boat)
{
	int i;
	int x = curShipPosX;
	int y = curShipPosY;

	// <-
	if (curOrientation == WEST)
	{
		x = x - (boats[boat].length - 1);
	}

	// ^
	// |
	if (curOrientation == NORTH)
	{
		y = y - (boats[boat].length - 1);
	}

	for (i = 0; i < boats[boat].length; i++)
	{
		if (player1Grid[y][x] == 1)
		{
			player1Grid[y][x] = 0;
		}
		else if (player1Grid[y][x] == 2)
		{
			player1Grid[y][x] = 1;
		}
		if (curOrientation == NORTH || curOrientation == SOUTH)
		{
			reDrawPlacingCell(x, y);
			y++;
		}
		else if (curOrientation == EAST || curOrientation == WEST)
		{
			reDrawPlacingCell(x, y);
			x++;
		}
	}
}

void drawBoat(int boat)
{
	shipPlacementError = 0;

	int i;
	int x = curShipPosX;
	int y = curShipPosY;

	// <-
	if (curOrientation == WEST)
	{
		x = x - (boats[boat].length - 1);
	}

	// ^
	// |
	if (curOrientation == NORTH)
	{
		y = y - (boats[boat].length - 1);
	}

	for (i = 0; i < boats[boat].length; i++)
	{
		if (player1Grid[y][x] == 0)
		{
			player1Grid[y][x] = 1;
		}
		else
		{
			if (shipPlacementError == 0)
			{

				shipPlacementError = 1;
			}
			player1Grid[y][x] = 2;
		}
		if (curOrientation == NORTH || curOrientation == SOUTH)
		{
			reDrawPlacingCell(x, y);
			y++;
		}
		else if (curOrientation == EAST || curOrientation == WEST)
		{
			reDrawPlacingCell(x, y);
			x++;
		}
	}
	if (shipPlacementError == 1)
	{
		displayMessageTop("Boats are overlapping!");
	}
	else
	{
		displayMessageTop("");
	}
}

void decrementOrientation()
{

	if (curOrientation == NORTH && curShipPosX - boats[curPlacingShip].length + 1 >= 0)
	{
		curOrientation = WEST;
	}
	else if (curOrientation == EAST && curShipPosY - boats[curPlacingShip].length + 1 >= 0)
	{
		curOrientation = NORTH;
	}
	else if (curOrientation == SOUTH && curShipPosX < NoRowColDef - boats[curPlacingShip].length + 1)
	{
		curOrientation = EAST;
	}
	else if (curOrientation == WEST && curShipPosY < NoRowColDef - boats[curPlacingShip].length + 1)
	{
		curOrientation = SOUTH;
	}
	updateDisplayCoords(curShipPosX, curShipPosY);
}

void incrementOrientation()
{
	if (curOrientation == NORTH && curShipPosX < NoRowColDef - boats[curPlacingShip].length + 1)
	{
		curOrientation = EAST;
	}
	else if (curOrientation == EAST && curShipPosY < NoRowColDef - boats[curPlacingShip].length + 1)
	{
		curOrientation = SOUTH;
	}
	else if (curOrientation == SOUTH && curShipPosX - boats[curPlacingShip].length + 1 >= 0)
	{
		curOrientation = WEST;
	}
	else if (curOrientation == WEST && curShipPosY - boats[curPlacingShip].length + 1 >= 0)
	{
		curOrientation = NORTH;
	}
	updateDisplayCoords(curShipPosX, curShipPosY);
}

void updateDisplayCoords(int x, int y)
{
	char xValue[10];
	sprintf(xValue, "(%d,%d)", x, y);
	display_string_xy(xValue, 5, 5);

	if (curOrientation == NORTH)
	{
		display_string_xy("North", 5, 20);
	}
	else if (curOrientation == EAST)
	{

		display_string_xy("East ", 5, 20);
	}
	else if (curOrientation == SOUTH)
	{

		display_string_xy("South ", 5, 20);
	}
	else if (curOrientation == WEST)
	{

		display_string_xy("West ", 5, 20);
	}
}

void initializePlacingGrid(int grid[NoRowColDef][NoRowColDef])
{
	clear_screen();
	updateDisplayCoords(curShipPosX, curShipPosY);
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
			// uint16_t outline = GridLineColor;

			// 0 for blank, 1 for ship placed, 2 for ship overlapping so error
			if (grid[j][i] == 0)
			{
				drawFilledRectangle(r, BackgroundColor, GridLineColor);
			}
			else if (grid[j][i] == 1)
			{
				drawFilledRectangle(r, ShipColor, GridLineColor);
			}
			else if (grid[j][i] == 2)
			{
				drawFilledRectangle(r, ErrorColor, GridLineColor);
			}

			top += cellHeight;
			bottom += cellHeight;
		}
		left += cellWidth;
		right += cellWidth;
	}
}

void initializeGrid(int grid[NoRowColDef][NoRowColDef])
{
	clear_screen();
	updateDisplayCoords(cursorX, cursorY);
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
			if (grid[j][i] == 0)
			{
				drawFilledRectangle(r, BackgroundColor, outline);
			}
			else if (grid[j][i] == 1)
			{
				drawFilledRectangle(r, MissColor, outline);
			}
			else if (grid[j][i] == 2)
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

void reDrawPlacingCell(int x, int y)
{
	int left = gridStartLeftPos + (x * cellWidth);
	int right = gridStartLeftPos + (x * cellWidth) + cellWidth;
	int top = gridStartTopPos + (y * cellHeight);
	int bottom = gridStartTopPos + (y * cellHeight) + cellHeight;
	rectangle r = {left, right, top, bottom};

	// uint16_t outline = GridLineColor;

	// 0 for blank, 1 for ship placed, 2 for ship overlapping so error
	if (player1Grid[y][x] == 0)
	{
		drawFilledRectangle(r, BackgroundColor, GridLineColor);
	}
	else if (player1Grid[y][x] == 1)
	{
		drawFilledRectangle(r, ShipColor, GridLineColor);
	}
	else if (player1Grid[y][x] == 2)
	{
		drawFilledRectangle(r, ErrorColor, GridLineColor);
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

int collect_delta(int state)
{
	position += os_enc_delta();
	return state;
}

uint8_t indexToX(uint8_t cellIndex)
{
	return cellIndex % NoRowColDef;
}

uint8_t indexToY(uint8_t cellIndex)
{
	return (uint8_t)floor((double)cellIndex / (double)NoRowColDef);
}

uint8_t coordsToIndex(uint8_t x, uint8_t y)
{
	return x + (y * NoRowColDef);
}

void add_miss(uint8_t res)
{
	player2Grid[cursorY][cursorX] = 1;
	reDrawCell(cursorX, cursorY);
}

void add_hit(uint8_t res)
{
	player2Grid[cursorY][cursorX] = 2;
	reDrawCell(cursorX, cursorY);
}

int getHitOrMissOrWon(uint8_t cellIndex)
{
	uint8_t x = indexToX(cellIndex);
	uint8_t y = indexToY(cellIndex);

	int won = 1;
	int i, j;
	for (i = 0; i < NoRowCols; i++)
	{
		for (j = 0; j < NoRowCols; j++)
		{
			if (player1Grid[j][i] == 1)
			{
				won = 0;
				break;
			}
		}
	}

	if (won == 1)
	{
		clear_screen();
		rectangle r = {0, LCDHEIGHT, 0, LCDWIDTH};
		drawFilledRectangle(r , RED, RED);
		// height = (number of chars * 5) / 2
		display_string_xy("you lost", (LCDHEIGHT / 2) - 20, (LCDWIDTH / 2) - 2.5);
		return WIN_MN;
	}
	if (player1Grid[y][x] == 1)
		return HIT_MN;
	if (player1Grid[y][x] == 0)
		return MISS_MN;
	return MISS_MN;
}

uint8_t getValueAtCoords(uint8_t x, uint8_t y)
{
	return player2Grid[y][x];
}

int gameSwitchCheck(int state)
{
	if (curPlayer == 0)
	{
		displayMessageTop("It is your turn!");
		// LED_OFF;
		os_led_brightness(255);
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
			updateDisplayCoords(cursorX, cursorY);
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
			updateDisplayCoords(cursorX, cursorY);
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
			updateDisplayCoords(cursorX, cursorY);
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
			updateDisplayCoords(cursorX, cursorY);
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
			while (waitForMsg)
			{
				uint8_t res = USART_Wait_And_Receive();

				if (res == HIT_MN)
				{
					// display_string("Got a hit message");
					// display_string("GOT HIT");
					displayMessageBottom("You hit!");
					add_hit(res);
					curPlayer = 1;
					waitForMsg = 0;
				}
				else if (res == MISS_MN)
				{
					// display_string("Got a miss message");
					// display_string("GOT MISS");
					displayMessageBottom("You missed!");
					add_miss(res);
					curPlayer = 1;
					waitForMsg = 0;
				}
				else if (res == WIN_MN)
				{
					show_win();
				}
				else
				{
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
	}
	else if (curPlayer == 1)
	{
		// Waiting to recieve a square from the other player
		// LED_ON;
		os_led_brightness(0);
		displayMessageTop("Waiting for other player...");
		uint8_t res = USART_Wait_And_Receive();
		int waitForMsg = 1;
		while (waitForMsg)
		{
			if (res <= 99)
			{
				int val = getHitOrMissOrWon(res);
				player1Grid[indexToY(res)][indexToX(res)] = 2;
				// display_string("Other player sent position");
				USART_Transmit(val);
				// display_string("Sending back HIT");
				if (val == WIN_MN)
				{
					curPlayer = 2;
				}
				else
				{
					curPlayer = 0;
				}
				waitForMsg = 0;
			}
		}

		// Reply whether hit or miss or won
	}
	return state;
}

int placementSwitchCheck(int state)
{
	if (get_switch_press(_BV(SWN)))
	{
		if (curShipPosY > 0 && curOrientation != North)
		{
			clearBoat(curPlacingShip);
			curShipPosY--;
			drawBoat(curPlacingShip);
		}
		else if (curShipPosY - boats[curPlacingShip].length + 1 > 0)
		{
			clearBoat(curPlacingShip);
			curShipPosY--;
			drawBoat(curPlacingShip);
		}
		updateDisplayCoords(curShipPosX, curShipPosY);
	}

	if (get_switch_press(_BV(SWE)))
	{
		if (curShipPosX < NoRowColDef - 1 && curOrientation != EAST)
		{
			clearBoat(curPlacingShip);
			curShipPosX++;
			drawBoat(curPlacingShip);
		}
		else if (curShipPosX + boats[curPlacingShip].length - 1 < NoRowColDef - 1)
		{
			clearBoat(curPlacingShip);
			curShipPosX++;
			drawBoat(curPlacingShip);
		}
		updateDisplayCoords(curShipPosX, curShipPosY);
	}

	if (get_switch_press(_BV(SWS)))
	{
		if (curShipPosY < NoRowColDef - 1 && curOrientation != SOUTH)
		{
			clearBoat(curPlacingShip);
			curShipPosY++;
			drawBoat(curPlacingShip);
		}
		else if (curShipPosY + boats[curPlacingShip].length - 1 < NoRowColDef - 1)
		{
			clearBoat(curPlacingShip);
			curShipPosY++;
			drawBoat(curPlacingShip);
		}
		updateDisplayCoords(curShipPosX, curShipPosY);
	}

	if (get_switch_press(_BV(SWW)))
	{
		if (curShipPosX > 0 && curOrientation != WEST)
		{
			clearBoat(curPlacingShip);
			curShipPosX--;
			drawBoat(curPlacingShip);
		}
		else if (curShipPosX - boats[curPlacingShip].length + 1 > 0)
		{
			clearBoat(curPlacingShip);
			curShipPosX--;
			drawBoat(curPlacingShip);
		}
		updateDisplayCoords(curShipPosX, curShipPosY);
	}

	if (get_switch_press(_BV(SWC)))
	{
		if (shipPlacementError == 0)
		{
			if (curPlacingShip == NO_BOATS - 1)
			{
				initGame();
			}
			else
			{
				curPlacingShip++;
				curShipPosX = 0;
				curShipPosY = 0;
				curOrientation = EAST;
				drawBoat(curPlacingShip);
			}
		}
	}

	int8_t tempPos = os_enc_delta();
	if (tempPos > encoderPosition)
	{
		encoderPosition = os_enc_delta();
		clearBoat(curPlacingShip);
		orientation prevOrientation = curOrientation;
		incrementOrientation();
		drawBoat(curPlacingShip);
		if (prevOrientation == curOrientation)
		{
			displayMessageTop("Can't place boat there");
		}
	}
	else if (tempPos < encoderPosition)
	{
		encoderPosition = os_enc_delta();
		clearBoat(curPlacingShip);
		orientation prevOrientation = curOrientation;
		decrementOrientation();
		drawBoat(curPlacingShip);
		if (prevOrientation == curOrientation)
		{
			displayMessageTop("Can't place boat there");
		}
	}

	return state;
}