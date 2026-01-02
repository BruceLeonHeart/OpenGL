#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};  

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	std::fstream stream(filepath);
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if(line.find("#shader") != std::string::npos)
        {
            if(line.find("vertex") != std::string::npos)
            {
                // vertex shader
				type = ShaderType::VERTEX;

            }
            else if(line.find("fragment") != std::string::npos)
            {
                // fragment shader
				type = ShaderType::FRAGMENT;
            }
		}
        else
        {
			ss[(int)type] << line << '\n';
        }
	}
	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Failed to compile "
            << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader!" << std::endl;
        std::cerr << message << std::endl;
        glDeleteShader(id);
        return 0;
	}
	return id;
}

static unsigned int CreateProgram(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER,vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

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
    // 使能顶点属性
	glEnableVertexAttribArray(0);
    // 告诉GPU buffer数据格式
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

    std::string vertexShader =
        "#version 330 core\n"
        "layout(location = 0) in vec4 position; \n"
        "void main() \n"
        "{ \n"
        "   gl_Position = position; \n"
        "} \n";

    std::string fragmentShader =
        "#version 330 core\n"
        "layout(location = 0) out vec4 color; \n"
        "void main() \n"
        "{ \n"
        "   color = vec4(1.0, 0.0, 0.0, 1.0); \n"
        "} \n";
	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
	std::cout << "VERTEX" << std::endl;
	std::cout << source.VertexSource << std::endl;
	std::cout << "FRAGMENT" << std::endl;
	std::cout << source.FragmentSource << std::endl;
	unsigned int program = CreateProgram(source.VertexSource, source.FragmentSource);

	glUseProgram(program);

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

	glDeleteProgram(program);
    glfwTerminate();
    return 0;
}