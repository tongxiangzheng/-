#include "init.h"
#include "def.h"
#include<iostream>
#include<fstream>

GLFWwindow* window;
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    H_W = 1.0 * SCR_HEIGHT / SCR_WIDTH;
}
void glfw_error(int error, const char* description) {
    std::cout << "Error id:" << error << ' ' << description << std::endl;
}

char* loadfile(const char* path) {
    FILE* infile;
    fopen_s(&infile, path, "rb");
    if (!infile) {
        return NULL;
    }
    fseek(infile, 0, SEEK_END);
    long long len = ftell(infile);
    fseek(infile, 0, SEEK_SET);
    char* source = new char[len + 1];
    fread(source, 1, len, infile);
    fclose(infile);
    source[len] = 0;
    return source;
}
unsigned int SCR_WIDTH = 1000, SCR_HEIGHT = 1000;
float H_W = 1;

int init() {
    glfwSetErrorCallback(glfw_error);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_REFRESH_RATE, 10);
    //glfwWindowHint(GLFW_SAMPLES, 4);


    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "snake", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_CURSOR_NORMAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
    return 0;
}
int initShader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    char* vertexShaderSource = loadfile(vertexShaderPath), * fragmentShaderSource = loadfile(fragmentShaderPath);
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}
int initVAO() {
    float vertices[] = {
        -1.0f, -1.0f, -1.0f,  -1.0f,
        -1.0f, 1.0f, -1.0f,  1.0f,
        1.0f,  -1.0f, 1.0f,  -1.0f,
        1.0f, 1.0f, 1.0f,  1.0f,
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    return VAO;
}