/*******************************************************************************************
*
*   TapToJump (advance_game.c) (v1.0)
*
*   <Simple runner prototype, lots of fun> developed by Marc Montagut - @MarcMDE  
*
*   Copyright (c) 2016 Marc Montagut
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

*   raylib - Advance Game template
*
*   <TapToJump>
*   <Simple runner prototype, lots of fun>
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (Ray San - raysan@raysanweb.com)
*
********************************************************************************************/

#include "raylib.h"
#include "screens/screens.h"    // NOTE: Defines global variable: currentScreen

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------

// Required variables to manage screen transitions (fade-in, fade-out)
float transAlpha = 0;
bool onTransition = false;
bool transFadeOut = false;
int transFromScreen = -1;
int transToScreen = -1;

// Temporal Gameplay error solution. Check line 229
bool firstGameplay = 0;

static const int screenWidth = 800;
static const int screenHeight = 450;
    
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
void TransitionToScreen(int screen);
void UpdateTransition(void);
void DrawTransition(void);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//---------------------------------------------------------
	const char windowTitle[30] = "TapToJump_v1.0 - @MarcMDE";
    
    InitWindow(screenWidth, screenHeight, windowTitle);

    // TODO: Load global data here (assets that must be available in all screens, i.e. fonts)
    
    // Setup and Init first screen
    currentScreen = LOGO;
    InitLogoScreen();
    
    /*
    InitGameplayScreen();
    UnloadGameplayScreen();
    InitGameplayScreen();
    */
    
	SetTargetFPS(60);
	//----------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (!onTransition)
        {
            switch(currentScreen) 
            {
                case LOGO: 
                {
                    UpdateLogoScreen();
                    
                    if (FinishLogoScreen()) TransitionToScreen(TITLE);
                    
                } break;
                case TITLE: 
                {
                    UpdateTitleScreen();
                    
                    if (FinishTitleScreen() == 1) TransitionToScreen(OPTIONS);
                    else if (FinishTitleScreen() == 2) TransitionToScreen(GAMEPLAY);

                } break;
                case OPTIONS:
                {
                    UpdateOptionsScreen();
                    
                    if (FinishOptionsScreen()) TransitionToScreen(TITLE);

                } break;
                case GAMEPLAY:
                { 
                    UpdateGameplayScreen();
                    
                    if (FinishGameplayScreen() == 1) TransitionToScreen(GAMEPLAY);
                    else if (FinishGameplayScreen() == 2) TransitionToScreen(ENDING);
  
                } break;
                case ENDING: 
                {
                    UpdateEndingScreen();
                    
                    if (FinishEndingScreen()) TransitionToScreen(TITLE);

                } break;
                default: break;
            }
        }
        else
        {
            // Update transition (fade-in, fade-out)
            UpdateTransition();
        }
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            switch(currentScreen) 
            {
                case LOGO: DrawLogoScreen(); break;
                case TITLE: DrawTitleScreen(); break;
                case OPTIONS: DrawOptionsScreen(); break;
                case GAMEPLAY: DrawGameplayScreen(); break;
                case ENDING: DrawEndingScreen(); break;
                default: break;
            }
            
            if (onTransition) DrawTransition();
            
            DrawText("@MarcMDE" ,12, 12, 20, WHITE); // "WHATERMARK"
        
            DrawFPS(screenWidth-85, 10);
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    
    // TODO: Unload all global loaded data (i.e. fonts) here!
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
	
    return 0;
}

void TransitionToScreen(int screen)
{
    onTransition = true;
    transFromScreen = currentScreen;
    transToScreen = screen;
}

void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += 0.05f;

        if (transAlpha >= 1.0)
        {
            transAlpha = 1.0;
        
            switch (transFromScreen)
            {
                case LOGO: UnloadLogoScreen(); break;
                case TITLE: UnloadTitleScreen(); break;
                case OPTIONS: UnloadOptionsScreen(); break;
                case GAMEPLAY: UnloadGameplayScreen(); break;
                case ENDING: UnloadEndingScreen(); break;
                default: break;
            }
            
            switch (transToScreen)
            {
                case LOGO:
                {
                    InitLogoScreen();
                    currentScreen = LOGO; 
                } break;
                case TITLE: 
                {
                    InitTitleScreen();
                    currentScreen = TITLE;                  
                } break;
                case OPTIONS:
                {
                    InitOptionsScreen(); 
                    currentScreen = OPTIONS;
                } break;
                case GAMEPLAY:
                {
                    // Temporal Gameplay error solution. Check line 51
                    if (!firstGameplay)
                    {
                        firstGameplay = 1;
                        InitGameplayScreen();
                        UnloadGameplayScreen();
                    }
                    
                    InitGameplayScreen(); 
                    currentScreen = GAMEPLAY;
                } break;
                case ENDING:
                {
                    InitEndingScreen(); 
                    currentScreen = ENDING;
                } break;
                default: break;
            }
            
            transFadeOut = true;
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.01f;
        
        if (transAlpha <= 0)
        {
            transAlpha = 0;
            transFadeOut = false;
            onTransition = false;
            transFromScreen = -1;
            transToScreen = -1;
        }
    }
}

void DrawTransition(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, transAlpha));
}