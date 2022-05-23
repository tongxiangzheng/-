#include <glad/glad.h>
#include <GLFW/glfw3.h>
extern GLFWwindow* window;
int init();
int initShader(const char *vertexShader,const char *fragmentShader);
int initVAO();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);