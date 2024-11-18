
#include "graphics.h"
#include "color_print.h"

void InitGraphics ()
{
    txCreateWindow (696, 600);

    HDC backgr = txLoadImage ("bro.bmp");

    txBitBlt (txDC(), 0, 0, 0, 0, backgr);

    txDisableAutoPause ();
}

void DrawButton (const Button* button)
{
    txSetColor (TX_YELLOW);
    txSetFillColor (button->color);

    Win32::RoundRect (txDC (), button->x, button->y,
                      button->x + button->sizeX, button->y + button->sizeY, 15, 15);


    //txRectangle (button->x, button->y,
    //             button->x + button->sizeX, button->y + button->sizeY);

    txDrawText (button->x, button->y, button->x + button->sizeX, button->y + button->sizeY, button->text);
}

void DrawButtons (const Button buttons[])
{
    for (int i = 0; buttons[i].text; i++)
        DrawButton (&buttons[i]);
}

bool PointInButton (const Button* button, double x, double y)
{
    return button->x <= x && x <= (button->x + button->sizeX) &&
    	   button->y <= y && y <= (button->y + button->sizeY);
}

int SelectButton (const Button buttons[])
{
    while (!txGetAsyncKeyState (VK_ESCAPE))
    {
	double x = txMouseX();
	double y = txMouseY();
        int pressed = txMouseButtons();

        Sleep (10); 

        if (pressed == 0)
            continue; 

        int i = 0;
        while (buttons[i].text != NULL)
        {
	    if (PointInButton (&buttons[i], x, y))
                return buttons[i].id;

	    i++;
        }
    }

    return -1;
}

int SelectMode ()
{
    Button menu[] = { { AkinatorGame,                 "akinator game", TX_LIGHTBLUE,  20, 100, 100, 100 },
 		      {   Definition,                    "definition", TX_LIGHTBLUE, 140, 100, 100, 100 },
 		      {   Comparison,                    "comparison", TX_LIGHTBLUE, 260, 100, 100, 100 },
 		      {    Q_N_Write,      "quit n write to database", TX_LIGHTBLUE, 380, 100, 176, 100 },
 		      {         Quit,                          "quit", TX_LIGHTBLUE, 576, 100, 100, 100 },
		      { } };
       
    printf (LIGHT_BLUE_TEXT("\nwhat do you wanna doing?\n"));
    printf (PURPLE_TEXT("[a]")LIGHT_BLUE_TEXT("kinator game, ")             PURPLE_TEXT("[d]")LIGHT_BLUE_TEXT("efinition, ")
            PURPLE_TEXT("[c]")LIGHT_BLUE_TEXT("omparison, ")
            PURPLE_TEXT("[q]")LIGHT_BLUE_TEXT("uit and write to database, ")PURPLE_TEXT("[Q]")LIGHT_BLUE_TEXT("uit\n"));

    int answer = 0;
    
    while (true)
    {
       	DrawButtons (menu);
	
        answer = SelectButton (menu);
	
	if (answer != -1)
	    break;
    }
                  	
    txSetFillColor (TX_BLACK);
    txClear ();
    HDC backgr = txLoadImage ("bro.bmp");
    txBitBlt (txDC(), 0, 0, 0, 0, backgr);

    

    return answer;
}

int SelectYesNo (const char* text)
{
    Button menu[] = { {  -1,  text, TX_LIGHTBLUE, 250, 100, 200, 100 },  // const NO_CHOICE = -1;
		      { Yes, "YES", TX_LIGHTBLUE, 125, 250, 148, 100 },
 		      {  No,  "NO", TX_LIGHTBLUE, 398, 250, 148, 100 },
		      { } };
    
    printf (LIGHT_BLUE_TEXT("\"%s\"?")"\n", text);
    printf (PURPLE_TEXT("[y/n]")"\n");


    while ( txMouseButtons () ) Sleep (20);

    int answer = 0;
    while (true)
    {
       	DrawButtons (menu);
	
        answer = SelectButton (menu);
	
	if (answer != -1)
	    break;
    }
                  	
    txSetFillColor (TX_BLACK);
    txClear ();

    HDC backgr = txLoadImage ("bro.bmp");
    txBitBlt (txDC(), 0, 0, 0, 0, backgr);


    return answer;

}
