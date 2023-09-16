#include "raylib.h"

#include <iostream>
#include <cmath>

using namespace std;

struct Vector {
    Vector3 start;
    Vector3 end;
};

const int screenWidth = 800;
const int screenHeight = 450;

Vector v1;
Vector v2;
Vector v3;

Camera3D camera;

float getDistance3D(Vector3 point1, Vector3 point2);
void cropVectorEnd(Vector& vectorToCrop, float newLength);

void createThirdVector();
void createSecondVector();
void createFirstVector();
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

float getDistance3D(Vector3 point1, Vector3 point2) {
    float dx = point2.x - point1.x;
    float dy = point2.y - point1.y;
    float dz = point2.z - point1.z;

    /*
    Pythagorean Theorem or Euclidean distance
    More info: https://en.wikipedia.org/wiki/Euclidean_distance
    */
    // Hypot refers to the hypotenuse of a triangle.
    return hypot(dx, dy, dz);
}

void cropVectorEnd(Vector& vectorToCrop, float newLength) {
    float currentLength = getDistance3D(vectorToCrop.start, vectorToCrop.end);

    if (currentLength > 0) {
        float scale = newLength / currentLength;

        /* We're rescaling a vector by it's scalar factor to change it's length while mantaining it's direction.
        More info: https://en.wikipedia.org/wiki/Scalar_multiplication
        */
        vectorToCrop.end.x = vectorToCrop.start.x + (vectorToCrop.end.x - vectorToCrop.start.x) * scale;
        vectorToCrop.end.y = vectorToCrop.start.y + (vectorToCrop.end.y - vectorToCrop.start.y) * scale;
        vectorToCrop.end.z = vectorToCrop.start.z + (vectorToCrop.end.z - vectorToCrop.start.z) * scale;
    }
}

void createThirdVector() {
    /* Creating a third vector at a 90 degree angle of the first two using cross product.
    More info: https://en.wikipedia.org/wiki/Cross_product
    */
    v3.end.x = ((v1.end.y * v2.end.z) - (v1.end.z * v2.end.y));
    v3.end.y = ((v1.end.z * v2.end.x) - (v1.end.x * v2.end.z));
    v3.end.z = ((v1.end.x * v2.end.y) - (v1.end.y * v2.end.x));

    cropVectorEnd(v3, static_cast<float>(getDistance3D(v1.start, v1.end) * .1));
}

void createSecondVector() {
    v2.start.x = 0;
    v2.start.y = 0;
    v2.start.z = 0;
    v2.end.x = -v1.end.y;
    v2.end.y = v1.end.x;
    v2.end.z = v1.end.z;

    cropVectorEnd(v2, getDistance3D(v1.start, v1.end));
}

void createFirstVector() {
    v1.start.x = 0;
    v1.start.y = 0;
    v1.start.z = 0;
    v1.end.x = static_cast<float>(GetRandomValue(1, 10));
    v1.end.y = static_cast<float>(GetRandomValue(1, 10));
    v1.end.z = static_cast<float>(GetRandomValue(-10, -1));
}

void initVectors() {
    createFirstVector();
    createSecondVector();
    createThirdVector();

    cout << "v1 length: " << getDistance3D(v1.start, v1.end) << "\n";
    cout << "v2 length: " << getDistance3D(v2.start, v2.end) << "\n";
    cout << "v3 length: " << getDistance3D(v3.start, v3.end) << "\n";
}

void draw() {
    BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);
            // Orientation Lines
            DrawLine3D({ 0, 0, 0 }, { 30, 0, 0 }, DARKGRAY);
            DrawLine3D({ 0, 0, 0 }, { 0, 30, 0 }, DARKGRAY);
            DrawLine3D({ 0, 0, 0 }, { 0, 0, 30 }, DARKGRAY);
            // --
            DrawLine3D(v1.start, v1.end, RED);
            DrawLine3D(v2.start, v2.end, GREEN);
            DrawLine3D(v3.start, v3.end, BLUE);

        EndMode3D();
    EndDrawing();
}

void update() {
    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_ORBITAL);

        draw();
    }
}

void init() {
    InitWindow(screenWidth, screenHeight, "Algebra_TP2");
    SetTargetFPS(60);

    camera.position = Vector3{ 0.0f, 5.0f, 20.0f };
    camera.target = Vector3{ 0.0f, 5.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    initVectors();
}

void uninit() {
    CloseWindow();
}