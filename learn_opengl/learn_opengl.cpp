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


//虽然不想但是好像除了把变量放在这也没有更好的办法了
static int win_width = 900, win_height = 900;
MCamera cam;
static float time_point = 0.f, prev_time_point = 0.f, time_delta = 0.f; //这一帧的时间、上一帧的时间、时间间隔, 单位秒

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
    std::cout << "Hello World!\n";

    glm::vec4 test_vec = { 1.0, 1.0, 0.0, 1.0 };
    glm::mat4 trans_mat(1.0);
    trans_mat = glm::translate(trans_mat, glm::vec3(1.0, 2.0, 3.0));
    test_vec = trans_mat * test_vec;

    GLFWwindow* window = createWindow(win_width, win_height);
    glfwSetWindowPos(window, (1920 - win_width) / 2, (1080 - win_height) / 2);
    glfwSetCursorPos(window, win_width / 2, win_height / 2);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    MShader shader(shader_vert, shader_frag);
    shader.use();

    typedef struct Vertex {
        glm::vec3 pos;
        glm::vec2 tex_coord;

        Vertex() = default;
        Vertex(float x, float y, float z, float s, float t) {
            pos = glm::vec3(x, y, z);
            tex_coord = glm::vec2(s, t);
        }
        Vertex(glm::vec3 pos, glm::vec2 tex_coord) {
            this->pos = pos;
            this->tex_coord = tex_coord;
        }
    }Vertex;

    Vertex vertices[] = {
        /*Vertex{ -5,  5, 0.0,    0.0, 1.0},
        Vertex{  5,  5, 0.0,    1.0, 1.0},
        Vertex{  5, -5, 0.0,    1.0, 0.0},
        Vertex{ -5, -5, 0.0,    0.0, 0.0},*/
        Vertex{ -0.5f, -0.5f, -0.5f,  0.0f, 0.0f},
        Vertex{  0.5f, -0.5f, -0.5f,  1.0f, 0.0f},
        Vertex{  0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        Vertex{  0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        Vertex{ -0.5f,  0.5f, -0.5f,  0.0f, 1.0f},
        Vertex{ -0.5f, -0.5f, -0.5f,  0.0f, 0.0f},

        Vertex{ -0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
        Vertex{  0.5f, -0.5f,  0.5f,  1.0f, 0.0f},
        Vertex{  0.5f,  0.5f,  0.5f,  1.0f, 1.0f},
        Vertex{  0.5f,  0.5f,  0.5f,  1.0f, 1.0f},
        Vertex{ -0.5f,  0.5f,  0.5f,  0.0f, 1.0f},
        Vertex{ -0.5f, -0.5f,  0.5f,  0.0f, 0.0f},

        Vertex{ -0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
        Vertex{ -0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        Vertex{ -0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        Vertex{ -0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        Vertex{ -0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
        Vertex{ -0.5f,  0.5f,  0.5f,  1.0f, 0.0f},

        Vertex{  0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
        Vertex{  0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        Vertex{  0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        Vertex{  0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        Vertex{  0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
        Vertex{  0.5f,  0.5f,  0.5f,  1.0f, 0.0f},

        Vertex{ -0.5f, -0.5f, -0.5f,  0.0f, 1.0f},
        Vertex{  0.5f, -0.5f, -0.5f,  1.0f, 1.0f},
        Vertex{  0.5f, -0.5f,  0.5f,  1.0f, 0.0f},
        Vertex{  0.5f, -0.5f,  0.5f,  1.0f, 0.0f},
        Vertex{ -0.5f, -0.5f,  0.5f,  0.0f, 0.0f},
        Vertex{ -0.5f, -0.5f, -0.5f,  0.0f, 1.0f},

        Vertex{ -0.5f,  0.5f, -0.5f,  0.0f, 1.0f},
        Vertex{  0.5f,  0.5f, -0.5f,  1.0f, 1.0f},
        Vertex{  0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
        Vertex{  0.5f,  0.5f,  0.5f,  1.0f, 0.0f},
        Vertex{ -0.5f,  0.5f,  0.5f,  0.0f, 0.0f},
        Vertex{ -0.5f,  0.5f, -0.5f,  0.0f, 1.0f}
    };

    int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

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

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof GLfloat, (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof GLfloat, (void*)(3 * sizeof GLfloat));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    MTexture2D tex_box(R"(F:\pictures\box.jpg)");
    MTexture2D tex_poster(R"(F:\pictures\11775.jpg)");

    glActiveTexture(GL_TEXTURE0);
    tex_box.bind();
    glActiveTexture(GL_TEXTURE1);
    tex_poster.bind();

    shader.setInt("tex", 0);
    shader.setInt("tex2", 1);

    //设置摄像机属性
    glm::vec3 cam_pos(0, 0, 10);
    glm::vec3 cam_lookat(0, 0, 0);
    constexpr float cam_fov = glm::radians(45.0f);
    cam.moveTo(cam_pos);
    cam.lookAt(cam_lookat);
    cam.zoom(cam_fov);
    shader.setMat4("view", cam.getView());

    ////定义投影矩阵
    //glm::mat4 projection(1.0f);
    //projection = glm::perspective(glm::radians(45.0f), win_width * 1.0f / win_height, 0.1f, 100.0f);
    ////projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 1.0f, 100.0f);
    //shader.setMat4("projection", projection);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    CHECK_GL_ERROR();

    glClearColor(0, 0, 0, 1.0);
    glClearDepth(1.0f);
    unsigned long long frame_count = 0;
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

        shader.setMat4("projection", cam.getProjection());
        shader.setMat4("view", cam.getView());

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
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (int i = 0; i < 10; i++) { //绘制10个立方体
            glm::mat4 model(1.0f);
            //model = glm::rotate(model, glm::radians(25*(time_point + i)), glm::vec3(0, 1.0f, 1.0f));
            model = glm::translate(model, cube_positions[i]);
            model = glm::rotate(model, glm::radians(25 * (time_point + i)), glm::vec3(0, 1.0f, 1.0f));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        glfwSwapBuffers(window);

        glfwPollEvents();

        ++frame_count;
    }

    auto end_time = std::chrono::steady_clock::now();

    glfwTerminate();

    auto time_total = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::printf("\nrender %lld frames in %lld seconds, average fps=%.2f\n", frame_count, time_total / 1000, frame_count / (time_total / 1000.0));

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
    static double last_x = win_width / 2, last_y = win_height / 2;
    static const double move_rate = 0.12;
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
    static const float move_rate = 5.0f;
    float cam_move = time_delta * move_rate;
    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
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
    static const float move_rate = 1.0f;
    float delta_zoom = -glm::radians(yoffset) * move_rate;
    cam.zoom(delta_zoom);
}
