#include "gl_env.h"
#include "gestures.h"
#include <glm/gtc/quaternion.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <unordered_map>
#include <string>

class GrooveGame {
public:
    static bool game_final_subtraction(int& curr_N) {
        int choice;
        if (curr_N > 3) {
            choice = (rand() % (3)) + 1;
        }
        else if (0 < curr_N <= 3) {
            choice = curr_N;
        }
        curr_N -= choice;
        switch (choice)
        {
        case 1:
            showNum_1();
            break;
        case 2:
            showNum_2();
            break;
        case 3:
            showNum_3();
            break;
        default:
            break;
        }
        if (curr_N == 0) {
            return true;
        }
        else {
            return false;
        }
    };
    static void game_sum();

};

typedef struct AnimeCameraState {
    glm::vec3 position;
    glm::vec3 front;
    float fov;
};

/*Window vars*/
int windowWidth = 800;
int windowHeight = 800;

/*Camera vars*/
bool focusOrigin = false;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 45.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 view;
glm::mat4 proj;
float fov = 45.0f;
double cameraSpeed = 0.05f;
double lastX = 0.0;
double lastY = 0.0;
double pitch = 0.0;
double yaw = 0.0;

/*Camera Transition vars*/
bool isRunningTransition = false;
AnimeCameraState startCamera = {cameraPos, cameraFront, fov};
AnimeCameraState endCamera = {cameraPos, cameraFront, fov};
float startTransitionTime;
float transitionTimeLength = 2.5f;

/*Model Anime vars*/
int animeChoose = 0;
float startAnimeTime = 0.0f;
float passed_time;

/*Game1 vars*/
bool is_game1_open = false;
bool has_clicked = false;
int N_for_game = (rand() % (66 - 10 + 1)) + 10;
std::string tips_text = "";
GrooveGame Groove;

std::unordered_map<int, bool> keyStates;
std::unordered_map<int, float> keyActionTime { {GLFW_KEY_Q, 0.0f} };
std::unordered_map<int, float> keyStartPressTime;

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
    case GLFW_KEY_SPACE:
        animeChoose = 0;
        fov = 45.0f;
        cameraPos = glm::vec3(0.0f, 0.0f, 45.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        restoreState();
        break;
    case GLFW_KEY_Q:
        if (action == GLFW_PRESS) {
            if (!keyStates[GLFW_KEY_Q])
                keyStartPressTime[GLFW_KEY_Q] = glfwGetTime();
            keyStates[GLFW_KEY_Q] = true;
        }
        else if (action == GLFW_RELEASE) {
            if (keyStates[GLFW_KEY_Q])
                keyActionTime[GLFW_KEY_Q] += glfwGetTime() - keyStartPressTime[GLFW_KEY_Q];
            keyStates[GLFW_KEY_Q] = false;
        }
        break;
    case GLFW_KEY_W:
        if (action == GLFW_PRESS) {
            if (!keyStates[GLFW_KEY_W])
                keyStartPressTime[GLFW_KEY_W] = glfwGetTime();
            keyStates[GLFW_KEY_W] = true;
        }
        else if (action == GLFW_RELEASE) {
            if (keyStates[GLFW_KEY_W])
                keyActionTime[GLFW_KEY_W] += glfwGetTime() - keyStartPressTime[GLFW_KEY_W];
            keyStates[GLFW_KEY_W] = false;
        }
        break;
    case GLFW_KEY_E:
        if (action == GLFW_PRESS) {
            if (!keyStates[GLFW_KEY_E])
                keyStartPressTime[GLFW_KEY_E] = glfwGetTime();
            keyStates[GLFW_KEY_E] = true;
        }
        else if (action == GLFW_RELEASE) {
            if (keyStates[GLFW_KEY_E])
                keyActionTime[GLFW_KEY_E] += glfwGetTime() - keyStartPressTime[GLFW_KEY_E];
            keyStates[GLFW_KEY_E] = false;
        }
        break;
    case GLFW_KEY_R:
        if (action == GLFW_PRESS) {
            if (!keyStates[GLFW_KEY_R])
                keyStartPressTime[GLFW_KEY_R] = glfwGetTime();
            keyStates[GLFW_KEY_R] = true;
        }
        else if (action == GLFW_RELEASE) {
            if (keyStates[GLFW_KEY_R])
                keyActionTime[GLFW_KEY_R] += glfwGetTime() - keyStartPressTime[GLFW_KEY_R];
            keyStates[GLFW_KEY_R] = false;
        }
        break;
    case GLFW_KEY_T:
        if (action == GLFW_PRESS) {
            if (!keyStates[GLFW_KEY_T])
                keyStartPressTime[GLFW_KEY_T] = glfwGetTime();
            keyStates[GLFW_KEY_T] = true;
        }
        else if (action == GLFW_RELEASE) {
            if (keyStates[GLFW_KEY_T])
                keyActionTime[GLFW_KEY_T] += glfwGetTime() - keyStartPressTime[GLFW_KEY_T];
            keyStates[GLFW_KEY_T] = false;
        }
        break;
    default:
        break;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    float sensitivity = 0.05f;
    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    lastX = xpos;
    lastY = ypos;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = -glm::normalize(front);
}


