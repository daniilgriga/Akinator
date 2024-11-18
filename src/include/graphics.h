#ifndef GRAPH_H_
#define GRAPH_H_

#pragma once

// #define TX_COMPILED
#include "TXLib.h"
#include <stdbool.h>

const int AkinatorGame = 'a';
const int Definition   = 'd';
const int Comparison   = 'c';
const int Q_N_Write    = 'q';
const int Quit         = 'Q';

const int Yes          = 'y';
const int No           = 'n';

struct Button
{
    int id;
    const char* text;
    COLORREF color;
    int x;
    int y;
    int sizeX;
    int sizeY;
};

void InitGraphics();

void DrawButton (const Button* button);

void DrawButtons (const Button buttons[]);

bool PointInButton (const Button* button, double x, double y);

int SelectButton (const Button buttons[]);

int SelectMode ();

int SelectYesNo (const char* text);

#endif
 