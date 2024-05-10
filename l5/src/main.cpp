#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <iostream>

const GLchar *vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec3 color;\n"
    "layout(location = 2) in vec2 texture;\n"
    "out vec3 vertexColor;\n"
    "out vec2 vertexTexture;\n"
    "void main()\n"
    "{\n"
    " gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
    " vertexColor = color;\n"
    " vertexTexture = texture;\n"
    "}\0";

const GLchar *fragmentShaderSource =
    "#version 330 core\n"
    "in vec3 vertexColor;\n"
    "in vec2 vertexTexture;\n"
    "out vec4 fragmentColor;\n"
    "uniform sampler2D uniTexture;\n"
    "uniform float uniScroll;\n"
    "void main()\n"
    "{\n"
    "    fragmentColor = mix(texture(uniTexture, vertexTexture), vec4(0.30f, 0.50f, 1.0f, 0.0f), uniScroll);\n"
    "}\0";

float scrollOffset = 0.0f;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  scrollOffset += yoffset * 0.1f;
  if(scrollOffset < 0.0) scrollOffset = 0.0;
    if(scrollOffset > 1.0) scrollOffset = 1.0;
}

int mode = 0;

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
      mode = 1;  
  } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
      mode = 2; 
  } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
      mode = 3;  
  }
}

void processInputKeyboard(GLFWwindow* window){
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
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

  // inicjalizacja GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  glEnable(GL_DEPTH_TEST);

  GLint status;
  GLchar error_message[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if (!status) {
    glGetShaderInfoLog(vertexShader, 512, NULL, error_message);
    std::cout << "Error (Vertex shader): " << error_message << "\n";
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
  if (!status) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, error_message);
    std::cout << "Error (Fragment shader): " << error_message << "\n";
  }

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
  if (!status) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, error_message);
    std::cout << "Error (Shader program): " << error_message << "\n";
  }

  glDetachShader(shaderProgram, vertexShader);
  
  GLfloat vertices[] = {
      -0.8f, -0.3f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
      -0.2f, -0.3f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 
      -0.2f, 0.3f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 
      -0.8f, 0.3f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 
      0.2f, -0.3f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,      
      0.8f, -0.3f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,      
      0.5f, 0.3f, 0.0f,   1.0f, 0.0f, 0.0f,   0.5f, 1.0f,     
  };

  GLuint indices[] = {
    0, 1, 2,
    0, 2, 3,
    4, 5, 6     
  };
  glDetachShader(shaderProgram, fragmentShader);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  int width_first, height_first, nrChannels_first;
  stbi_set_flip_vertically_on_load(true);
  GLubyte *data_first = stbi_load("../textures/first.png", &width_first,
                                  &height_first, &nrChannels_first, 0);

  int width_second, height_second, nrChannels_second;
  stbi_set_flip_vertically_on_load(true);
  GLubyte *data_second = stbi_load("../textures/second.png", &width_second,
                                   &height_second, &nrChannels_second, 0);

  GLuint textures[2];
  glGenTextures(2, textures);

  // first
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_first, height_first, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_first);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data_first);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // second
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_second, height_second, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_second);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data_second);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  GLfloat vertices[] = {
      -0.9f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
      -0.1f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 
      -0.1f, 0.3f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 
      -0.9f, 0.3f, 0.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 
      0.2f, -0.1f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,      
      0.8f, -0.1f, 0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 0.0f,      
      0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.5f, 1.0f,     
  };

  GLuint indices[] = {
    0, 1, 2,
    0, 2, 3,
    4, 5, 6     
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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                        (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                        (void *)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

  glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);

  glfwSetScrollCallback(window, scroll_callback);

  // pętla zdarzeń
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.18f, 0.2f, 0.22f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    if (mode == 1) {
      glBindTexture(GL_TEXTURE_2D, textures[0]);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    if (mode == 2) {
      glBindTexture(GL_TEXTURE_2D, textures[1]);
      glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(6 * sizeof(GLuint)));
    }

    if (mode == 3) {
      glBindTexture(GL_TEXTURE_2D, textures[0]);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

      glBindTexture(GL_TEXTURE_2D, textures[1]);
      glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(6 * sizeof(GLuint)));
    }

    int scrollLocation = glGetUniformLocation(shaderProgram, "uniScroll");
    glUniform1f(scrollLocation, scrollOffset);

    glfwSwapBuffers(window);
    glfwPollEvents();

    processInput(window);
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}