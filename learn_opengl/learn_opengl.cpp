// learn_opengl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include <gltools.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "shaders.h"
#include "MShader.h"
#include "MTexture.h"
#include "MCamera.h"
#include <chrono>
#include <string>
#include "Model.h"

//struct Vertex {
//    glm::vec3 pos;
//    glm::vec3 normal;
//    glm::vec2 tex_coord;
//
//    Vertex() = default;
//    Vertex(float x, float y, float z, float nx, float ny, float nz, float s, float t) {
//        pos = glm::vec3(x, y, z);
//        normal = glm::vec3(nx, ny, nz);
//        tex_coord = glm::vec2(s, t);
//    }
//    Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 tex_coord)
//    :pos(pos), normal(normal), tex_coord(tex_coord) {}
//};

struct Point3D {
    glm::vec3 pos;

    Point3D() : pos(0) {}
    Point3D(float x, float y, float z) : pos({ x, y, z }) {}
    Point3D(glm::vec3 pos) : pos(pos) {}
};

//虽然不想但是好像除了把变量放在这也没有更好的办法了
static int screen_width = 0, screen_height = 0;
static int win_width = 1500, win_height = 1500;
MCamera cam;
static float time_point = 0.f, prev_time_point = 0.f, time_delta = 0.f; //这一帧的时间、上一帧的时间、时间间隔, 单位秒

static bool switch_change_light_color = false; //光源的颜色随时间改变
static bool switch_rotate_objects = false; //物体随时间自转

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
    std::cout << "Hello World!\n";

    /*glm::vec4 test_vec = { 1.0, 1.0, 0.0, 1.0 };
    glm::mat4 trans_mat(1.0);
    trans_mat = glm::translate(trans_mat, glm::vec3(1.0, 2.0, 3.0));
    test_vec = trans_mat * test_vec;*/

    GLFWwindow* window = createWindow(win_width, win_height, "", false);
    static MMonitorInfo biggest = getBiggestMonitorInfo();
    screen_width = biggest.width, screen_height = biggest.height;
    glfwSetWindowPos(window, (screen_width - win_width) / 2, (screen_height - win_height) / 2);
    glfwSetCursorPos(window, static_cast<double>(win_width / 2), static_cast<double>(win_height / 2));
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1); //核显在运行时不加这行会出现帧数过高的问题

    /*auto check_binding = []() {
        GLint pre_vao, pre_ebo;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &pre_vao);
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &pre_ebo);
        std::cout << "binding vao: " << pre_vao << ", binding ebo: " << pre_ebo << std::endl;
        };
    GLuint vao_[2], ebo_[2];
    glGenVertexArrays(2, vao_);
    glGenBuffers(2, ebo_);

    glBindVertexArray(vao_[0]);
    check_binding();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_[0]);
    check_binding();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_[1]);
    check_binding();
    glBindVertexArray(0);
    check_binding();
    glBindVertexArray(vao_[1]);
    check_binding();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_[1]);
    check_binding();
    glBindVertexArray(vao_[0]);
    check_binding();*/

#pragma region objects define
    MShader shader_obj(shader_vert, shader_frag);
    //shader_obj.use();

    //每个立方体的顶点和纹理坐标
    Vertex vertices[] = {
        /*Vertex{ -5,  5, 0.0,    0.0, 1.0},
        Vertex{  5,  5, 0.0,    1.0, 1.0},
        Vertex{  5, -5, 0.0,    1.0, 0.0},
        Vertex{ -5, -5, 0.0,    0.0, 0.0},*/
        /*             position               normal               tex*/
        // Back face
        Vertex{ -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f},
        Vertex{  0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f},
        Vertex{  0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f},
        Vertex{  0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f},
        Vertex{ -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f},
        Vertex{ -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f},
        // Front face
        Vertex{ -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f},
        Vertex{  0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f},
        Vertex{  0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f},
        Vertex{  0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f},
        Vertex{ -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f},
        Vertex{ -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f},
        // Left face
        Vertex{ -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f},
        Vertex{ -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 1.0f},
        Vertex{ -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f},
        Vertex{ -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 1.0f},
        Vertex{ -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    0.0f, 0.0f},
        Vertex{ -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,    1.0f, 0.0f},
        // Right face
        Vertex{  0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f},
        Vertex{  0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f},
        Vertex{  0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 1.0f},
        Vertex{  0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 1.0f},
        Vertex{  0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    1.0f, 0.0f},
        Vertex{  0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,    0.0f, 0.0f},
        // Bottom face
        Vertex{ -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f},
        Vertex{  0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 1.0f},
        Vertex{  0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f},
        Vertex{  0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    1.0f, 0.0f},
        Vertex{ -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 0.0f},
        Vertex{ -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,    0.0f, 1.0f},
        // Top face
        Vertex{ -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f},
        Vertex{  0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f},
        Vertex{  0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 1.0f},
        Vertex{  0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    1.0f, 0.0f},
        Vertex{ -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 1.0f},
        Vertex{ -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,    0.0f, 0.0f}
    };
    //立方体一个面4个顶点的下标
    int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    //十个立方体在世界坐标系的位置
    glm::vec3 cube_positions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    //立方体的颜色
    glm::vec3 cube_color(0.914f, 0.753f, 0.580f);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, pos));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, tex_coord));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    MTexture2D tex_box_diffuse(R"(D:\3Dmaterials\pictures\box(1)_diffuse.png)");
    MTexture2D tex_box_specular(R"(D:\3Dmaterials\pictures\box(1)_specular.png)");

    shader_obj.use();
    glActiveTexture(GL_TEXTURE0);
    tex_box_diffuse.bind();
    glActiveTexture(GL_TEXTURE1);
    tex_box_specular.bind();

    shader_obj.setInt("material.diffuse", 0);
    shader_obj.setInt("material.specular", 1);
    CHECK_GL_ERROR();
