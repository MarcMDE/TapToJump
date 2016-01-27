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
#include "c2dmath.h" // Simple 2d Maths
#include "ceasings.h" // Izincs!!!

#include <stdio.h> // printf() used on testing
#include <stdlib.h> // malloc() & free()
#include <time.h> // RAND_MAX

// Defines
#define GAME_SPEED 60

#define GRID_WIDTH 100
#define GRID_HEIGHT 14
#define CELL_SIZE 32
#define ASSETS_SCALE 2

#define MAX_TRIANGLES 3
#define MAX_PLATFORMS 5
#define MAX_PARTICLES 60

#define MAX_TRIANGLE_COLLIDING_POINTS 4

// boolean true/false
#define TRUE 1
#define FALSE 0

// Enums
/*
typedef enum 
{
}EnumName;
*/

// Sctructs
typedef struct SquareObject
{
    Vector2 sourcePosition;
    Vector2 position;
    Rectangle collider;
    bool isActive;
    bool isOver;
}SquareObject;

typedef struct TriangleObject
{
    Vector2 sourcePosition;
    Vector2 position;
    Vector2 collidingPoints[4]; // (0 -> botLeft, 1 -> midTop, 2 -> botRight, 3 -> center)
    bool isActive; // The triangle is in the screen
    bool isOver; // The triangle has been used and is out the screen
}TriangleObject;

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

typedef struct Transform2D
{
    Vector2 position;
    float rotation;
    float scale;
}Transform2D;

typedef struct DynamicObject
{
    Rectangle checker;
    Vector2 direction;
    Vector2 speed;
    Vector2 velocity;
    bool isGrounded;
    //int onAirCounter;
}DynamicObject;

typedef struct Easing
{
    float t, b, c, d;
    bool isFinished;
}Easing;

typedef struct Particle
{
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float scale;
    Color color;
    int duration;
    int framesCounter;
    bool isActive;
}Particle;

typedef struct SourceParticle
{
    Vector2 position;
    Vector2 direction;
    Vector2 minSpeed, maxSpeed;
    float minRotation, maxRotation;
    float minScale, maxScale;
    Color aColor, bColor;
    int minDuration, maxDuration;
}SourceParticle;

typedef struct ParticleEmitter
{
    Vector2 position;
    Vector2 offset;
    Texture2D texture;
    SourceParticle source;
    GravityForce gravity;
    Particle *particles;
    int spawnFrequency;
    int framesCounter;
}ParticleEmitter;

