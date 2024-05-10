#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

const GLchar *vertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec2 texture;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec2 vertexTexture;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * view * model * vec4(position, 1.0);\n"
    "    vertexTexture = texture;\n"
    "}\0";

const GLchar *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 fragmentColor;\n"
    "in vec2 vertexTexture;\n"
    "uniform sampler2D uniTexture;\n"
    "void main()\n"
    "{\n"
    "    fragmentColor = texture(uniTexture, vertexTexture);\n"
    "}\0";


const unsigned int window_width = 1000;
const unsigned int window_height = 800;

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 2.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  static bool first_window_enter = true;

  static float pitch = 0.0f;
  static float yaw = -90.0f;

  static float previousX = static_cast<float>(window_width)/2.0f;
  static float previousY = static_cast<float>(window_height)/2.0f;

  if (first_window_enter) {
    previousX = xpos;
    previousY = ypos;
    first_window_enter = false;
  }

  float xdifference = xpos - previousX;
  float ydifference = previousY - ypos;

  previousX = xpos;
  previousY = ypos;

  const float sensitivity = 0.075f;
  xdifference *= sensitivity;
  ydifference *= sensitivity;

  pitch += ydifference;
  yaw += xdifference;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 cameraFront_new;
  cameraFront_new.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront_new.y = sin(glm::radians(pitch));
  cameraFront_new.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(cameraFront_new);
}

int main() {
  // inicjalizacja GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Tworzenie okna
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

  // shadery
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  glEnable(GL_DEPTH_TEST);

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
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  GLubyte *data_first = stbi_load("../textures/first.png", &width,
                                  &height, &nrChannels, 0);

  GLuint textures[1];
  glGenTextures(1, textures);

  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_first);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data_first);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


  GLfloat vertices[] = {
      // vertices coords     // texture coords
      -0.5f, -0.5f, -0.5f,    0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,    1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,    1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,    1.0f,  1.0f,
      -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f,    0.0f,  0.0f,

      -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,    1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,    1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,    1.0f,  1.0f,
      -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,
      -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,

      -0.5f,  0.5f,  0.5f,    1.0f,  0.0f,
      -0.5f,  0.5f, -0.5f,    1.0f,  1.0f,
      -0.5f, -0.5f, -0.5f,    0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f,    0.0f,  1.0f,
      -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,
      -0.5f,  0.5f,  0.5f,    1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,    1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,    1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,    0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,    0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,    0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,    1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f,    0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,    1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,    1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,    1.0f,  0.0f,
      -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f,    0.0f,  1.0f,

      -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,    1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,    1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,    1.0f,  0.0f,
      -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,
      -0.5f,  0.5f, -0.5f,    0.0f,  1.0f
  };

  
  GLuint indices[] = {
    0, 1, 2, 3, 4, 5, 
    6, 7, 8, 9, 10, 11, 
    12, 13, 14, 15, 16, 17, 
    18, 19, 20, 21, 22, 23, 
    24, 25, 26, 27, 28, 29, 
    30, 31, 32, 33, 34, 35
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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                        (void *)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);

  GLint viewLoc = glGetUniformLocation(shaderProgram, "view");

  GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
  glUseProgram(shaderProgram);

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 100.0f);

  GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouseCallback);

  glfwSwapInterval(0);
  glfwSetTime(0.0);

  float deltaTime = 0.0f;
  float previousTime = 0.0f;
  float titleUpdateTime = 0.0f;

  float totalRotation = 0.0f;
  // pętla zdarzeń
  while (!glfwWindowShouldClose(window)) {
    float currentTime = glfwGetTime();

    deltaTime = currentTime - previousTime;
    previousTime = currentTime;

    totalRotation += 45.0f * deltaTime;

    const float cameraSpeed = 2.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cameraPosition += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cameraPosition -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (currentTime - titleUpdateTime >= 1.0f) {
      glfwSetWindowTitle(window, ("FPS: " + std::to_string(1.0f / deltaTime) + " Frame time: " + std::to_string(deltaTime*1000.0f) + "ms").c_str());
      titleUpdateTime = currentTime;
    }
    // renderowanie
    glClearColor(0.066f, 0.09f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(totalRotation), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //
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