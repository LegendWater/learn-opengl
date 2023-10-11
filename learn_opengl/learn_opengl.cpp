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

int main()
{
    std::cout << "Hello World!\n";

    int width = 1600, height = 900;
    GLFWwindow* window = createWindow(width, height);

    MShader shader(shader_vert, shader_frag);
    shader.use();

    float vertices[] = {
        -1.0, 1.0, -1.0,    0.0, 1.0,
        1.0, 1.0, -1.0,     1.0, 1.0,
        1.0, -1.0, -1.0,     1.0, 0.0,
        -1.0, -1.0, -1.0,    0.0, 0.0
    };

    int indices[] = {
        0, 1, 2,
        2, 3, 0
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

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClearColor(0, 0, 0, 1.0);
    while (!glfwWindowShouldClose(window))
    {
        static auto processInput = [window]() {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }
        };
        processInput();

        float sin = glm::sin(glfwGetTime());
        shader.setFloat("coefficient", sin);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

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
