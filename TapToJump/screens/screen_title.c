/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Title Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014 Ramon Santamaria (Ray San - raysan@raysanweb.com)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "ceasings.h"

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------

// Title screen global variables
static int framesCounter;
static int finishScreen;

//Texture2D titleTexture;
float titleFadeDelay, titleFadeInDuration, titleAlpha;
bool isTitleAnimFinished;
float fadeFramesCounter;
float startTextDelay, startTextFadeDuration, startTextAlpha;
bool startTextFadeIn;

//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    // TODO: Initialize TITLE screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    
    //titleTexture = LoadTexture("assets/logo_screen/TapToJump_title.png"
    titleFadeDelay = 0.5f*60;
    titleFadeInDuration = 1.0f*60;
    titleAlpha = 0;
    isTitleAnimFinished = 0;
    
    fadeFramesCounter = 0;
    
    startTextDelay = titleFadeDelay+titleFadeInDuration+0.05f*60;
    startTextFadeDuration = 0.4f*60;
    startTextAlpha = 0;
    startTextFadeIn = 1;
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    if (!isTitleAnimFinished)
    {
        if (framesCounter>=titleFadeDelay)
        {
            if (fadeFramesCounter<=titleFadeInDuration)
            {
                titleAlpha = CubicEaseIn(fadeFramesCounter, 0, 1, titleFadeInDuration);
                fadeFramesCounter++;
            }
            else
            {
                fadeFramesCounter = 0;
                isTitleAnimFinished = 1;
                titleAlpha = 1;
            }
        }
        else framesCounter++;
    }
    else
    {
        if (framesCounter>=startTextDelay)
        {
            if (fadeFramesCounter<=startTextFadeDuration)
            {
                if(startTextFadeIn) startTextAlpha = SineEaseOut(fadeFramesCounter, 0.2, 1, startTextFadeDuration);
                else startTextAlpha = SineEaseIn(fadeFramesCounter, 1, -0.8, startTextFadeDuration);
                
                fadeFramesCounter++;
            }
            else
            {
                fadeFramesCounter=0;   
                startTextFadeIn = !startTextFadeIn;
            }
        }
        else framesCounter++;
    }

    if (isTitleAnimFinished&&IsKeyPressed(KEY_ENTER))
    {
        //finishScreen = 1;   // OPTIONS
        finishScreen = 2;   // GAMEPLAY
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    // TODO: Draw TITLE screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLUE);
    //DrawTextureEx(titleTexture, (Vector2){GetScreenWidth()/2-titleTexture.width/2, GetScreenHeight()/2-titleTexture.height/2}, 0, 1, Fade(WHITE, titleAlpha));
    DrawRectangle(GetScreenWidth()/2-200, GetScreenHeight()/2-100, 400, 150, Fade(YELLOW, titleAlpha));
    DrawText("PRESS <ENTER> to START the GAME", 208, GetScreenHeight()-75, 20, Fade(BLACK, startTextAlpha));
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // TODO: Unload TITLE screen variables here!
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    return finishScreen;
}