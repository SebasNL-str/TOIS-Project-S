
#include "Menu.h"
#include <GLFW/glfw3.h>
#include "SoundManager.h"

enum class MenuScreen
{
    Main,
    Settings
};

// Vinculamos las funciones del menú con las globales de main
void ShowMainMenu(MenuRenderer& menu);
void ShowSettingsMenu(MenuRenderer& menu, const SoundManager& sound, bool hitboxDebug);
void SetMenuOpen(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool open);