typedef struct Player
{
    Transform2D transform;
    DynamicObject dnObj;
    Rectangle collider;
    Easing rotationEasing;
    Color color;
    Texture2D texture;
    ParticleEmitter pEmitter;
    bool isAlive;
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
TriangleObject *triangles;
SquareObject *platforms;
int maxTriangles;
int maxPlatforms;

// SquareObject textures
Texture2D triangleTexture, platformTexture;

int groundCoordinadeY;
int groundPositionY;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------
void UpdateRotationEasing(Easing *easing, float *value);
void StartEasing(Easing *easing);
void FinishEasing(Easing *easing);
Vector2 GetOnGridPosition(Vector2 coordinates);
void SetPlayerAsGrounded(Vector2 newPosition);
void InitializePlayer(Player *p, Vector2 coordinates, Vector2 speed, int rotationDuration);
void InitializeTriangle(TriangleObject *t, Vector2 coordinates);
void UpdateDynamicObject(DynamicObject *dnObj, Transform2D *transform, Rectangle *collider);
void UpdatePlayer(Player *p);
void DrawPlayer(Player p);
void DrawObjectOnCameraPosition(Texture2D texture, Vector2 position);
Vector2 GetGravityForce(GravityForce g);
void UpdateMainCamera(Camera2D *c);
void UpdatePosition(Vector2 *position, Rectangle *collider, Vector2 velocity);
void SetPosition(Vector2 *position, Rectangle *collider, Vector2 newPosition);
bool CheckPlayerTrianglesCollision();
void UpdateTrianglesPosition(Vector2 cameraPosition);
void UpdateTrianglesState();
void InitializePlatform(SquareObject *s, Vector2 coordinates);
void UpdatePlatformsState();
void UpdatePlatformsPosition(Vector2 cameraPosition);
void CheckPlayerPlatformsCollision();
void UpdatePlayerCheker();
void UpdateParticleEmitter(ParticleEmitter *pE, Vector2 newPosition);
void UpdateParticle(Particle *p, Vector2 gravityForce);
void InitializeParticleEmitter(ParticleEmitter *pE, Vector2 position, Vector2 offset, Vector2 direction, Vector2 minSpeed, Vector2 maxSpeed, float minRotation, 
float maxRotation, float minScale, float maxScale, Color aColor, Color bColor, int minDuration, int maxDuration, int spawnFrequency);
void InitPlayerParticle(Particle *p);
void SetParticleActive(Particle *p, bool active);
Vector2 GetRandomVector2(Vector2 a, Vector2 b);
float GetRandomFloat(float min, float max);

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
    
    // MAP LAODING
        // TODO: Read .bmp file propierly in order to get image width & height
    Color *mapPixels = malloc(GRID_WIDTH*GRID_HEIGHT * sizeof(Color));
    mapPixels = GetImageData(LoadImage("assets/gameplay_screen/maps/debug.bmp"));
    
    maxTriangles = 0;
    maxPlatforms = 0;
    
    for (int i=0; i<GRID_WIDTH*GRID_HEIGHT; i++)
    {
        /*
        printf("r: %i\n", mapPixels[i].r);
        printf("g: %i\n", mapPixels[i].g);
        printf("b: %i\n\n", mapPixels[i].b);
        */
        if (mapPixels[i].r == 255 && mapPixels[i].g == 0 && mapPixels[i].b == 0) maxTriangles++;
        else if (mapPixels[i].r == 0 && mapPixels[i].g == 255 && mapPixels[i].b == 0) maxPlatforms++;
    }
    
    triangles = malloc(maxTriangles * sizeof(TriangleObject));
    platforms = malloc(maxPlatforms * sizeof(SquareObject));
    
    int trianglesCounter=0;
    int platformsCounter=0;
    
    
    for (int y=0; y<GRID_HEIGHT; y++)
    {
        for (int x=0; x<GRID_WIDTH; x++)
        {
            if (mapPixels[y*GRID_WIDTH+x].r == 255 && mapPixels[y*GRID_WIDTH+x].g == 0 && mapPixels[y*GRID_WIDTH+x].b == 0) 
            {
                InitializeTriangle(&triangles[trianglesCounter], (Vector2){x, y});
                trianglesCounter++;
            }
            else if (mapPixels[y*GRID_WIDTH+x].r == 0 && mapPixels[y*GRID_WIDTH+x].g == 255 && mapPixels[y*GRID_WIDTH+x].b == 0) 
            {
                InitializePlatform(&platforms[platformsCounter], (Vector2){x, y});
                platformsCounter++;
            }
        }
    }
    
    free(mapPixels);
    
    //DEBUGGING && TESTING variables
    pause = FALSE;
    srand(time(NULL)); 
    
    // Textures loading
    /*
    player.texture = LoadTexture("assets/gameplay_screen/cube_main.png");
    triangleTexture = LoadTexture("assets/gameplay_screen/triangle_main.png");
    platformTexture = LoadTexture("assets/gameplay_screen/platform_main.png");
    player.pEmitter.texture = LoadTexture("assets/gameplay_screen/particle_main.png");
    */
    player.texture = LoadTexture("assets/gameplay_screen/debug.png");
    triangleTexture = LoadTexture("assets/gameplay_screen/debug.png");
    platformTexture = LoadTexture("assets/gameplay_screen/debug.png");
    player.pEmitter.texture = LoadTexture("assets/gameplay_screen/particle_main.png");
    
    // Camera initialization
    mainCamera = (Camera2D){Vector2Right(), (Vector2){4, 4}, Vector2Zero(), TRUE};
    
    // Gravity initialization
    gravity = (GravityForce){Vector2Up(), 0.6f};
    
    // Ground position and coordinate
    groundCoordinadeY = GetScreenHeight()/CELL_SIZE-1;
    groundPositionY = GetOnGridPosition((Vector2){0, groundCoordinadeY}).y;
    
    // Player initialization
    InitializePlayer(&player, (Vector2){4, groundCoordinadeY-1}, (Vector2){0, 10.5f}, 0.8f*GAME_SPEED);
    
    /*
    // Triangles initialization
    InitializeTriangle(&triangles[0], (Vector2){40, groundCoordinadeY-1});
    InitializeTriangle(&triangles[1], (Vector2){50, groundCoordinadeY-1});
    InitializeTriangle(&triangles[2], (Vector2){85, groundCoordinadeY-1});
    
    // Platforms initialization
    InitializePlatform(&platforms[0], (Vector2){20, groundCoordinadeY-1});
    InitializePlatform(&platforms[1], (Vector2){21, groundCoordinadeY-1});
    InitializePlatform(&platforms[2], (Vector2){22, groundCoordinadeY-1});
    InitializePlatform(&platforms[3], (Vector2){23, groundCoordinadeY-2});
    InitializePlatform(&platforms[4], (Vector2){24, groundCoordinadeY-2});
    */
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    if (IsKeyDown('P')) pause = !pause;
    
    if (!pause)
    {     
        // TODO: Update GAMEPLAY screen variables here!
        UpdateMainCamera(&mainCamera);
        
        UpdateTrianglesPosition(mainCamera.position);
        UpdateTrianglesState();
        UpdatePlatformsPosition(mainCamera.position);
        UpdatePlatformsState();
        
        UpdatePlayer(&player);
    }
    // Press enter to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER)||!player.isAlive)
    {
        finishScreen = 1;
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    
    // Background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    
    // Ground
    DrawRectangle(0, groundPositionY, GetScreenWidth(), 1, RED);
   
    DrawPlayer(player);
    
    // Draw triangles 
    for (int i=0; i<maxTriangles; i++)
    {
        if (triangles[i].isActive) DrawObjectOnCameraPosition(triangleTexture, triangles[i].position);
    }
    
    for (int i=0; i<maxPlatforms; i++)
    {
        if (platforms[i].isActive) DrawObjectOnCameraPosition(platformTexture, platforms[i].position);
        if (platforms[i].isActive) DrawRectangleRec(platforms[i].collider, RED);
    }
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    
    UnloadTexture(triangleTexture);
    UnloadTexture(player.texture);
    UnloadTexture(platformTexture);
    UnloadTexture(player.pEmitter.texture);
    free(player.pEmitter.particles);
    free(platforms);
    free(triangles);
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

void InitializePlayer(Player *p, Vector2 coordinates, Vector2 speed, int rotationDuration)
{
    p->transform = (Transform2D){GetOnGridPosition(coordinates), 0, ASSETS_SCALE};
    p->dnObj = (DynamicObject){(Rectangle){0, 0, 0, 0}, (Vector2){0, -1}, speed, Vector2Zero(), FALSE};
    p->collider = (Rectangle){p->transform.position.x, p->transform.position.y, p->texture.width*ASSETS_SCALE, p->texture.height*ASSETS_SCALE};
    p->rotationEasing = (Easing){0, 0, -180, rotationDuration, TRUE};
    p->color = WHITE;
    p->isAlive = TRUE;
    p->dnObj.checker = p->collider;
    
    InitializeParticleEmitter(&p->pEmitter, p->transform.position, (Vector2){0, p->texture.height*ASSETS_SCALE-5}, (Vector2){-1, -1}, 
    (Vector2){4, -0.4f}, (Vector2){6, 0.75f}, 0, 360, 0.25f, 2.5f, GREEN, (Color){255, 255, 255, 0}, 0.45f*GAME_SPEED, 0.65f*GAME_SPEED, 1);
    printf("%.3f\n", p->pEmitter.offset.y);
    for (int i=0; i<MAX_PARTICLES; i++)
    {
        InitPlayerParticle(&p->pEmitter.particles[i]);
        SetParticleActive(&p->pEmitter.particles[i], FALSE);
    }
}

void InitializeParticleEmitter(ParticleEmitter *pE, Vector2 position, Vector2 offset, Vector2 direction, Vector2 minSpeed, Vector2 maxSpeed, float minRotation, 
float maxRotation, float minScale, float maxScale, Color aColor, Color bColor, int minDuration, int maxDuration, int spawnFrequency)
{
    pE->position = Vector2Add(position, offset);
    pE->offset = offset;
    pE->source = (SourceParticle){position, direction, minSpeed, maxSpeed, minRotation, maxRotation, minScale*ASSETS_SCALE, maxScale*ASSETS_SCALE, aColor, bColor, minDuration, maxDuration};
    pE->gravity = (GravityForce){(Vector2){1, 0.1f}, 0.075f};
    pE->particles = malloc(MAX_PARTICLES * sizeof(Particle));
    pE->spawnFrequency = spawnFrequency;
    pE->framesCounter = 0;
}

void InitPlayerParticle(Particle *p)
{
    *p = (Particle){player.pEmitter.source.position, Vector2Zero(), GetRandomFloat(player.pEmitter.source.minRotation, player.pEmitter.source.maxRotation), 
    GetRandomFloat(player.pEmitter.source.minScale, player.pEmitter.source.maxScale), player.pEmitter.source.aColor, GetRandomFloat(player.pEmitter.source.minDuration, player.pEmitter.source.maxDuration), 0, TRUE};
    
    p->velocity = Vector2Product(GetRandomVector2(player.pEmitter.source.minSpeed, player.pEmitter.source.maxSpeed), player.pEmitter.source.direction);
}

void SetParticleActive(Particle *p, bool active)
{
    p->isActive = active;
}

Vector2 GetRandomVector2(Vector2 a, Vector2 b)
{
    return (Vector2){GetRandomFloat(a.x, b.x), GetRandomFloat(a.y, b.y)};
}

void InitializeTriangle(TriangleObject *t, Vector2 coordinates)
{
    t->sourcePosition = GetOnGridPosition(coordinates);
    t->position = t->sourcePosition;
    t->collidingPoints[0] = (Vector2){t->position.x, t->position.y+triangleTexture.height*ASSETS_SCALE};
    t->collidingPoints[1] = (Vector2){t->position.x+triangleTexture.width/2*ASSETS_SCALE, t->position.y};
    t->collidingPoints[2] = (Vector2){t->position.x+triangleTexture.width*ASSETS_SCALE, t->position.y+triangleTexture.height*ASSETS_SCALE};
    t->collidingPoints[3] = (Vector2){t->position.x+triangleTexture.width/2*ASSETS_SCALE, t->position.y+triangleTexture.height/2*ASSETS_SCALE};
    t->isActive = FALSE;
    t->isOver = FALSE;
}

void InitializePlatform(SquareObject *s, Vector2 coordinates)
{
        s->sourcePosition = GetOnGridPosition(coordinates);
        s->position = s->sourcePosition;
        s->collider = (Rectangle){s->position.x, s->position.y, platformTexture.width*ASSETS_SCALE, platformTexture.height*ASSETS_SCALE};
        s->isActive = FALSE;
        s->isOver = FALSE;
}

void DrawObjectOnCameraPosition(Texture2D texture, Vector2 position)
{
    DrawTextureEx(texture, position, 0, ASSETS_SCALE, WHITE);
}

void DrawPlayer(Player p)
{
    for (int i=0; i<MAX_PARTICLES; i++)
    {
        if (p.pEmitter.particles[i].isActive) DrawTextureEx(p.pEmitter.texture, p.pEmitter.particles[i].position, p.pEmitter.particles[i].rotation, 
        p.pEmitter.particles[i].scale, p.pEmitter.particles[i].color);
    }
    DrawTexturePro(p.texture, (Rectangle){0, 0, p.texture.width, p.texture.height}, (Rectangle){p.transform.position.x+p.texture.width/2*ASSETS_SCALE, 
    p.transform.position.y+p.texture.height/2*ASSETS_SCALE, p.texture.width*ASSETS_SCALE, p.texture.height*ASSETS_SCALE}, (Vector2){p.texture.width/2*ASSETS_SCALE, 
    p.texture.height/2*ASSETS_SCALE}, p.transform.rotation, p.color);
}

void SetPlayerAsGrounded(Vector2 newPosition)
{
    player.dnObj.isGrounded = TRUE;
    //dnObj->onAirCounter = 0;
    if (player.dnObj.velocity.y>0) player.dnObj.velocity.y = 0; // If player is moving down, set velocity at 0
    newPosition.y-=player.collider.height; // Draw player upside the ground
    SetPosition(&player.transform.position, &player.collider, newPosition);
}

void UpdateDynamicObject(DynamicObject *dnObj, Transform2D *transform, Rectangle *collider)
{  
    dnObj->isGrounded = FALSE;

    UpdatePosition(&transform->position, collider, dnObj->velocity);
        
    // If dnObj reaches the ground
    if (collider->y+collider->height>=groundPositionY)
    {
        SetPlayerAsGrounded((Vector2){transform->position.x, groundPositionY});
    }

    // Gravity
    if (!dnObj->isGrounded)
    {
        dnObj->velocity = Vector2Add(dnObj->velocity, GetGravityForce(gravity)); // Add gravity force
        //dnObj->onAirCounter++; // Gravity acceleration
    }
}

void UpdatePlayer(Player *p)
{   
    if (p->dnObj.isGrounded && IsKeyDown(KEY_SPACE))
    {
        p->dnObj.isGrounded = FALSE;
        p->dnObj.velocity.y = p->dnObj.speed.y*p->dnObj.direction.y;
        StartEasing(&p->rotationEasing);
    }
    
    UpdateDynamicObject(&p->dnObj, &p->transform, &p->collider);
  
    if (CheckPlayerTrianglesCollision()) player.isAlive = FALSE;
    CheckPlayerPlatformsCollision();
    UpdatePlayerCheker();
    
    if (p->dnObj.isGrounded) FinishEasing(&p->rotationEasing);
    UpdateRotationEasing(&p->rotationEasing, &p->transform.rotation);
    
    UpdateParticleEmitter(&p->pEmitter, p->transform.position);
}

void UpdatePosition(Vector2 *position, Rectangle *collider, Vector2 velocity)
{
    *position = Vector2Add(*position, velocity);
    collider->x = position->x;
    collider->y = position->y;
}

void SetPosition(Vector2 *position, Rectangle *collider, Vector2 newPosition)
{
    *position = newPosition;
    collider->x = position->x;
    collider->y = position->y;
}

Vector2 GetGravityForce(GravityForce g)
{
    return Vector2FloatProduct(g.direction, g.value);
}

void UpdateMainCamera(Camera2D *c)
{
    if (c->isMoving) c->position = Vector2Add(c->position, Vector2Product(c->direction, c->speed));
}

void UpdateRotationEasing(Easing *easing, float *value)
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
            easing->isFinished = TRUE;
        }
    }
}

