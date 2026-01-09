#include "raylib-cpp.hpp"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif

#include <array>


int main() {
    
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT);  // Enable Multi Sampling Anti Aliasing 4x (if available)
    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - basic lighting");

    raylib::Camera3D camera(
        raylib::Vector3(2, 4, 6),
        raylib::Vector3(0, 0.5, 0.)
    );
    camera.fovy = 45.;
    camera.projection = CAMERA_PERSPECTIVE;

    // Load basic lighting shader
    raylib::Shader shader (TextFormat("resources/shaders/glsl%i/lighting.vs", GLSL_VERSION),
                           TextFormat("resources/shaders/glsl%i/lighting.fs", GLSL_VERSION));
    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = shader.GetLocation("viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading,
    // no need to get the location again if using that uniform name
    //shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");

    // Ambient light level (some basic lighting)
    int ambientLoc = shader.GetLocation("ambient");
    std::array<float, 4> ambientValues = {0.1f, 0.1f, 0.1f, 1.0f};
    shader.SetValue(ambientLoc, ambientValues.data(), SHADER_UNIFORM_VEC4);

    // Create lights
    std::array<Light, MAX_LIGHTS> lights = {
        CreateLight(LIGHT_POINT, (Vector3) {-2, 1, -2}, Vector3Zero(), YELLOW, shader),
        CreateLight(LIGHT_POINT, (Vector3) {2, 1, 2}, Vector3Zero(), RED, shader),
        CreateLight(LIGHT_POINT, (Vector3) {-2, 1, 2}, Vector3Zero(), GREEN, shader),
        CreateLight(LIGHT_POINT, (Vector3) {2, 1, -2}, Vector3Zero(), BLUE, shader),
    };

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        camera.Update(CAMERA_ORBITAL);

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        std::array<float, 3> cameraPos = { camera.position.x, camera.position.y, camera.position.z };
        shader.SetValue(shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos.data(), SHADER_UNIFORM_VEC3);

        // Check key inputs to enable/disable lights
        if (IsKeyPressed(KEY_Y)) { lights[0].enabled = !lights[0].enabled; }
        if (IsKeyPressed(KEY_R)) { lights[1].enabled = !lights[1].enabled; }
        if (IsKeyPressed(KEY_G)) { lights[2].enabled = !lights[2].enabled; }
        if (IsKeyPressed(KEY_B)) { lights[3].enabled = !lights[3].enabled; }

        // Update light values (actually, only enable/disable them)
        for (const auto& light : lights) UpdateLightValues(shader, light);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        BeginShaderMode(shader);

        DrawPlane(Vector3Zero(), (Vector2) { 10.0, 10.0 }, WHITE);
        DrawCube(Vector3Zero(), 2.0, 4.0, 2.0, WHITE);

        EndShaderMode();

        // Draw spheres to show where the lights are
        for (const auto& light : lights)
        {
            if (light.enabled) DrawSphereEx(light.position, 0.2f, 8, 8, light.color);
            else DrawSphereWires(light.position, 0.2f, 8, 8, ColorAlpha(light.color, 0.3f));
        }

        DrawGrid(10, 1.0f);

        EndMode3D();

        DrawFPS(10, 10);

        DrawText("Use keys [Y][R][G][B] to toggle lights", 10, 40, 20, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    //UnloadShader(shader);   // Unload shader

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
