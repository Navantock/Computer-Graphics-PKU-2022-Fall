#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "draw_ui.h"
#include "config.h"

#include <stdlib.h>
#include <iostream>

#define MAX_PARTICLE_NUM 10000

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int error, const char* description);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void playCameraTransition();
void processKeyCommand();
void processMouseCommand(GLFWwindow* window);

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float focusDistance = 40.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// model param
float tilt_angle = 23.5;
float animeTime = 0.0f;

// light src presettings
glm::vec3 light_initial_pos = glm::vec3(0.0f, 0.0f, 20.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetErrorCallback(error_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
        exit(EXIT_FAILURE);


    ImGui::CreateContext();     // Setup Dear ImGui context
    ImGui::StyleColorsDark();       // Setup Dear ImGui style
    ImGui_ImplGlfw_InitForOpenGL(window, true);     // Setup Platform/Renderer backends
    ImGui_ImplOpenGL3_Init("#version 450");

    glEnable(GL_DEPTH_TEST);

    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------

    Shader mainShader(SRC_DIR"/src/f.vert", SRC_DIR"/src/f.frag");
    Shader shadowShader(SRC_DIR"/src/d.vert", SRC_DIR"/src/d.frag", SRC_DIR"/src/d.geom");
    Model ourModel(DATA_DIR"/earth/Earth_2K.obj");
    ParticleRing starRing(MAX_PARTICLE_NUM, 10.0, 0.1);
    starRing.SetTex(DATA_DIR"/particles.jpg");
    LightSrc fixedStarLightSrc(light_initial_pos);


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processKeyCommand();
        processMouseCommand(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        draw_ui(camera, ourModel, starRing, fixedStarLightSrc);
        ImGui::Render();

        if (isRunningTransition)
            playCameraTransition();

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        
        glm::mat4 model = glm::mat4(1.0f);
        // set light source
        fixedStarLightSrc.use(shadowShader);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shadowShader.setMat4("model", model);
        shadowShader.setBool("is_particle", false);
        ourModel.Draw(shadowShader);
        shadowShader.setBool("is_particle", true);
        starRing.Draw(shadowShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // coordinates system
        mainShader.use();
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        if (camera.focusOrigin) {
            camera.pos = -focusDistance * camera.front;
        }
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        mainShader.setMat4("projection", projection);
        mainShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::rotate(model, glm::radians(tilt_angle), glm::vec3(0.0f, 0.0f, 1.0f));
        if (ourModel.rotateAnime)
            animeTime = fmod(animeTime + deltaTime * 10, 360);
        model = glm::rotate(model, glm::radians(animeTime), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));


        mainShader.setMat4("model", model);
        mainShader.setVec3("viewPos", camera.pos);

        fixedStarLightSrc.use2(mainShader);
        mainShader.setBool("is_particle", false);
        ourModel.Draw(mainShader);
        mainShader.setBool("is_particle", true);
        starRing.Draw(mainShader);
        starRing.UpdateState(deltaTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


static void processKeyCommand() {
}

static void processMouseCommand(GLFWwindow* window) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwGetCursorPos(window, &camera.lastX, &camera.lastY);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, {});
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
        camera.front = interpolatedqua * startCamera.front;
    }

    camera.fov = startCamera.fov + (delta_t / transitionTimeLength) * delta_fov;
    camera.pos = startCamera.position + (delta_t / transitionTimeLength) * dis_vec;
}


// callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
    case GLFW_KEY_SPACE:
        camera.fov = 45.0f;
        camera.pos = glm::vec3(0.0f, 0.0f, 45.0f);
        camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
        break;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    float sensitivity = 0.05f;
    float xoffset = xpos - camera.lastX;
    float yoffset = ypos - camera.lastY;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    camera.lastX = xpos;
    camera.lastY = ypos;

    camera.yaw += xoffset;
    camera.pitch += yoffset;
    
    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if (camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front.y = sin(glm::radians(camera.pitch));
    front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));

    if (camera.focusOrigin)
        camera.pos = -focusDistance * front;
    camera.front = -glm::normalize(front);
}


static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (camera.fov >= 1.0f && camera.fov <= 45.0f)
        camera.fov -= yoffset;
    if (camera.fov <= 1.0f)
        camera.fov = 1.0f;
    if (camera.fov >= 45.0f)
        camera.fov = 45.0f;
}
