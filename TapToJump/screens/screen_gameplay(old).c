/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
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
#include "c2dmath.h"
#include "ceasings.h"

#include <stdio.h> // printf used on testing

// Defines
#define GAME_SPEED 60

#define CELL_SIZE 32
#define ASSETS_SCALE 2

#define MAX_OBJECT_TYPES 2
#define MAX_TRIANGLES 3

#define TRUE 1
#define FALSE 0

// Enums
typedef enum 
{
    OT_PLAYER=0,
    OT_TRIANGLE,
}ObjectTypes;

// Sctructs
typedef struct StaticObject
{
    ObjectTypes objType;
    Vector2 position;
    float rotation;
    float scale;
}StaticObject;

typedef struct Camera2D
{
    Vector2 direction;
    Vector2 speed;
    Vector2 position;
    bool isMoving;
}Camera2D;

typedef struct GravityForce
{
    Vector2 direction;
    float value;
}GravityForce;

typedef struct DynamicObject
{
    ObjectTypes objType; 
    Vector2 position;
    float rotation;
    float scale;
    Vector2 direction;
    Vector2 speed;
    Vector2 velocity;
    bool isGrounded;
    int onAirCounter;
}DynamicObject;

typedef struct Easing
{
    float t, b, c, d;
    bool isFinished;
}Easing;

typedef struct Player
{
    DynamicObject dnObj;
    Easing rotationEasing;
}Player;

//----------------------------------------------------------------------------------
// Global Variables Definition (local to this module)
//----------------------------------------------------------------------------------
// Gameplay screen global variables
static int framesCounter;
static int finishScreen;

//TESTING & DEBUGGING
bool pause;

// Game camera
Camera2D mainCamera;

// Gravity
GravityForce gravity;

// Game SquareObjects (On Game Grid Objects)
Player player;
StaticObject triangle[MAX_TRIANGLES];

// SquareObject textures
Texture2D objectTextures[MAX_OBJECT_TYPES];

