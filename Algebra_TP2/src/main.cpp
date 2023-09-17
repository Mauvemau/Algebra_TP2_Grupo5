#pragma warning(disable: 4099 4098)

#include "raylib.h"

#include <iostream>
#include <cmath>

using namespace std;

struct Vector {
    Vector3 start;
    Vector3 end;
};

const int screenWidth = 800;
const int screenHeight = 600;

Vector v1;
Vector v2;
Vector v3;

int amountSteps;

Camera3D camera;

Vector3 getMidPoint(Vector3 point1, Vector3 point2, Vector3 point3);
float getDistance3D(Vector3 point1, Vector3 point2);
Vector3 getPointAtSpecificVectorLength(Vector vector, float length);
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

Vector3 getMidPoint(Vector3 point1, Vector3 point2, Vector3 point3) {
    /*
    We can find the middle point of a triangle using the formula to calculate the centroid of a triangle
    More info: https://byjus.com/maths/centroid-of-a-triangle/
    */
    Vector3 midpoint;
    midpoint.x = (point1.x + point2.x + point3.x) / 3.0f;
    midpoint.y = (point1.y + point2.y + point3.y) / 3.0f;
    midpoint.z = (point1.z + point2.z + point3.z) / 3.0f;
    return midpoint;
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

Vector3 getPointAtSpecificVectorLength(Vector vector, float length) {
    float currentLength = getDistance3D(vector.start, vector.end);

    if (currentLength <= 0)
        return vector.start;

    float scale = length / currentLength;
    Vector3 result;

    /* Scalar factor of a vector, this way you can get a position at a specified length while mantaining it's direction/angle.
        More info: https://en.wikipedia.org/wiki/Scalar_multiplication
    */
    result.x = vector.start.x + (vector.end.x - vector.start.x) * scale;
    result.y = vector.start.y + (vector.end.y - vector.start.y) * scale;
    result.z = vector.start.z + (vector.end.z - vector.start.z) * scale;

    return result;
}

void cropVectorEnd(Vector& vectorToCrop, float newLength) {
    vectorToCrop.end = getPointAtSpecificVectorLength(vectorToCrop, newLength);
}

void createThirdVector() {
    float multiplier = 1.0f / static_cast<float>(amountSteps); // Completamente innecesario pero tenia ganas.

    /* Creating a third vector at a 90 degree angle of the first two using cross product.
    More info: https://en.wikipedia.org/wiki/Cross_product
    */
    v3.end.x = ((v1.end.y * v2.end.z) - (v1.end.z * v2.end.y));
    v3.end.y = ((v1.end.z * v2.end.x) - (v1.end.x * v2.end.z));
    v3.end.z = ((v1.end.x * v2.end.y) - (v1.end.y * v2.end.x));

    cropVectorEnd(v3, static_cast<float>(getDistance3D(v1.start, v1.end) * multiplier));
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

            // Drawing the randomly generated vectors
            DrawLine3D(v1.start, v1.end, RED);
            DrawLine3D(v2.start, v2.end, GREEN);
            DrawLine3D(v3.start, v3.end, BLUE);
            
            // A guide line that points towards the direction the pyramid was generated
            float dirLength = (getDistance3D(v3.start, v3.end) * amountSteps);
            Vector pDir = { {0, 0, 0},  getMidPoint(v1.end, v2.end, getPointAtSpecificVectorLength(v3, dirLength)) };
            DrawLine3D(pDir.start, pDir.end, RAYWHITE);

            for (int i = 0; i <= amountSteps; i++) {
                // We create 3 points at our current step height
                float multiplier = 1.0f / static_cast<float>(i);
                float currentLength = (getDistance3D(v3.start, v3.end) * i);
                Vector3 p1 = getPointAtSpecificVectorLength(v1, currentLength);
                Vector3 p2 = getPointAtSpecificVectorLength(v2, currentLength);
                Vector3 p3 = getPointAtSpecificVectorLength(v3, currentLength);

                // Drawing the bottom of the step
                DrawLine3D(p3, p1, PINK);
                DrawLine3D(p3, p2, PINK);
                DrawLine3D(p1, p2, PINK);

                if (i > 1) {
                    /*
                    We can calculate the position of the third point in each step of the pyramid using our current functions with this following logic:
                    Each current subdivision of the pyramid is a triangle, if we slice a smaller triangle with a multiplier divided by the current step
                    (Ej: if at step two out of ten then multiplier is 20% or x0.8)
                    And then we use the function to calculate the middle of this smaller triangle, we should get the position of the third point.
                    */
                    Vector3 stepPoint1;
                    Vector3 stepPoint2;
                    Vector3 stepPoint3;

                    Vector3 slicedPoint1;
                    Vector3 slicedPoint2;

                    // We slice two points in our triangle using the multiplier
                    slicedPoint1 = getPointAtSpecificVectorLength({ p3, p1 }, (getDistance3D(p3, p1) * multiplier));
                    slicedPoint2 = getPointAtSpecificVectorLength({ p3, p2 }, (getDistance3D(p3, p2) * multiplier));
                    // We get a middle point between our current selected point and the other two we sliced, giving us the third point we want.
                    stepPoint1 = getMidPoint(p3, slicedPoint1, slicedPoint2);

                    slicedPoint1 = getPointAtSpecificVectorLength({ p1, p2 }, (getDistance3D(p1, p2) * multiplier));
                    slicedPoint2 = getPointAtSpecificVectorLength({ p1, p3 }, (getDistance3D(p1, p3) * multiplier));
                    stepPoint2 = getMidPoint(p1, slicedPoint1, slicedPoint2);

                    slicedPoint1 = getPointAtSpecificVectorLength({ p2, p1 }, (getDistance3D(p2, p1) * multiplier));
                    slicedPoint2 = getPointAtSpecificVectorLength({ p2, p3 }, (getDistance3D(p2, p3) * multiplier));
                    stepPoint3 = getMidPoint(p2, slicedPoint1, slicedPoint2);

                    // Drawing time;

                    // Varibale holding the lenght of the previous step, which we're gonna use to position ourlselves at the top of the current step
                    float previousLength = (getDistance3D(v3.start, v3.end) * (i - 1));
                    // Line at the middle of the step 1
                    DrawLine3D(stepPoint1, stepPoint2, YELLOW);
                    // Face of the step 1
                    DrawTriangle3D(getPointAtSpecificVectorLength(v3, previousLength), getPointAtSpecificVectorLength(v1, previousLength), stepPoint2, RAYWHITE);
                    DrawTriangle3D(stepPoint1, getPointAtSpecificVectorLength(v3, previousLength), stepPoint2, RAYWHITE); // Hardcoded light by just giving each face a different tone of gray
                    // Line at the middle of the step 2
                    DrawLine3D(stepPoint1, stepPoint3, YELLOW);
                    // Face of the step 2
                    DrawTriangle3D(getPointAtSpecificVectorLength(v1, previousLength), getPointAtSpecificVectorLength(v2, previousLength), stepPoint2, LIGHTGRAY);
                    DrawTriangle3D(stepPoint2, getPointAtSpecificVectorLength(v2, previousLength), stepPoint3, LIGHTGRAY);
                    // Line at the middle of the step 3
                    DrawLine3D(stepPoint3, stepPoint2, YELLOW);
                    // Face of the step 3
                    DrawTriangle3D(getPointAtSpecificVectorLength(v2, previousLength), getPointAtSpecificVectorLength(v3, previousLength), stepPoint3, DARKGRAY);
                    DrawTriangle3D(stepPoint3, getPointAtSpecificVectorLength(v3, previousLength), stepPoint1, DARKGRAY);
                }

                if (i < amountSteps) // Floor of the step
                    DrawTriangle3D(p3, p2, p1, DARKGRAY);
            }

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
    InitWindow(screenWidth, screenHeight, "PyramidGen");
    SetTargetFPS(60);

    camera.position = Vector3{ 0.0f, 5.0f, 20.0f };
    camera.target = Vector3{ 0.0f, 5.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    amountSteps = 11;

    initVectors();
}

void uninit() {
    CloseWindow();
}