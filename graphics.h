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
    displayMessageTop("You won");
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