void UpdateTrianglesPosition(Vector2 cameraPosition)
{
    for (int i=0; i<maxTriangles; i++)
    {
        if (!triangles[i].isOver) // If triangle has not been used. 
        {
            triangles[i].position = Vector2Sub(triangles[i].sourcePosition, cameraPosition);
            triangles[i].collidingPoints[0] = (Vector2){triangles[i].position.x, triangles[i].position.y+triangleTexture.height*ASSETS_SCALE};
            triangles[i].collidingPoints[1] = (Vector2){triangles[i].position.x+triangleTexture.width/2*ASSETS_SCALE, triangles[i].position.y};
            triangles[i].collidingPoints[2] = (Vector2){triangles[i].position.x+triangleTexture.width*ASSETS_SCALE, triangles[i].position.y+triangleTexture.height*ASSETS_SCALE};
            triangles[i].collidingPoints[3] = (Vector2){triangles[i].position.x+triangleTexture.width/2*ASSETS_SCALE, triangles[i].position.y+triangleTexture.height/2*ASSETS_SCALE};
        }
    }
}

void UpdatePlatformsPosition(Vector2 cameraPosition)
{
    for (int i=0; i<maxPlatforms; i++)
    {
        if (!platforms[i].isOver)
        {
            platforms[i].position = Vector2Sub(platforms[i].sourcePosition, cameraPosition);
            platforms[i].collider.x = platforms[i].position.x;
            platforms[i].collider.y = platforms[i].position.y;
        }
    }
}

