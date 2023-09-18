#pragma warning(disable: 4099 4098)

#include "raylib.h"

#include <iostream>
#include <cmath>

using namespace std;

struct Vector {
    Vector3 start;
    Vector3 end;
};

const int screenWidth = 1280;
const int screenHeight = 720;

const double m_pi = 3.14159265358979323846;

Vector v1;
Vector v2;
Vector v3;

static int amountSteps;

static float totalPerimeter;
static float totalArea;
static float totalVolume;

Camera3D camera;

float calculateTriangleArea(float a, float b, float c);
float calculateStepHeight(float hypotenuse);
void calculatePyramid();

Vector3 getMidPoint(Vector3 point1, Vector3 point2, Vector3 point3);
float getDistance3D(Vector3 point1, Vector3 point2);
Vector3 getPointAtSpecificVectorLength(Vector vector, float length);
void cropVectorEnd(Vector& vectorToCrop, float newLength);

void drawStatus();
void setSteps(int value);
void createThirdVector();
void createSecondVector();
void createFirstVector();
void initVectors();

void drawControls();
void handleControls();

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

float calculateTriangleArea(float a, float b, float c) {
    /* Area of a triangle using heron's formula
       More info: https://en.wikipedia.org/wiki/Heron%27s_formula
    */
    float s = ((a + b + c) / 2);
    return sqrt(s * ((s - a) * (s - b) * (s - c)));
}

float calculateStepHeight(float hypotenuse) {
    double angleRad = m_pi / 4.0; // Our angles are always 90/45/45

    return static_cast<float>(hypotenuse * sin(angleRad));
}

void calculatePyramid() {
    cout << "\n----- CALCULATING NEW PYRAMID -----\n\n";
    totalPerimeter = 0;
    totalArea = 0;
    totalVolume = 0;

    float prismLength = 0; // Height of the step, it's called prism because we're mathematically calculating prisms
    prismLength = calculateStepHeight(getDistance3D(v3.start, v3.end)); // All the prisms will have the same length

    for (int i = 1; i < amountSteps; i++) {
        cout << "STEP " << i << ":\n";
        float currentLength = (getDistance3D(v3.start, v3.end) * i);
        Vector3 p1 = getPointAtSpecificVectorLength(v1, currentLength);
        Vector3 p2 = getPointAtSpecificVectorLength(v2, currentLength);
        Vector3 p3 = getPointAtSpecificVectorLength(v3, currentLength);
        float a = 0;
        float b = 0;
        float c = 0;

        // -- PERIMETER --
        float stepPerimeter = 0;

        // To calculate the perimeter it's simple
        a = getDistance3D(p1, p2); // First we get each side of one of the two triangles in the prism (Assuming both are equal here)
        b = getDistance3D(p1, p3);
        c = getDistance3D(p2, p3);
        stepPerimeter += (a + b + c) * 2; // The sum of all sides of the triangle 2 times
        stepPerimeter += (prismLength * 3); // The length 3 times

        cout << "The Perimeter of the step is: " << stepPerimeter << "\n";

        // -- VOLUME --
        float stepVolume = 0;
        float tArea = 0; // Area of the triangle

        // First we find the are a of the triangle
        tArea = calculateTriangleArea(a, b, c);
        
        // Now to calculate the volume of the step 
        stepVolume = static_cast<float>(0.5 * tArea * prismLength);

        cout << "The Volume of the step is: " << stepVolume << "\n";

        // -- AREA --
        float stepArea = 0;
        float areaA = 0;
        float areaB = 0;
        float areaC = 0;
        float areaCurrentTriangle = 0;

        // Each step has 1 triangle and 3 rectangles
        // First we calculate the area of each rectangle
        areaA = a * prismLength; // To calculate the area we just do lenght * height
        areaB = b * prismLength;
        areaC = c * prismLength;

        //For the area of the triangle it depends on the step
        if (i > 1) {
            // If we're at a second step we need to substract the area of the triangle from the previous step, since our current triangle is void in the middle
            float previousLength = (getDistance3D(v3.start, v3.end) * (i - 1));
            Vector3 previousP1 = getPointAtSpecificVectorLength(v1, previousLength);
            Vector3 previousP2 = getPointAtSpecificVectorLength(v2, previousLength);
            Vector3 previousP3 = getPointAtSpecificVectorLength(v3, previousLength);
            float previousA = getDistance3D(previousP1, previousP2);
            float previousB = getDistance3D(previousP1, previousP3);
            float previousC = getDistance3D(previousP2, previousP3);
            float areaPreviousTriangle = calculateTriangleArea(previousA, previousB, previousC);
            // Now we calculate the area of the current triangle by substracting it with the previous
            areaCurrentTriangle = (tArea - areaPreviousTriangle);
        }
        else {
            // If we're at the first step we just use the area we already calculated for the volume
            areaCurrentTriangle = tArea;
        }

        stepArea = (areaA + areaB + areaC + areaCurrentTriangle); // The sum of all 4 areas is the total

        cout << "The area of the step is: " << stepArea << "\n\n";

        // --

        totalPerimeter += stepPerimeter;
        totalArea += stepArea;
        totalVolume += stepVolume;
    }
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

void drawStatus() {
    DrawText(TextFormat("Cantidad de escalones: %i", (amountSteps - 1)), 10, static_cast<int>(GetScreenHeight() * .8), 20, ORANGE);
    DrawText(TextFormat("Perimetro total: %f", totalPerimeter), 10, static_cast<int>(GetScreenHeight() * .85), 20, ORANGE);
    DrawText(TextFormat("Superficie total: %f", totalArea), 10, static_cast<int>(GetScreenHeight() * .9), 20, ORANGE);
    DrawText(TextFormat("Volumen total: %f", totalVolume), 10, static_cast<int>(GetScreenHeight() * .95), 20, ORANGE);
}

void setSteps(int value) {
    if (value < 2) return;
    float multiplier = 1.0f / static_cast<float>(value);
    cropVectorEnd(v3, static_cast<float>(getDistance3D(v1.start, v1.end) * multiplier));
    amountSteps = value;
    calculatePyramid();
}

void createThirdVector() {
    float multiplier = 1.0f / static_cast<float>(amountSteps);

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
    cout << "\n";
}

void drawControls() {
    DrawText("[Q] Para quitar escalones.", 10, 10, 20, RAYWHITE);
    DrawText("[E] Para agregar escalones.", 10, 40, 20, RAYWHITE);
    DrawText("[R] Para generar una nueva piramide aleatoria.", 10, 70, 20, RAYWHITE);
    DrawText("[ESC] para cerrar el programa.", 10, 100, 20, RAYWHITE);
}

void handleControls() {
    if (IsKeyPressed(KEY_R)) {
        initVectors();
        calculatePyramid();
    }
    if (IsKeyPressed(KEY_E)) {
        setSteps(amountSteps + 1);
    }
    if (IsKeyPressed(KEY_Q)) {
        setSteps(amountSteps - 1);
    }
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

        drawStatus();
        drawControls();

    EndDrawing();
}

void update() {
    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_ORBITAL);

        handleControls();

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

    totalPerimeter = 0;

    initVectors();
    calculatePyramid();
}

void uninit() {
    CloseWindow();
}