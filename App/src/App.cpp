#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}


static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

static void GLCheckError()
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
    }
}

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
            if (type != ShaderType::NONE)
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
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    if (vs == 0 || fs == 0) {
        std::cerr << "Shader compilation failed. Aborting program creation." << std::endl;
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        glDeleteProgram(program);
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::string message(length, ' ');
        glGetProgramInfoLog(program, length, &length, &message[0]);
        std::cerr << "Program linking failed:\n" << message << std::endl;
        glDeleteShader(vs);
        glDeleteShader(fs);
        glDeleteProgram(program);
        return 0;
    }

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

	glfwSwapInterval(1); // Enable vsync 垂直同步

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	
	// draw Square  
    // anti-clockwise
    float positions[] = {
		-0.5f, -0.5f, // left bottom
         0.5f, -0.5f,
		 0.5f, 0.5f,
		-0.5f, 0.5f,
    };

	// indices for square using two triangles
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
	};

    
    //申请 vertex buffer
    unsigned int positionBuffer;
    GLCall(glGenBuffers(1, &positionBuffer));
    // 上下文绑定申请的buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, positionBuffer));
	// 把数据copy到GPU
    GLCall(glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), (void*)positions, GL_STATIC_DRAW));

    //申请 indices buffer
    unsigned int indexBuffer;
    GLCall(glGenBuffers(1, &indexBuffer));
    // 上下文绑定申请的buffer
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer));
    // 把数据copy到GPU
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), (void*)indices, GL_STATIC_DRAW));

    // 使能顶点属性
	glEnableVertexAttribArray(0);
    // 告诉GPU buffer数据格式
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    if (source.VertexSource.empty() || source.FragmentSource.empty()) {
        std::cerr << "Shader sources are empty. Check shader file path and that the file was copied to the working directory." << std::endl;
        return -1;
    }
	//std::cout << "VERTEX" << std::endl;
	//std::cout << source.VertexSource << std::endl;
	//std::cout << "FRAGMENT" << std::endl;
	//std::cout << source.FragmentSource << std::endl;
	unsigned int program = CreateProgram(source.VertexSource, source.FragmentSource);
    if (program == 0) {
        std::cerr << "Failed to create shader program." << std::endl;
        return -1;
    }
    GLCall(glUseProgram(program));

    int location = glGetUniformLocation(program, "u_Color");
    if (location == -1) {
        std::cerr << "Uniform 'u_Color' not found (location = -1). Possible causes: name mismatch, shader not linked, or optimized out." << std::endl;
        return -1;
    }
    ASSERT(location != -1);

    float r = 0.1f;
    

	float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
		//auto start = std::chrono::high_resolution_clock::now();

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        if(r > 1.0f)
            increment = -0.05f;
        else if(r < 0.0f)
			increment = 0.05f;

        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        // draw Square using indices
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        //GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr));

		r += increment;
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        //auto end = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<float, std::milli> duration = end - start;
		//std::cout << "Frame Time: " << duration.count() << " ms" << std::endl;
        //std::cout << "Frate Time: " << static_cast<int>(1000.0 / duration.count()) << " Hz" << std::endl;
    }

    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}