bool CheckPlayerTrianglesCollision()
{
    for (int i=0; i<maxTriangles; i++)
    {
        if (triangles[i].isActive) // If triangle is in the screen
        {
           for (int j=0; j<MAX_TRIANGLE_COLLIDING_POINTS; j++)
            {
                if (CheckCollisionPointRec(triangles[i].collidingPoints[j], player.collider)) 
                {
                    return TRUE;
                }
            } 
        }
    }
    return FALSE;
}

void CheckPlayerPlatformsCollision()
{
    for (int i=0; i<maxPlatforms; i++)
    {
        if (platforms[i].isActive)
        {
            if (CheckCollisionRecs(player.collider, platforms[i].collider))
            {
                if (player.dnObj.checker.y+player.dnObj.checker.height<=platforms[i].position.y) SetPlayerAsGrounded((Vector2){player.transform.position.x, platforms[i].position.y});
                else player.isAlive = FALSE;
            }
        }
    }
}

void UpdateTrianglesState()
{
    for (int i=0; i<maxTriangles; i++)
    {
        if (!triangles[i].isOver)
        {
            if (triangles[i].position.x<0-triangleTexture.width) 
            {
                triangles[i].isOver = TRUE;   
                triangles[i].isActive = FALSE;
            }
            else if (triangles[i].position.x>GetScreenWidth()) triangles[i].isActive = FALSE;
            else triangles[i].isActive = TRUE;
        }
    }
}

