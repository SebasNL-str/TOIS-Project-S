// Project includes
#include "Menu.h"
#include "SoundManager.h"

// GLFW include
#include <GLFW/glfw3.h>

void processGameplayInput(GLFWwindow* window, bool& flashlightEnabled); // Entrada para la linterna || Flashlight input
void processMenuInput(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool& hitboxDebug); // Entradas para el menu || Menu inputs
void mouse_callback(GLFWwindow* window, double xpos, double ypos); // Entradas relacionadas al raton/mouse || Mouse related inputs