#include "raylib.h"
#include "chipmunk/chipmunk.h"
#include <math.h>
#include <stdlib.h>

#define SQUARE_SIZE 32
#define BOB_AMPLITUDE 0
#define SPAWN_INTERVAL 0.002 // Spawn a new square every 2 seconds
#define DUPLICATE_OFFSET 128

typedef struct {
    int x;
    int y;
} CustomPoint;

typedef struct {
    cpBody* body;
    float size;
} BlackSquare;

void applySineWaveMotion(CustomPoint* points, int numPoints, int time) {
    for (int i = 0; i < numPoints; i++) {
        points[i].y += (int)(BOB_AMPLITUDE * sin(0.1 * time));
    }
}

// Function to draw the terrain
void drawTerrain(CustomPoint* points, int numPoints) {
    // Draw triangles
    for (int i = 0; i < numPoints - 2; i += 2) {
        Vector2 v1 = { (float)points[i].x, (float)points[i].y };
        Vector2 v2 = { (float)points[i + 1].x, (float)points[i + 1].y };
        Vector2 v3 = { (float)points[i + 2].x, (float)points[i + 2].y };
        DrawTriangle(v1, v2, v3, ORANGE);

        // Draw the duplicate flipped triangle
        v1 = (Vector2){ (float)points[i + 1].x, (float)points[i + 1].y };
        v2 = (Vector2){ (float)points[i + 3].x, (float)points[i + 3].y };
        v3 = (Vector2){ (float)points[i + 2].x, (float)points[i + 2].y };
        DrawTriangle(v1, v2, v3, GREEN);

        // Create collision shape for concave polygon
        cpVect verts[4] = {
            cpv(v1.x, v1.y),
            cpv(v2.x, v2.y),
            cpv(points[i + 3].x, points[i + 3].y), // The duplicated point
            cpv(points[i + 2].x, points[i + 2].y)
        };
    }
}

int main() {
    const int screenWidth = 1800;
    const int screenHeight = 1000;

    InitWindow(screenWidth, screenHeight, "Falling Squares with Terrain");

    SetTargetFPS(60);

    CustomPoint* points = NULL;
    int numPoints = 0;

    // Initialize Chipmunk
    cpVect gravity = cpv(0, 1000);
    cpSpace *space = cpSpaceNew();
    cpSpaceSetGravity(space, gravity);

    cpBody *staticBody = cpSpaceGetStaticBody(space);

    BlackSquare* squares = NULL;
    int numSquares = 0;

    double lastSquareSpawnTime = GetTime(); // Initialize lastSquareSpawnTime

    while (!WindowShouldClose()) {
        double currentTime = GetTime();

        // Check if it's time to spawn a new square
        if (currentTime - lastSquareSpawnTime >= SPAWN_INTERVAL) {
            // Spawn a square at the top of the screen
            int spawnX = GetRandomValue(0, screenWidth - SQUARE_SIZE);
            int spawnY = -SQUARE_SIZE;

            // Create a square
            cpFloat mass = 1.0;
            cpFloat size = SQUARE_SIZE;
            cpFloat moment = cpMomentForBox(mass, size, size);
            cpBody* squareBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
            cpBodySetPosition(squareBody, cpv(spawnX, spawnY));
            cpShape* squareShape = cpBoxShapeNew(squareBody, size, size, 0);
            cpShapeSetFriction(squareShape, 0.7);
            cpShapeSetElasticity(squareShape, 0.2);
            cpSpaceAddShape(space, squareShape);

            // Store the square in the array
            numSquares++;
            squares = (BlackSquare*)realloc(squares, numSquares * sizeof(BlackSquare));
            squares[numSquares - 1].body = squareBody;
            squares[numSquares - 1].size = size;

            // Update the last spawn time
            lastSquareSpawnTime = currentTime;
        }

        // Update
        applySineWaveMotion(points, numPoints, GetTime());

        // Draw
        BeginDrawing();
        ClearBackground(SKYBLUE);

        // Draw terrain
        // drawTerrain(points, numPoints);
        // Draw triangles
        for (int i = 0; i < numPoints - 2; i += 2) {
            Vector2 v1 = { (float)points[i].x, (float)points[i].y };
            Vector2 v2 = { (float)points[i + 1].x, (float)points[i + 1].y };
            Vector2 v3 = { (float)points[i + 2].x, (float)points[i + 2].y };
            DrawTriangle(v1, v2, v3, ORANGE);

            // Draw the duplicate flipped triangle
            v1 = (Vector2){ (float)points[i + 1].x, (float)points[i + 1].y };
            v2 = (Vector2){ (float)points[i + 3].x, (float)points[i + 3].y };
            v3 = (Vector2){ (float)points[i + 2].x, (float)points[i + 2].y };
            DrawTriangle(v1, v2, v3, GREEN);

            // Create collision shape for concave polygon
            cpVect verts[4] = {
                cpv(v1.x, v1.y),
                cpv(v2.x, v2.y),
                cpv(points[i + 3].x, points[i + 3].y), // The duplicated point
                cpv(points[i + 2].x, points[i + 2].y)
            };
        }

        // Create a new point when left-clicking
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int mouseX = GetMouseX();
            int mouseY = GetMouseY();

            numPoints++;
            points = (CustomPoint*)realloc(points, numPoints * sizeof(CustomPoint));
            points[numPoints - 1].x = mouseX;
            points[numPoints - 1].y = mouseY;

            numPoints++;
            points = (CustomPoint*)realloc(points, numPoints * sizeof(CustomPoint));
            points[numPoints - 1].x = mouseX;
            points[numPoints - 1].y = mouseY + DUPLICATE_OFFSET;

            // Create collision shape for concave polygon
            for (int i = 0; i < numPoints - 2; i += 2) {
                cpVect verts[4] = {
                    cpv(points[i].x, points[i].y),
                    cpv(points[i + 1].x, points[i + 1].y),
                    cpv(points[i + 3].x, points[i + 3].y), // The duplicated point
                    cpv(points[i + 2].x, points[i + 2].y)
                };
                cpShape *polyShape = cpPolyShapeNew(staticBody, 4, verts, cpTransformIdentity, 0);
                cpShapeSetFriction(polyShape, 0.7);
                cpShapeSetElasticity(polyShape, 0.2);
                cpSpaceAddShape(space, polyShape);
            }
        }

        // Draw points
        for (int i = 0; i < numPoints; i++) {
            DrawCircle(points[i].x, points[i].y, 10, RED);
        }

        // Draw squares
        for (int i = 0; i < numSquares; i++) {
            cpVect position = cpBodyGetPosition(squares[i].body);
            DrawRectangle(position.x - squares[i].size / 2, position.y - squares[i].size / 2, squares[i].size, squares[i].size, BLACK);
            // if (position.y - squares[i].size / 2 > 200){
                // cpBodyGetSpace( squares[i].body );
                // cpBodyFree( cpBodyGetSpace( squares[i].body ) );
            //     cpBodyDestroy( squares[i].body );
            //     cpBodyFree( squares[i].body );
            // }
        }

        // Update Chipmunk physics
        cpSpaceStep(space, 1.0/60);

        EndDrawing();
    }

    // Cleanup Chipmunk space
    cpSpaceFree(space);

    free(points);
    free(squares);

    CloseWindow();

    return 0;
}
