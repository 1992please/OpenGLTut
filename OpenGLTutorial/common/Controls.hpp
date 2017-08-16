#ifndef CONTROLS_HPP
#define CONTROLS_HPP

// Include GLFW
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>

void computeMatricesFromInputs(GLFWwindow* window);
glm::mat4 getProjectionMatrix();
glm::mat4 getViewMatrix();

#endif // CONTROLS_HPP
