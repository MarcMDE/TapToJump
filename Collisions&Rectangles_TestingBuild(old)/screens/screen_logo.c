/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Logo Screen Functions Definitions (Init, Update, Draw, Unload)
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

#define LOGOSCALE 10

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------

// Logo screen global variables
static int framesCounter;
static int finishScreen;

Texture2D logoTexture;
float logoAlpha, logoFadeCounter, logoFadeDelay, logoFadeInDuration, logoDuration;
//----------------------------------------------------------------------------------
// Logo Screen Functions Definition
//----------------------------------------------------------------------------------

// Logo Screen Initialization logic
void InitLogoScreen(void)
{
    // TODO: Initialize LOGO screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    
    logoFadeCounter = 0;
    logoFadeDelay = 1*60;
    logoFadeInDuration = 1.5f*60;
    logoDuration = 1.8f*60;
    logoAlpha = 0;
    
    logoTexture = LoadTexture("assets/logo_Screen/PixelBar_Logo.png");
}

// Logo Screen Update logic
void UpdateLogoScreen(void)
{
    // TODO: Update LOGO screen variables here! 
    if (framesCounter>=logoFadeDelay)
    {
        if (logoFadeCounter<=logoFadeInDuration)
        {
            logoAlpha = SineEaseIn(logoFadeCounter, 0, 1, logoFadeInDuration);
            logoFadeCounter++;
        }
        else if (framesCounter>=logoFadeDelay+logoFadeInDuration+logoDuration) finishScreen = true; // Jump to next screen
    }
    
    framesCounter++;
}

// Logo Screen Draw logic
void DrawLogoScreen(void)
{
    // TODO: Draw LOGO screen here!
    DrawTextureEx(logoTexture, (Vector2){GetScreenWidth()/2-logoTexture.width*LOGOSCALE/2, GetScreenHeight()/2-logoTexture.height*LOGOSCALE/2}, 0, LOGOSCALE, Fade(WHITE, logoAlpha));
}

// Logo Screen Unload logic
void UnloadLogoScreen(void)
{
    // TODO: Unload LOGO screen variables here!
    UnloadTexture(logoTexture);
}

// Logo Screen should finish?
int FinishLogoScreen(void)
{
    return finishScreen;
}