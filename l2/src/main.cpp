#include <glad/glad.h> // musi być dołączony jako pierwszy
#include <GLFW/glfw3.h>

#include <iostream>

int main() {
  // inicjalizacja GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Tworzenie okna
  const unsigned int window_width = 1000;
  const unsigned int window_height = 800;
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

  glViewport(0, 0, (GLuint)window_width, (GLuint)window_height);

  // pętla zdarzeń
  while (!glfwWindowShouldClose(window)) {
    // renderowanie
    glClearColor(0.298f, 0.141f, 0.141f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}