#include "raylib.h"

struct Vector {
    Vector3 vectorStart;
    Vector3 vectorEnd;
};

const int screenWidth = 800;
const int screenHeight = 450;

Vector v1;
Vector v2;
Vector v3;

Camera3D camera;

void initVectors();

void draw();
void update();
void init();
void uninit();

int main(void)
{
    init();

    update();

    uninit();
    return 0;
}

void initVectors() {

}

void draw() {
    BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);



        EndMode3D();
    EndDrawing();
}

void update() {
    while (!WindowShouldClose()) {


        draw();
    }
}

void init() {
    InitWindow(screenWidth, screenHeight, "Algebra_TP2");
    SetTargetFPS(60);

    camera.position = Vector3{ 0.0f, 0.0f, 20.0f };
    camera.target = Vector3{ 0.0f, 5.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void uninit() {
    CloseWindow();
}