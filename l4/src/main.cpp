#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <math.h>

const GLchar *vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 position;\n"
    "uniform float scrollOffset;\n"
    "out vec3 vertexColor;\n"
    "void main()\n"
    "{\n"
    " gl_Position = vec4(position.x, position.y+scrollOffset, position.z, 1.0);\n"
    " vertexColor = vec3(0.30f, 0.50f, 1.0f);\n"
    "}\0";

const GLchar *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 fragmentColor;\n"
    "uniform vec3 shapeColor;\n"
    "void main()\n"
    "{\n"
    " fragmentColor = vec4(shapeColor, 1.0f);\n"
    "}\0";

float scrollOffset = 0.0f;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  scrollOffset += yoffset * 0.1f;
  if (scrollOffset > 0.5f) {
    scrollOffset = 0.5f;
  } else if (scrollOffset < -0.5f) {
    scrollOffset = -0.5f;
  }
}

GLfloat colors[][3] = {
    {0.5f, 0.0f, 0.0f},
    {0.3f, 0.1f, 0.0f},
    {0.2f, 0.0f, 1.0f}
};

int colorIndex = 0;

void keyPressPooling(GLFWwindow* window){
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        colorIndex = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        colorIndex = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        colorIndex = 2;
    }
}

int main() {
  // inicjalizacja GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Tworzenie okna
  const unsigned int window_width = 1000;
  const unsigned int window_height = 1000;
  GLFWwindow *window = glfwCreateWindow(window_width, window_height,
                                        "grafika komputerowa", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetScrollCallback(window, scroll_callback);

  // inicjalizacja GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  GLint status;
  GLchar error_message[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if (!status) {
    glGetShaderInfoLog(vertexShader, 512, NULL, error_message);
    std::cout << "Error (Vertex shader): " << error_message << std::endl;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
  if (!status) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, error_message);
    std::cout << "Error (Fragment shader): " << error_message << std::endl;
  }

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
  if (!status) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, error_message);
    std::cout << "Error (Shader program): " << error_message << std::endl;
  }

  glDetachShader(shaderProgram, vertexShader);
  glDetachShader(shaderProgram, fragmentShader);

  GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f, // Vertex 0 
     0.0f,  0.5f, 0.0f, // Vertex 1 
    -0.1f,  0.0f, 0.0f, // Vertex 2 
     -0.7f,  -0.3f, 0.0f, // Vertex 3 
    0.6f, 0.6f, 0.0f, // Vertex 4 
     0.1f,  0.6f, 0.0f, // Vertex 5 
    0.2f,  0.1f, 0.0f, // Vertex 6 
     0.8f,  0.4f, 0.0f, // Vertex 7 
  };

  GLuint indices[] = {
    0, 1, 2, // The first triangle uses vertices 0, 1, and 2
    0, 1, 3, // The second triangle uses vertices 0, 1, and 3
    4, 5, 6, // 
    4, 5, 7, // 
  };


  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);


  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);


  int scrollOffsetLocation = glGetUniformLocation(shaderProgram, "scrollOffset");
  int colorLocation = glGetUniformLocation(shaderProgram, "shapeColor");
  GLfloat defaultColor[] = {0.0f, 1.0f, 0.0f};

  // pętla zdarzeń
  while (!glfwWindowShouldClose(window)) {
    keyPressPooling(window);

    glClearColor(0.18f, 0.2f, 0.22f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glUniform1f(scrollOffsetLocation, scrollOffset);
    glUniform3fv(colorLocation, 1, defaultColor);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glUniform3fv(colorLocation, 1, colors[colorIndex]);
    glUniform1f(scrollOffsetLocation, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(GLuint))); 
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}