static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

static void processKeyCommand() {
    if (keyStates[GLFW_KEY_Q]) {
        thumbAnime(keyActionTime[GLFW_KEY_Q] + glfwGetTime() - keyStartPressTime[GLFW_KEY_Q]);
    }
    if (keyStates[GLFW_KEY_W]) {
        indexAnime(keyActionTime[GLFW_KEY_W] + glfwGetTime() - keyStartPressTime[GLFW_KEY_W]);
    }
    if (keyStates[GLFW_KEY_E]) {
        middleAnime(keyActionTime[GLFW_KEY_E] + glfwGetTime() - keyStartPressTime[GLFW_KEY_E]);
    }
    if (keyStates[GLFW_KEY_R]) {
        ringAnime(keyActionTime[GLFW_KEY_R] + glfwGetTime() - keyStartPressTime[GLFW_KEY_R]);
    }
    if (keyStates[GLFW_KEY_T]) {
        pinkyAnime(keyActionTime[GLFW_KEY_T] + glfwGetTime() - keyStartPressTime[GLFW_KEY_T]);
    }
}

static void processMouseCommand(GLFWwindow* window) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwGetCursorPos(window, &lastX, &lastY);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, {});
    }

}


static void playAnimation() {
    switch (animeChoose) {
    case 0:
        break;
    case 1:
        flickAnime(glfwGetTime() - startAnimeTime);
        break;
    case 2:
        fistAnime(glfwGetTime() - startAnimeTime);
        break;
    case 3:
        shakeHandAnime(glfwGetTime() - startAnimeTime);
        break;
    default:
        break;
    }
}

static void playCameraTransition() {
    float delta_t = glfwGetTime() - startTransitionTime;
    if (delta_t > transitionTimeLength) {
        isRunningTransition = false;
        return;
    }
    float delta_fov = endCamera.fov - startCamera.fov;
    glm::vec3 dis_vec = endCamera.position - startCamera.position;

    if (startCamera.front != endCamera.front) {
        glm::vec3 rotationAxis = glm::normalize(glm::cross(startCamera.front, endCamera.front));
        float cosTheta = glm::dot(startCamera.front, endCamera.front);
        glm::qua<float> q0 = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::qua<float> q1 = glm::quat(sqrt((1 + cosTheta) / 2), rotationAxis.x * sqrt((1 - cosTheta) / 2), rotationAxis.y * sqrt((1 - cosTheta) / 2), rotationAxis.z * sqrt((1 - cosTheta) / 2));
        glm::qua<float> interpolatedqua = glm::mix(q0, q1, delta_t / transitionTimeLength);
        cameraFront = interpolatedqua * startCamera.front;
    }

    fov = startCamera.fov + (delta_t / transitionTimeLength) * delta_fov;
    cameraPos = startCamera.position + (delta_t / transitionTimeLength) * dis_vec;
}

