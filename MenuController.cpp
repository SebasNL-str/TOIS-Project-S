#include "MenuController.h"

// Vinculación mediante extern a las variables de control de main.cpp
extern bool gameStarted;
extern bool menuOpen;
extern bool firstMouse;
extern MenuScreen currentMenuScreen;

void ShowMainMenu(MenuRenderer& menu)
{
    currentMenuScreen = MenuScreen::Main;
    menu.SetTitle("T.O.I.S: PROJECT S");
    menu.SetSubtitle("MAIN MENU");
    menu.SetFooter("ESC: MENU   ENTER: SELECT");
    menu.SetItems({
        gameStarted ? "CONTINUE" : "START TOUR",
        "SETTINGS",
        "EXIT"
        });
    menu.SetSelectedIndex(0);
}

void ShowSettingsMenu(MenuRenderer& menu, const SoundManager& sound, bool hitboxDebug)
{
    currentMenuScreen = MenuScreen::Settings;
    menu.SetTitle("SETTINGS");
    menu.SetSubtitle("SETTINGS");
    menu.SetFooter("ENTER: SWITCH   ESC: BACK");
    menu.SetItems({
        sound.IsAmbientEnabled() ? "AUDIO: ON" : "AUDIO: OFF",
        hitboxDebug ? "HITBOX: ON" : "HITBOX: OFF",
        "BACK"
        });
}

void SetMenuOpen(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool open)
{
    menuOpen = open;
    menu.SetVisible(open);
    firstMouse = true;

    if (open)
    {
        ShowMainMenu(menu);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        sound.StopAmbient();
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (gameStarted)
        {
            sound.PlayAmbientIfEnabled();
        }
    }
}