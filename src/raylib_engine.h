#ifndef __RAYLIB_ENGINE_H__
#define __RAYLIB_ENGINE_H__

#include "raylib-cpp.hpp"
#include "rlights.h"

#include <optional>
#include <vector>

namespace engine {

class Joystick {
public:
  static std::optional<Joystick> get_available();

  std::vector<float> get_axes() const;

private:
  Joystick(int jid) : m_jid(jid) {}
  int m_jid;
};

class RaylibDevice {
public:
  RaylibDevice(int screen_width, int screen_height);
  ~RaylibDevice();

  raylib::Camera3D get_camera() { return m_camera; }

  raylib::Model load_model(const std::string &file_name,
                           bool enable_lighting = true);
  Light &create_light(int type, raylib::Vector3 position,
                      raylib::Vector3 target, raylib::Color color);

  void add_skybox_from_single_image(std::string image_path);
  void add_skybox_from_6_images(std::string right, std::string left,
                                std::string top, std::string bottom,
                                std::string back, std::string front);

  void begin_frame();
  void end_frame();

private:
  void add_skybox_from_image(const raylib::Image &image);

  raylib::Camera3D m_camera;
  raylib::Shader m_lighting_shader;
  std::vector<Light> m_lights;
  std::optional<raylib::Model> m_skybox_model;
};

} // namespace engine

#endif // __RAYLIB_ENGINE_H__