void UpdatePlatformsState()
{
    for (int i=0; i<maxPlatforms; i++)
    {
        if (!platforms[i].isOver)
        {
            if (platforms[i].position.x<0-platformTexture.width*ASSETS_SCALE) 
            {
                platforms[i].isOver = TRUE;   
                platforms[i].isActive = FALSE;
            }
            else if (platforms[i].position.x>GetScreenWidth()) platforms[i].isActive = FALSE;
            else platforms[i].isActive = TRUE;
        }
    }
}

void UpdatePlayerCheker()
{
    player.dnObj.checker = player.collider;
}

void StartEasing(Easing *easing)
{
    easing->isFinished = false;
    if (easing->b<=-360) easing->b = 0;
}

void FinishEasing(Easing *easing)
{
    if (!easing->isFinished) easing->t = easing->d;
}

void UpdateParticleEmitter(ParticleEmitter *pE, Vector2 newPosition)
{
   pE->position = Vector2Add(newPosition, pE->offset);
   pE->source.position = pE->position;
   
   if (pE->framesCounter>=pE->spawnFrequency)
   {
       for (int i=0; i<MAX_PARTICLES; i++)
       {
           if (!pE->particles[i].isActive) 
           {
               SetParticleActive(&pE->particles[i], TRUE);
               InitPlayerParticle(&pE->particles[i]);
               i = MAX_PARTICLES;
               pE->framesCounter = 0;
           }
       }
   }
   pE->framesCounter++;
   
   for (int i=0; i<MAX_PARTICLES; i++)
   {
       if (pE->particles[i].isActive)
       {    
            UpdateParticle(&pE->particles[i], GetGravityForce(pE->gravity));
       }
   }
}

void UpdateParticle(Particle *p, Vector2 gravityForce)
{
    if (p->framesCounter<=p->duration)
    {
        p->position = Vector2Add(p->position, p->velocity);
        p->velocity = Vector2Add(p->velocity, gravityForce);
        p->framesCounter++;
    }
    else
    {
        SetParticleActive(p, FALSE);
    }
}

float GetRandomFloat(float min, float max)
{
    return (max-min) * ((float)rand() / (float) RAND_MAX) + min;
}





















