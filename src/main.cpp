#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <iostream>
#include <cstdint>
#include <math.h>

#include "shader_handler.c"

const int WIDTH = 800;
const int HEIGHT = 600;

int curr_width;
int curr_height;

float vertices[] = {
    //positions           //colors
     0.0f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,//top
    -0.5f, -0.5f, -0.5f,  0.64f, 0.2f, 0.89f,//bottom front left
     0.5f, -0.5f, -0.5f,  0.64f, 0.2f, 0.89f,//bottom front right
    -0.5f, -0.5f, 0.5f,   0.64f, 0.2f, 0.89f,//bottom back left
     0.5f, -0.5f, 0.5f,   0.64f, 0.2f, 0.89f //bottom back right
};
unsigned int indices[] = {
    0, 1, 2,//front triangle
    0, 1, 3,//left triangle
    0, 2, 4,//right triangle
    0, 3, 4 //back triangle
};

const char* vertex_shader_src = get_shader("vertex_shader.vert");
const char* fragment_shader_src = get_shader("fragment_shader.frag");


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
int input_callback(ImGuiInputTextCallbackData* data);

float zoom = 2.0f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Technera", NULL, NULL);

    if (window == NULL) {
        std::cout << "glfw window failed\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "glad failed\n";
        return 2;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //vert shader
    unsigned int vertex_shader = init_shader(GL_VERTEX_SHADER, vertex_shader_src);

    //fragment shader
    unsigned int fragment_shader = init_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

    //shader program
    unsigned int shader_program = init_shader_program(vertex_shader, fragment_shader);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    //IMGUI setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    ImGui::GetStyle().ScrollbarSize = 0.0f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    static char buffer[1024] = "This is a\n text box gng.";

    ImGui::SetNextWindowSize(ImVec2(curr_width/2, curr_height), ImGuiCond_Always);
    while(!glfwWindowShouldClose(window)) {
        process_input(window);

        //start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        //ImGui Window
        glfwGetWindowSize(window, &curr_width, &curr_height);

        io.FontGlobalScale = zoom;

        ImGui::SetNextWindowSizeConstraints(ImVec2(100, curr_height), ImVec2(FLT_MAX, curr_height));
        ImGui::SetNextWindowPos(ImVec2(0.0f,0.0f), ImGuiCond_Always);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
        ImGui::Begin("my window", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
        ImGui::InputTextMultiline("##", buffer, sizeof(buffer), ImVec2(-1.0f, -1.0f), ImGuiInputTextFlags_CallbackAlways, input_callback);

        ImGui::Button("Button", ImVec2(10.0f, 5.0f));

        ImGui::PopStyleColor(1);
        ImGui::End();

        //render stuff
        ImGui::Render();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);
        //rotate the prism
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)curr_width/curr_height, 0.1f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

        unsigned int model_location = glGetUniformLocation(shader_program, "model");
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
        unsigned int projection_location = glGetUniformLocation(shader_program, "projection");
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
        unsigned int view_location = glGetUniformLocation(shader_program, "view");
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

char behind_cursor = 0;
int input_callback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackAlways) {

        //for getting deleted chars
        if (!ImGui::IsKeyPressed(ImGuiKey_Backspace) && !ImGui::IsKeyPressed(ImGuiKey_Enter)) {
            behind_cursor = data->Buf[data->CursorPos - 1];
        }

        //zooming
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Equal)) {
            if (zoom == 5.0f) {
                return 1;
            }

            zoom += 1.0f;
            return 1;
        }
        else if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Minus)) {
            if (zoom == 2.0f) {
                return 1;
            }
            
            zoom -= 1.0f;
            return 1;
        }

        //tabbing
        if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
            data->InsertChars(data->CursorPos, "    ");
            return 1;
        }

        //bracket and quote inserting
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_9)) {
            data->InsertChars(data->CursorPos, ")");
            data->CursorPos = std::max(0, data->CursorPos - 1);
            return 1;
        }
        else if (!ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) {
            data->InsertChars(data->CursorPos, "]");
            data->CursorPos = std::max(0, data->CursorPos - 1);
            return 1;
        }
        else if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) {
            data->InsertChars(data->CursorPos, "}");
            data->CursorPos = std::max(0, data->CursorPos - 1);
            return 1;
        }
        else if (!ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_Apostrophe)) {
            data->InsertChars(data->CursorPos, "\'");
            data->CursorPos = std::max(0, data->CursorPos - 1);
            return 1;
        }
        else if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_Apostrophe)) {
            data->InsertChars(data->CursorPos, "\"");
            data->CursorPos = std::max(0, data->CursorPos - 1);
            return 1;
        }
        
        //bracket and quote removing
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace) && behind_cursor == '(' && data->Buf[data->CursorPos] == ')') {
            data->DeleteChars(data->CursorPos, 1);
            return 1;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_Backspace) && behind_cursor == '{' && data->Buf[data->CursorPos] == '}') {
            data->DeleteChars(data->CursorPos, 1);
            return 1;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_Backspace) && behind_cursor == '[' && data->Buf[data->CursorPos] == ']') {
            data->DeleteChars(data->CursorPos, 1);
            return 1;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_Backspace) && behind_cursor == '\"' && data->Buf[data->CursorPos] == '\"') {
            data->DeleteChars(data->CursorPos, 1);
            return 1;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_Backspace) && behind_cursor == '\'' && data->Buf[data->CursorPos] == '\'') {
            data->DeleteChars(data->CursorPos, 1);
            return 1;
        }

        //moving the cursor back when moving down a line in empty brackets
        if (ImGui::IsKeyPressed(ImGuiKey_Enter) && behind_cursor == '(' && data->Buf[data->CursorPos] == ')') {
            data->InsertChars(data->CursorPos, "    \n");
            data->CursorPos = std::max(0, data->CursorPos - 1);
            return 1;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_Enter) && behind_cursor == '[' && data->Buf[data->CursorPos] == ']') {
            data->InsertChars(data->CursorPos, "    \n");
            data->CursorPos = std::max(0, data->CursorPos - 1);
            return 1;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_Enter) && behind_cursor == '{' && data->Buf[data->CursorPos] == '}') {
            data->InsertChars(data->CursorPos, "    \n");
            data->CursorPos = std::max(0, data->CursorPos - 1);
            return 1;
        }
    }
    return 0;
}

int split_lines(ImGuiInputTextCallbackData* data) {
    
}

