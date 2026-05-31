#include "Menu.h"
#include <GLFW/glfw3.h>
#include "SoundManager.h"

void processGameplayInput(GLFWwindow* window, bool& flashlightEnabled);
void processMenuInput(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool& hitboxDebug);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);