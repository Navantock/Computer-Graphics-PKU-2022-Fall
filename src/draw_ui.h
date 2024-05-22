#include <glm/gtc/quaternion.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <string>

#include "camera.h"
#include "model.h"
#include "particle.h"
#include "light.h"

static void draw_ui(Camera& camera, Model& model, ParticleRing& particle_ring, LightSrc& light_src) {
    {
        ImGui::Begin("View Tools");
        ImGui::Text("Some View Options For the Assignment.");
        ImGui::Text("You can tap 'SPACE' anytime to recover the initial pose.");

        ImGui::Checkbox("Focus on origin(Uncheck to move the viewpoint)", &camera.focusOrigin);
        ImGui::SliderFloat("Set X", &camera.pos.x, -70.5f, 70.5f, "camera x = %.2f");
        ImGui::SliderFloat("Set Y", &camera.pos.y, -70.5f, 70.5f, "camera y = %.2f");
        ImGui::SliderFloat("Set Z", &camera.pos.z, -70.5f, 70.5f, "camera z = %.2f");
        if (ImGui::CollapsingHeader("Camera Transition")) {
            if (ImGui::Button("Save Initial Camera")) {
                startCamera.position = camera.pos;
                startCamera.front = camera.front;
                startCamera.fov = camera.fov;
            }
            ImGui::SameLine();
            if (ImGui::Button("Save Ending Camera")) {
                endCamera.position = camera.pos;
                endCamera.front = camera.front;
                endCamera.fov = camera.fov;
            }

            ImGui::Text("Current camera: Position: (%.2f, %.2f, %.2f), Front: (%.2f, %.2f, %.2f), fov: %.2f", camera.pos.x, camera.pos.y, camera.pos.z, camera.front.x, camera.front.y, camera.front.z, camera.fov);
            ImGui::Text("Current initial camera: Position: (%.2f, %.2f, %.2f), Front: (%.2f, %.2f, %.2f), fov: %.2f", startCamera.position.x, startCamera.position.y, startCamera.position.z, startCamera.front.x, startCamera.front.y, startCamera.front.z, startCamera.fov);
            ImGui::Text("Current ending camera: Position: (%.2f, %.2f, %.2f), Front: (%.2f, %.2f, %.2f), fov: %.2f", endCamera.position.x, endCamera.position.y, endCamera.position.z, endCamera.front.x, endCamera.front.y, endCamera.front.z, endCamera.fov);

            if (ImGui::Button("Run Camera Transition")) {
                isRunningTransition = true;
                startTransitionTime = glfwGetTime();
            }
        }
        if (ImGui::CollapsingHeader("Model Movement and Light Source Settings")) {
            ImGui::Text("Model movement");
            ImGui::Checkbox("Rotate Earth", &model.rotateAnime);
            ImGui::Text("Light source settings");
            ImGui::Text("Light source position");
            ImGui::SliderFloat("Set X", &light_src.lightPos.x, -40.0f, 40.0f, "x = %.2f");
            ImGui::SliderFloat("Set Y", &light_src.lightPos.y, -40.0f, 40.0f, "y = %.2f");
            ImGui::SliderFloat("Set Z", &light_src.lightPos.z, -40.0f, 40.0f, "z = %.2f");
            ImGui::Text("Light source color");
            ImGui::ColorEdit3("light color", reinterpret_cast<float*>(&light_src.lightColor));
        }
        if (ImGui::CollapsingHeader("Particle Setting")) {
            if (ImGui::CollapsingHeader("Particle Ring Setting")) {
                ImGui::Text("Ring Params");
                ImGui::InputFloat("Radius", &particle_ring.radius, 0.1f, 0.5f, "%.2f");
                ImGui::InputFloat("Degree of dispersion", &particle_ring.var, 0.1f, 0.2f, "%.2f");
                ImGui::InputFloat("Angular velocity (rad/s)", &particle_ring.angular_velocity, 0.1f, M_PI/12, "%.2f");
                ImGui::SliderFloat("Particle size", &particle_ring.particle_size, 0.05f, 0.5f, "%.2f");
                ImGui::Text("Particle color settings");
                ImGui::Checkbox("Transparent Mode", &particle_ring.enable_alpha);
                ImGui::Checkbox("Use Texture", &particle_ring.enableTexture);
                ImGui::Checkbox("Use Transition Color", &particle_ring.useColorTrans);
                ImGui::SliderFloat("R", &particle_ring.color_rgb.x, .0f, 1.0f, "R");
                ImGui::SliderFloat("G", &particle_ring.color_rgb.y, .0f, 1.0f, "G");
                ImGui::SliderFloat("B", &particle_ring.color_rgb.z, .0f, 1.0f, "B");
                ImGui::ColorEdit3("color 1", reinterpret_cast<float*>(&particle_ring.color_rgb));
            }
        }
        ImGui::End();
    }
}