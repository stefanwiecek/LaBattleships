#ifndef graphics
#define graphics

#include "os.h"
#include "laBattleships.h"
#include <string.h>

char *prevMsgTop = "";
char *prevMsgBottom = "";

void show_win();
void drawFilledRectangle(rectangle r, uint16_t fillCol, uint16_t lineCol);
void displayMessageTop(char *msg);
void displayMessageBottom(char *msg);

void show_win()
{
    // draw big red rectange with white text centre of screen
    // clear screen
    // play again?

    clear_screen();
    rectangle r = {0, LCDHEIGHT, 0, LCDWIDTH};
    drawFilledRectangle(r, GREEN, GREEN);
    // height = (number of chars * 5) / 2
	display_string_xy("you won", (LCDHEIGHT / 2) - 17.5, (LCDWIDTH / 2) - 2.5);
}

void drawFilledRectangle(rectangle r, uint16_t fillCol, uint16_t lineCol)
{
    fill_rectangle(r, lineCol);
    rectangle innerRect = {r.left + LineThickness, r.right - LineThickness, r.top + LineThickness, r.bottom - LineThickness};
    fill_rectangle(innerRect, fillCol);
}

void displayMessageTop(char *msg)
{
    if (strcmp(prevMsgTop, msg) != 0)
    {
        rectangle r = {gridStartLeftPos, gridStartLeftPos + gridTotalWidth, 0, gridStartTopPos / 2};
        drawFilledRectangle(r, BLACK, BLACK);
        display_string_xy(msg, gridStartLeftPos, gridStartTopPos / 4);
        prevMsgTop = msg;
    }
}

void displayMessageBottom(char *msg)
{
    if (strcmp(prevMsgBottom, msg) != 0)
    {
        rectangle r = {gridStartLeftPos, gridStartLeftPos + gridTotalWidth, gridStartTopPos / 2, gridStartTopPos};
        drawFilledRectangle(r, BLACK, BLACK);
        display_string_xy(msg, gridStartLeftPos, (gridStartTopPos / 4) * 3);
        prevMsgBottom = msg;
    }
}
#endif