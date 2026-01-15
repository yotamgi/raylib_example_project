#include "raylib-cpp.hpp"

#include "raylib_engine.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <string>

int main() {

  auto joystick = Joystick::get_available();
  if (!joystick) {
    std::cout << "No RC controller found" << std::endl;
  }

  RaylibDevice device(1440, 900);

  // NOTE: "matModel" location name is automatically assigned on shader loading,
  // no need to get the location again if using that uniform name
  // shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader,
  // "matModel");

  // Create lights
  std::array<Light, MAX_LIGHTS> lights = {
      device.create_light(LIGHT_POINT, (Vector3){-2, 1, -2}, Vector3Zero(),
                          YELLOW),
      device.create_light(LIGHT_POINT, (Vector3){2, 1, 2}, Vector3Zero(), RED),
      device.create_light(LIGHT_POINT, (Vector3){-2, 1, 2}, Vector3Zero(),
                          GREEN),
      device.create_light(LIGHT_POINT, (Vector3){2, 1, -2}, Vector3Zero(),
                          BLUE),
  };

  //------------------------------------------------------------------------------
  // Load airplane models
  raylib::Model model =
      device.load_model("resources/media/cessna/CessnaBodyFixed2.obj");

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Check key inputs to enable/disable lights
    if (IsKeyPressed(KEY_Y)) {
      lights[0].enabled = !lights[0].enabled;
    }
    if (IsKeyPressed(KEY_R)) {
      lights[1].enabled = !lights[1].enabled;
    }
    if (IsKeyPressed(KEY_G)) {
      lights[2].enabled = !lights[2].enabled;
    }
    if (IsKeyPressed(KEY_B)) {
      lights[3].enabled = !lights[3].enabled;
    }

    device.begin_frame();
    DrawPlane(Vector3Zero(), (Vector2){10.0, 10.0}, WHITE);
    model.Draw(Vector3{0.0f, 1.0f, 0.0f}, 1.0f, WHITE);

    // Draw spheres to show where the lights are
    for (const auto &light : lights) {
      if (light.enabled)
        DrawSphereEx(light.position, 0.2f, 8, 8, light.color);
      else
        DrawSphereWires(light.position, 0.2f, 8, 8,
                        ColorAlpha(light.color, 0.3f));
    }

    DrawGrid(10, 1.0f);

    device.end_frame();
  }

  return 0;
}