static void draw_ui() {
    static bool game1 = false;
    {
        ImGui::Begin("View Tools");
        ImGui::Text("Some View Options For the Assignment.");
        ImGui::Text("You can tap 'SPACE' anytime to recover the initial pose.");
        ImGui::Text("Hello! I'm a hand and my name is Groove. Nice to meet you!");

        ImGui::Checkbox("Focus on origin(Uncheck to move the viewpoint)", &focusOrigin);
        ImGui::SliderFloat("Set X", &cameraPos.x, -70.5f, 70.5f, "camera x = %.2f");
        ImGui::SliderFloat("Set Y", &cameraPos.y, -70.5f, 70.5f, "camera y = %.2f");
        ImGui::SliderFloat("Set Z", &cameraPos.z, -70.5f, 70.5f, "camera z = %.2f");
        if (ImGui::CollapsingHeader("Camera Transition")) {
            if (ImGui::Button("Save Initial Camera")) {
                startCamera.position = cameraPos;
                startCamera.front = cameraFront;
                startCamera.fov = fov;
            }
            ImGui::SameLine();
            if (ImGui::Button("Save Ending Camera")) {
                endCamera.position = cameraPos;
                endCamera.front = cameraFront;
                endCamera.fov = fov;
            }

            ImGui::Text("Current camera: Position: (%.2f, %.2f, %.2f), Front: (%.2f, %.2f, %.2f), fov: %.2f", cameraPos.x, cameraPos.y, cameraPos.z, cameraFront.x, cameraFront.y, cameraFront.z, fov);
            ImGui::Text("Current initial camera: Position: (%.2f, %.2f, %.2f), Front: (%.2f, %.2f, %.2f), fov: %.2f", startCamera.position.x, startCamera.position.y, startCamera.position.z, startCamera.front.x, startCamera.front.y, startCamera.front.z, startCamera.fov);
            ImGui::Text("Current ending camera: Position: (%.2f, %.2f, %.2f), Front: (%.2f, %.2f, %.2f), fov: %.2f", endCamera.position.x, endCamera.position.y, endCamera.position.z, endCamera.front.x, endCamera.front.y, endCamera.front.z, endCamera.fov);

            if (ImGui::Button("Run Camera Transition")) {
                isRunningTransition = true;
                startTransitionTime = glfwGetTime();
            }
        }
        if (ImGui::CollapsingHeader("Animation")) {
            ImGui::Text("This part provides animations of some actions.");
            if (ImGui::Button("Play: Flick")) {
                if (animeChoose != 1) {
                    startAnimeTime = glfwGetTime();
                    animeChoose = 1;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Play: Fist")) {
                if (animeChoose != 2) {
                    startAnimeTime = glfwGetTime();
                    animeChoose = 2;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Play: Shake hand")) {
                if (animeChoose != 3) {
                    startAnimeTime = glfwGetTime();
                    animeChoose = 3;
                }
            }
        }

        if (ImGui::CollapsingHeader("Numbers")) {
            ImGui::Text("This part provides poses of numbers.");
            if (ImGui::Button("0")) {
                showNum_0();
            }
            ImGui::SameLine();
            if (ImGui::Button("1")) {
                showNum_1();
            }
            ImGui::SameLine();
            if (ImGui::Button("2")) {
                showNum_2();
            }
            ImGui::SameLine();
            if (ImGui::Button("3")) {
                showNum_3();
            }
        }

        if (ImGui::CollapsingHeader("Gestures")) {
            ImGui::Text("This part provides meaningful or meaningless gestures.");
            if (ImGui::Button("Fist")) {
                fist();
            }
            ImGui::SameLine();
            if (ImGui::Button("Orchid")) {
                orchidFinger();
            }
            ImGui::SameLine();
            if (ImGui::Button("Tao")) {
                tao();
            }
        }
        
        if (ImGui::CollapsingHeader("Games")) {
            if (ImGui::Button("Final subtraction!")) {
                animeChoose = 0;
                restoreState();
                is_game1_open = true;
            }
        }
        ImGui::End();
    }
    if (is_game1_open) {
        {
            ImGui::Begin("FINAL SUBTRACTION", &is_game1_open);
            ImGui::Text("Game Rules:");
            ImGui::Text("Here is an integer N, Groove and you take turns to give the integars 1~3.");
            ImGui::Text("Then N minus the corresponding value.");
            ImGui::Text("The last person to give the number (N minus the number is less than or equal to 0) wins!");

            if (ImGui::Button("1")) {
                N_for_game -= 1;
                tips_text = "Minus 1";
                has_clicked = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("2")) {
                N_for_game -= 2;
                tips_text = "Minus 2";
                has_clicked = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("3")) {
                N_for_game -= 3;
                tips_text = "Minus 3";
                has_clicked = true;
            }

            if (ImGui::Button("Remake")) {
                animeChoose = 0;
                restoreState();
                N_for_game = (rand() % (66 - 10 + 1)) + 10;
                tips_text = "Game has been remaked.";
            }
            if (has_clicked && N_for_game > 0) {
                if (Groove.game_final_subtraction(N_for_game)) {
                    showNum_1();
                    startAnimeTime = glfwGetTime();
                    tips_text = "You lose!";
                    animeChoose = 3;
                }
                has_clicked = false;
            }
            if (has_clicked)
                tips_text = "You win!";


            ImGui::Text("Now N = %d", N_for_game);
            ImGui::Text("%s", tips_text.c_str());
            ImGui::End();
        }
    }
}