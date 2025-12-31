#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;



    

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
		 0.5f, -0.5f
    };
    //申请buffer
    unsigned int buffer;
	glGenBuffers(1, &buffer);
    // 上下文绑定申请的buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
	// 把数据copy到GPU
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), (void*)positions, GL_STATIC_DRAW);
	// 告诉GPU buffer数据格式
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	// 使能顶点属性
    glDisableVertexAttribArray(0);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // draw Triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}