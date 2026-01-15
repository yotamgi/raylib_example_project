#include "raylib_engine.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif

#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// RaylibDevice implementation
////////////////////////////////////////////////////////////////////////////////

RaylibDevice::RaylibDevice(int screen_width, int screen_height)
    : m_camera(raylib::Vector3(2, 4, 6),
               raylib::Vector3(0, 0.5, 0.0)) // Initialize a default camera.
{

  SetConfigFlags(FLAG_MSAA_4X_HINT); // Enable Multi Sampling Anti Aliasing 4x
                                     // (if available)
  InitWindow(screen_width, screen_height,
             "raylib [shaders] example - basic lighting");

  m_lighting_shader = raylib::Shader(
      TextFormat("resources/shaders/glsl%i/lighting.vs", GLSL_VERSION),
      TextFormat("resources/shaders/glsl%i/lighting.fs", GLSL_VERSION));

  m_camera.fovy = 45.0;
  m_camera.projection = CAMERA_PERSPECTIVE;

  // Get some required shader locations
  m_lighting_shader.locs[SHADER_LOC_VECTOR_VIEW] =
      m_lighting_shader.GetLocation("viewPos");

  // Ambient light level (some basic lighting)
  int ambientLoc = m_lighting_shader.GetLocation("ambient");
  std::array<float, 4> ambientValues = {0.1f, 0.1f, 0.1f, 1.0f};
  m_lighting_shader.SetValue(ambientLoc, ambientValues.data(),
                             SHADER_UNIFORM_VEC4);

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
}

Light &RaylibDevice::create_light(int type, raylib::Vector3 position,
                                  raylib::Vector3 target, raylib::Color color) {
  m_lights.push_back(
      CreateLight(type, position, target, color, m_lighting_shader));
  return m_lights.back();
}

raylib::Model RaylibDevice::load_model(const std::string &file_name,
                                       bool enable_lighting) {
  raylib::Model model(file_name);
  if (enable_lighting) {
    for (int i = 0; i < model.GetMaterialCount(); i++) {
      model.GetMaterials()[i].shader = m_lighting_shader;
    }
  }
  return model;
}

void RaylibDevice::begin_frame() {
  m_camera.Update(CAMERA_CUSTOM);

  // Update the shader with the camera view vector (points towards { 0.0f,
  // 0.0f, 0.0f })
  std::array<float, 3> cameraPos = {m_camera.position.x, m_camera.position.y,
                                    m_camera.position.z};
  m_lighting_shader.SetValue(m_lighting_shader.locs[SHADER_LOC_VECTOR_VIEW],
                             cameraPos.data(), SHADER_UNIFORM_VEC3);

  // Update light values (actually, only enable/disable them)
  for (const auto &light : m_lights) {
    UpdateLightValues(m_lighting_shader, light);
  }
  BeginDrawing();

  ClearBackground(RAYWHITE);

  BeginMode3D(m_camera);
  m_lighting_shader.BeginMode();
}

void RaylibDevice::end_frame() {
  m_lighting_shader.EndMode();
  EndMode3D();
  DrawFPS(10, 10);
  DrawText("Use keys [Y][R][G][B] to toggle lights", 10, 40, 20, DARKGRAY);
  EndDrawing();
}

RaylibDevice::~RaylibDevice() {
  m_lighting_shader.Unload(); // Unload shader
  CloseWindow();
}

////////////////////////////////////////////////////////////////////////////////
// Joystick control
////////////////////////////////////////////////////////////////////////////////

#if defined(PLATFORM_DESKTOP)
// Speial handling for platform desktop, since raylib doesn't seem to be able to
// recognize simple joysticks.

static int is_rc_control_heuristic(int jid) {

  // Num axes heuristic.
  int num_axes;
  glfwGetJoystickAxes(jid, &num_axes);
  if (num_axes < 4) {
    return false;
  }

  // If is considered a gamepad by GLFW, prefer it.
  if (glfwJoystickIsGamepad(jid)) {
    return 2;
  }

  // Name heuristic.
  std::string name(glfwGetJoystickName(jid));

  std::transform(name.begin(), name.end(), name.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  if (name.find("keyboard") != std::string::npos ||
      name.find("mouse") != std::string::npos ||
      name.find("touchpad") != std::string::npos ||
      name.find("elan") != std::string::npos) {
    return false;
  }

  return 1;
}

static std::vector<int> get_rc_controllers() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return {};
  }

  std::vector<int> result;
  for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++) {
    if (glfwJoystickPresent(jid)) {
      int rc_controller_priority = is_rc_control_heuristic(jid);
      if (rc_controller_priority == 2) {
        return {jid};
      } else if (rc_controller_priority == 1) {
        std::cout << "Found joystick " << jid << ": "
                  << glfwGetJoystickName(jid) << std::endl;
        result.push_back(jid);
      }
    }
  }
  return result;
}

std::optional<Joystick> Joystick::get_available() {
  auto jids = get_rc_controllers();
  if (jids.empty()) {
    return std::nullopt;
  }
  return Joystick{jids[0]};
}

std::vector<float> Joystick::get_axes() const {
  int count;
  const float *axes = glfwGetJoystickAxes(m_jid, &count);
  return std::vector<float>(axes, axes + count);
}

#else
// For other platforms, use raylib's joystick handling.

std::optional<Joystick> Joystick::get_available() {
  for (int jid = 0; jid < 4; jid++) {
    if (IsGamepadAvailable(jid)) {
      return Joystick{jid};
    }
  }
  return std::nullopt;
}

std::vector<float> Joystick::get_axes() const {
  std::vector<float> axes;
  int axis_count = GetGamepadAxisCount(m_jid);
  for (int i = 0; i < axis_count; i++) {
    axes.push_back(GetGamepadAxisMovement(m_jid, i));
  }
  return axes;
}

#endif