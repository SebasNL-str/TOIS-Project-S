#pragma once
// Project includes
#include "Menu.h"
#include "SoundManager.h"

// GLFW include
#include <GLFW/glfw3.h>


// Pantallas disponibles en el menu || Available menu screens
enum class MenuScreen
{
    Main,
    Settings,
    Help,
    Credits
};

// Declaracion de funciones de control del menu || Declaration of menu control functions
void ShowMainMenu(MenuRenderer& menu);
void ShowSettingsMenu(MenuRenderer& menu, const SoundManager& sound, bool hitboxDebug);
void ShowHelpMenu(MenuRenderer& menu);
void ShowCreditsMenu(MenuRenderer& menu);
void SetMenuOpen(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool open);