int groundCoordinadeY;
int groundPositionY;
//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
Vector2 GetOnGridPosition(Vector2 coordinates);
Vector2 GetOnGridObjectPosition(Vector2 coordinates, Texture2D texture);
Vector2 GetOnGridPlayerPosition(Vector2 coordinates, Texture2D texture);
void InitializeStaticObject(StaticObject *a, ObjectTypes objType, Vector2 coordinates);
void InitializeDynamicObject(DynamicObject *a, ObjectTypes objType, Vector2 coordinates, float scale, Vector2 speed, bool isGrounded);
void DrawObjectOnCameraPosition(Texture2D texture, Vector2 position);
void DrawDynamicObject(Texture2D texture, DynamicObject dnObj);
void UpdateDynamicObjectMovement(DynamicObject *dnObj, int positionOffset);
void SetDynamicObjectAsGrounded(DynamicObject *dnObj);
Vector2 GetGravityForce();
void UpdatePlayer ();
void UpdateMainCamera();
void UpdateEasing(Easing *easing, float *value);
void StartEasing(Easing *easing);
void FinishEasing(Easing *easing);

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    
    //DEBUGGING && TESTING variables
    pause = FALSE;
    
    // Textures loading
    objectTextures[OT_PLAYER] = LoadTexture("assets/gameplay_screen/cube_blue.png");
    objectTextures[OT_TRIANGLE] = LoadTexture("assets/gameplay_screen/triangle_main.png");
    
    // Camera initialization
    mainCamera = (Camera2D){Vector2Right(), (Vector2){4, 4}, Vector2Zero(), TRUE};
    
    // Gravity initialization
    gravity = (GravityForce){Vector2Up(), 1.75f};
    
    // Ground position
    groundCoordinadeY = GetScreenHeight()/CELL_SIZE-1;
    groundPositionY = GetOnGridPosition((Vector2){0, groundCoordinadeY}).y;
    
    // Player initialization
    InitializeDynamicObject(&player.dnObj, OT_PLAYER, (Vector2){4, groundCoordinadeY}, ASSETS_SCALE, (Vector2){0, 7}, TRUE);
    player.rotationEasing = (Easing){0, 0, -90, 0.7f*GAME_SPEED};

    // Triangles initialization
    InitializeStaticObject(&triangle[0], OT_TRIANGLE, (Vector2){20, groundCoordinadeY});
    InitializeStaticObject(&triangle[1], OT_TRIANGLE, (Vector2){30, groundCoordinadeY});
    InitializeStaticObject(&triangle[2], OT_TRIANGLE, (Vector2){50, groundCoordinadeY});
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    if (IsKeyDown('P')) pause = !pause;
    if (!pause)
    {     
        // TODO: Update GAMEPLAY screen variables here!
        UpdateMainCamera();
        UpdatePlayer();
    }
    // Press enter to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER))
    {
        finishScreen = 1;
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    
    DrawDynamicObject(objectTextures[OT_PLAYER], player.dnObj);
    for (int i=0; i<MAX_TRIANGLES; i++)
    {
        DrawObjectOnCameraPosition(objectTextures[OT_TRIANGLE], triangle[i].position);
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}

Vector2 GetOnGridPosition(Vector2 coordinates)
{
    Vector2Scale(&coordinates, CELL_SIZE);
    return coordinates; 
}

Vector2 GetOnGridObjectPosition(Vector2 coordinates, Texture2D texture)
{
    coordinates = Vector2FloatProduct(coordinates, CELL_SIZE);
    coordinates.y-=texture.height*ASSETS_SCALE;
    
    //Vector2Scale(&coordinates, CELL_SIZE);
    return coordinates; 
}

Vector2 GetOnGridPlayerPosition(Vector2 coordinates, Texture2D texture)
{
    Vector2Scale(&coordinates, CELL_SIZE);
    coordinates.y-=texture.height*ASSETS_SCALE/2;
    return coordinates; 
}

void InitializeStaticObject(StaticObject *a, ObjectTypes objType, Vector2 coordinates)
{
    *a = (StaticObject){objType, GetOnGridObjectPosition(coordinates, objectTextures[objType]), 0, ASSETS_SCALE};
}

void InitializeDynamicObject(DynamicObject *a, ObjectTypes objType, Vector2 coordinates, float scale, Vector2 speed, bool isGrounded)
{
    *a = (DynamicObject){objType, GetOnGridPlayerPosition(coordinates, objectTextures[objType]), 0, scale, Vector2Zero(), speed, Vector2Zero(), isGrounded, 0};
}

void DrawObjectOnCameraPosition(Texture2D texture, Vector2 position)
{
    DrawTextureEx(texture, Vector2Sub(position, mainCamera.position), 0, ASSETS_SCALE, WHITE);
}

void DrawDynamicObject(Texture2D texture, DynamicObject dnObj)
{
    //DrawTextureEx(texture, dnObj.position, dnObj.rotation, dnObj.scale, WHITE);
    DrawTexturePro(texture, (Rectangle){0, 0, texture.width, texture.height}, (Rectangle){dnObj.position.x, dnObj.position.y, texture.width*dnObj.scale, texture.height*dnObj.scale}, (Vector2){texture.width*dnObj.scale/2, texture.height*dnObj.scale/2}, dnObj.rotation, WHITE);
}

void SetDynamicObjectAsGrounded(DynamicObject *dnObj)
{
    dnObj->isGrounded = TRUE;
    dnObj->onAirCounter = 0;
    dnObj->direction.y = 0;
    dnObj->velocity.y = 0;
}

void UpdateDynamicObjectMovement(DynamicObject *dnObj, int positionOffset)
{   
    // Update position
    dnObj->position = Vector2Add(dnObj->position, dnObj->velocity);
    
    // Gravity
    if (!dnObj->isGrounded)
    {
        //printf("Player is not grounded\n");
        
        // On air velocity update
        //dnObj->velocity = Vector2Add(dnObj->velocity, GetGravityForce()); // No acceleration
        dnObj->velocity = Vector2Add(dnObj->velocity, Vector2FloatProduct(GetGravityForce(), (float)dnObj->onAirCounter/GAME_SPEED)); // Graity acceleration
        //printf("%.3f\n", dnObj->velocity.y);
        dnObj->onAirCounter++;
        
        // If dnObj collides with the ground
        if (dnObj->position.y>=groundPositionY-positionOffset)
        {
            dnObj->position.y = groundPositionY-positionOffset;
            SetDynamicObjectAsGrounded(dnObj);
        }
    }
}

Vector2 GetGravityForce()
{
    return Vector2FloatProduct(gravity.direction, gravity.value);
}

void UpdatePlayer ()
{   
    if (player.dnObj.isGrounded&&IsKeyDown(KEY_SPACE))
    {
        player.dnObj.isGrounded = FALSE;
        player.dnObj.direction.y = -1;
        player.dnObj.velocity.y = player.dnObj.speed.y*player.dnObj.direction.y;
        StartEasing(&player.rotationEasing);
        //printf("SPACE pressed\n");
    }
    printf("---");
    //player.dnObj.position = Vector2Add(player.dnObj.position, Vector2Product(player.dnObj.direction, player.dnObj.speed));
    UpdateDynamicObjectMovement(&player.dnObj, objectTextures[OT_PLAYER].height*ASSETS_SCALE/2);
    if (player.dnObj.isGrounded) FinishEasing(&player.rotationEasing);
    UpdateEasing(&player.rotationEasing, &player.dnObj.rotation);
    //printf("%0.1f\n", player.rotationEasing.b);
    /*
    if (!player.dnObj.isGrounded)
    {
        UpdateEasing(&player.rotationEasing, &player.dnObj.rotation);
    }
    else
    {
        FinishEasing(&player.rotationEasing);
        UpdateEasing(&player.rotationEasing, &player.dnObj.rotation);
    }
    */
}

void UpdateMainCamera()
{
    if (mainCamera.isMoving) mainCamera.position = Vector2Add(mainCamera.position, Vector2Product(mainCamera.direction, mainCamera.speed));
}

void UpdateEasing(Easing *easing, float *value)
{
    if (!easing->isFinished)
    {
        if (easing->t<=easing->d)
        {
            *value = CubicEaseOut(easing->t, easing->b, easing->c, easing->d);
            easing->t++;
        }
        if (easing->t>=easing->d)
        {
            easing->t = 0;
            easing->b+=easing->c;
            if (easing->b<=-360) easing->b = 0;
            easing->isFinished = TRUE;
        }
    }
}

void StartEasing(Easing *easing)
{
    easing->isFinished = false;
}

void FinishEasing(Easing *easing)
{
    if (!easing->isFinished) easing->t = easing->d;
}