#pragma endregion objects define
#pragma region light source define
    MShader shader_light_source(shader_light_source_vert, shader_light_source_frag);

    //光源的结构(将光源视为立方体)
    const glm::vec3 light_points[] = {
        { -0.5f, -0.5f, -0.5f},
        {  0.5f,  0.5f, -0.5f},
        {  0.5f, -0.5f, -0.5f},
        {  0.5f,  0.5f, -0.5f},
        { -0.5f, -0.5f, -0.5f},
        { -0.5f,  0.5f, -0.5f},

        { -0.5f, -0.5f,  0.5f},
        {  0.5f, -0.5f,  0.5f},
        {  0.5f,  0.5f,  0.5f},
        {  0.5f,  0.5f,  0.5f},
        { -0.5f,  0.5f,  0.5f},
        { -0.5f, -0.5f,  0.5f},

        { -0.5f,  0.5f,  0.5f},
        { -0.5f,  0.5f, -0.5f},
        { -0.5f, -0.5f, -0.5f},
        { -0.5f, -0.5f, -0.5f},
        { -0.5f, -0.5f,  0.5f},
        { -0.5f,  0.5f,  0.5f},

        {  0.5f,  0.5f,  0.5f},
        {  0.5f, -0.5f, -0.5f},
        {  0.5f,  0.5f, -0.5f},
        {  0.5f, -0.5f, -0.5f},
        {  0.5f,  0.5f,  0.5f},
        {  0.5f, -0.5f,  0.5f},

        { -0.5f, -0.5f, -0.5f},
        {  0.5f, -0.5f, -0.5f},
        {  0.5f, -0.5f,  0.5f},
        {  0.5f, -0.5f,  0.5f},
        { -0.5f, -0.5f,  0.5f},
        { -0.5f, -0.5f, -0.5f},

        { -0.5f,  0.5f, -0.5f},
        {  0.5f,  0.5f,  0.5f},
        {  0.5f,  0.5f, -0.5f},
        {  0.5f,  0.5f,  0.5f},
        { -0.5f,  0.5f, -0.5f},
        { -0.5f,  0.5f,  0.5f}
    };
    //4个光源在世界坐标系中的位置
    const glm::vec3 light_pos[] = {
        { 0.f,  0.f, -3.5f },
        { 2.3f, -3.3f, -4.0f},
        { -4.0f,  2.0f, -12.0f },
        { 0.0f,  1.0f, -13.0f }
    };
    //光源颜色
    const glm::vec3 light_color = {1.0f, 1.0f, 1.0f};
    //光线衰减系数
    const glm::vec3 light_attenuation = {1.0f, 0.025f, 0.0047f};

    GLuint VAO_light, VBO_light, EBO_light;
    glGenVertexArrays(1, &VAO_light);
    glGenBuffers(1, &VBO_light);
    glGenBuffers(1, &EBO_light);

    glBindVertexArray(VAO_light);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_light);
    glBufferData(GL_ARRAY_BUFFER, sizeof light_points, light_points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof GLfloat, (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_light);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    shader_light_source.use();
    //shader_light_source.setVec("color", light_color);
    shader_light_source.setFloat("constant", light_attenuation[0]);
    shader_light_source.setFloat("linear", light_attenuation[1]);
    shader_light_source.setFloat("quadratic", light_attenuation[2]);
    CHECK_GL_ERROR();
#pragma endregion light source define
#pragma region model define
    Model nanosuit(R"(D:\3Dmaterials\model\nanosuit\nanosuit.obj)");
    MShader shader_nanosuit(shader_nanosuit_vert, shader_nanosuit_frag);
#pragma endregion model define

    shader_obj.use();

    //设置摄像机属性
    glm::vec3 cam_pos(0, 0, 10);
    glm::vec3 cam_lookat(0, 0, 0);
    constexpr float cam_fov = glm::radians(45.0f);
    cam.moveTo(cam_pos);
    cam.lookAt(cam_lookat);
    cam.zoomTo(cam_fov);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    CHECK_GL_ERROR();

    //启用面剔除
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    unsigned long long frame_count = 0; //总渲染帧数统计
    auto start_time = std::chrono::steady_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        prev_time_point = time_point;
        time_point = glfwGetTime(); //单位秒
        time_delta = time_point - prev_time_point;

        char buf[1024];
        sprintf_s(buf, 1024, "render one frame cost %.2f ms, fps = %.2f", time_delta * 1000, 1.0f / time_delta);
        int strlen = std::strlen(buf);
        for (int i = 0; i < strlen; i++) {
            buf[strlen + i] = '\b';
        }
        buf[strlen * 2] = '\0';
        std::printf("%s", buf);
        float sin = glm::sin(time_point);
        float cos = glm::cos(time_point);

        glm::vec3 light_color_frame = light_color;
        if (switch_change_light_color) {
            light_color_frame *= glm::vec3(sin, glm::cos(time_point + 1), 0.5f + (glm::sin(time_point + 2) + glm::cos(time_point + 3)) / 2) + 0.5f;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#define DRAW_BOXES 1
#if defined DRAW_BOXES && DRAW_BOXES == 1
        shader_obj.use();
        shader_obj.setMat4("projection", cam.getProjection());
        shader_obj.setMat4("view", cam.getView());
        shader_obj.setVec("view_pos", cam.getPos());
        //shader_obj.setVec("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        //shader_obj.setVec("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        //shader_obj.setVec("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        shader_obj.setFloat("material.shininess", 32.0f);
        for (int i = 0, light_num = sizeof light_pos / sizeof glm::vec3; i < light_num; i++)
        {
            std::string light_name = "light[" + std::to_string(i) + "]";
            glm::vec3 light_color_i = glm::vec3(glm::sin((float)i) + 0.6, glm::cos((float)i) + 0.6, 1 - glm::sin((float)i) + 0.6) * light_color_frame;
            shader_obj.setVec((light_name + ".pos").c_str(), light_pos[i]);
            shader_obj.setVec((light_name + ".ambient").c_str(), 0.2f * light_color_i);
            shader_obj.setVec((light_name + ".diffuse").c_str(), 0.5f * light_color_i);
            shader_obj.setVec((light_name + ".specular").c_str(), 1.0f * light_color_i);
            shader_obj.setFloat((light_name + ".constant").c_str(), light_attenuation[0]);
            shader_obj.setFloat((light_name + ".linear").c_str(), light_attenuation[1]);
            shader_obj.setFloat((light_name + ".quadratic").c_str(), light_attenuation[2]);
        }

        if (0) { //CPU变换
            glm::mat4 model(1.0f);
            Vertex vertex_copy[sizeof vertices / sizeof Vertex];
            memcpy_s(vertex_copy, sizeof vertex_copy, vertices, sizeof vertices);
            for (Vertex& v : vertex_copy) {
                glm::vec4 point(v.pos, 1.0);
                auto after_model = model * point;
                auto after_view = cam.getView() * after_model;
                point = cam.getProjection() * after_view;
                v.pos = point;
            }
        }
        
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        tex_box_diffuse.bind();
        glActiveTexture(GL_TEXTURE1);
        tex_box_specular.bind();
        for (int i = 0; i < 10; i++) { //绘制10个立方体
            glm::mat4 model(1.0f);
            if (switch_rotate_objects) {
                model = glm::rotate(model, glm::radians(25 * (time_point + i)), glm::vec3(1.0f, -1.0f, -1.0f));
            }
            model = glm::translate(model, cube_positions[i]);
            if (switch_rotate_objects) {
                model = glm::rotate(model, glm::radians(25.0f * (time_point + i)), glm::vec3(0, 1.0f, 1.0f));
            }
            else
            {
                model = glm::rotate(model, glm::radians(25.0f * (i)), glm::vec3(0, 1.0f, 1.0f));
            }
            shader_obj.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

#endif
#define DRAW_NANOSUIT 1
#if defined DRAW_NANOSUIT && DRAW_NANOSUIT == 1
        shader_nanosuit.use();
        glm::mat4 model_nanosuit(1.0f);
        model_nanosuit = glm::translate(model_nanosuit, cube_positions[0] + glm::vec3(3.0f, 0.0f, 0.0f));
        model_nanosuit = glm::scale(model_nanosuit, glm::vec3(0.125f));
        model_nanosuit = glm::rotate(model_nanosuit, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader_nanosuit.setMat4("model", model_nanosuit);
        shader_nanosuit.setMat4("view", cam.getView());
        shader_nanosuit.setMat4("projection", cam.getProjection());
        shader_nanosuit.setVec("view_pos", cam.getPos());
        shader_nanosuit.setBool("material.diffuse_exist", false);
        shader_nanosuit.setBool("material.specular_exist", false);
        shader_nanosuit.setFloat("material.shininess", 32.0f);
        for (int i = 0, light_num = sizeof light_pos / sizeof glm::vec3; i < light_num; i++)
        {
            std::string light_name = "light[" + std::to_string(i) + "]";
            glm::vec3 light_color_i = glm::vec3(glm::sin((float)i) + 0.6, glm::cos((float)i) + 0.6, 1 - glm::sin((float)i) + 0.6) * light_color_frame;
            shader_nanosuit.setVec((light_name + ".pos").c_str(), light_pos[i]);
            shader_nanosuit.setVec((light_name + ".ambient").c_str(), 0.2f * light_color_i);
            shader_nanosuit.setVec((light_name + ".diffuse").c_str(), 0.5f * light_color_i);
            shader_nanosuit.setVec((light_name + ".specular").c_str(), 1.0f * light_color_i);
            shader_nanosuit.setFloat((light_name + ".constant").c_str(), light_attenuation[0]);
            shader_nanosuit.setFloat((light_name + ".linear").c_str(), light_attenuation[1]);
            shader_nanosuit.setFloat((light_name + ".quadratic").c_str(), light_attenuation[2]);
        }
        nanosuit.draw(shader_nanosuit);
#endif // DRAW_NANOSUIT && DRAW_NANOSUIT == 1

#define DRAW_LIGHT_SOURCE 1
#if defined DRAW_LIGHT_SOURCE && DRAW_LIGHT_SOURCE == 1

        glBindVertexArray(VAO_light);
        shader_light_source.use();
        for (int i = 0; i < sizeof light_pos / sizeof glm::vec3; i++)
        {
            glm::vec3 light_color_i = glm::vec3(glm::sin((float)i) + 0.6, glm::cos((float)i) + 0.6, 1 - glm::sin((float)i) + 0.6) * light_color_frame;
            shader_light_source.setVec("color", light_color_i);
            shader_light_source.setVec("pos", light_pos[i]);
            shader_light_source.setVec("view_pos", cam.getPos());
            glm::mat4 model(1.0f);
            model = glm::translate(model, light_pos[i]);
            model = glm::scale(model, glm::vec3(0.25));
            shader_light_source.setMat4("model", model);
            shader_light_source.setMat4("view", cam.getView());
            shader_light_source.setMat4("projection", cam.getProjection());

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
#endif // DRAW_LIGHT_SOURCE && DRAW_LIGHT_SOURCE == 1       

        glfwSwapBuffers(window);

        glfwPollEvents();

        ++frame_count;
    }

    auto end_time = std::chrono::steady_clock::now();

    //clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO_light);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO_light);
    glDeleteBuffers(1, &EBO_light);
    glfwTerminate();

    auto time_total = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::printf("\nrender %lld frames in %.2f seconds, average fps=%.2f\n", frame_count, time_total / 1000.0, frame_count / (time_total / 1000.0));

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static double last_x = static_cast<double>(win_width / 2), last_y = static_cast<double>(win_height / 2);
    constexpr double move_rate = 0.12;
    //计算pitch俯仰角、yaw偏航角, 不考虑roll翻滚角
    float xoffset = (last_x - xpos) * move_rate;
    float yoffset = (last_y - ypos) * move_rate;
    /*if (abs(xoffset) < 0.01 && abs(yoffset) < 0.01) {
        return;
    }*/
    last_x = xpos;
    last_y = ypos;

    float yaw = glm::radians(xoffset);
    float pitch = glm::radians(yoffset);

    cam.shake(pitch, yaw);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    constexpr float move_rate = 5.0f;
    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
        float cam_move = time_delta * move_rate;
        switch (key)
        {
        case GLFW_KEY_A:
            //std::printf("left pressed\n");
            cam.move(0, -cam_move, 0);
            break;
        case GLFW_KEY_D:
            //std::printf("right pressed\n");
            cam.move(0, cam_move, 0);
            break;
        case GLFW_KEY_W:
            //std::printf("up pressed\n");
            cam.move(cam_move, 0, 0);
            break;
        case GLFW_KEY_S:
            //std::printf("down pressed\n");
            cam.move(-cam_move, 0, 0);
            break;
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            return;
        default:
            break;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    constexpr float move_rate = 1.0f;
    float delta_zoom = -glm::radians(yoffset) * move_rate;
    cam.zoom(delta_zoom);
}
