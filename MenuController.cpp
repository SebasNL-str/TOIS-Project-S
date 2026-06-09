#include "MenuController.h"

// Vinculacion externa de variables de control global || External linking of global control variables
extern bool gameStarted;
extern bool menuOpen;
extern bool firstMouse;
extern MenuScreen currentMenuScreen;

// Configurar y desplegar la interfaz del menu principal || Configure and display the main menu interface
void ShowMainMenu(MenuRenderer& menu)
{
    currentMenuScreen = MenuScreen::Main;
    menu.SetTitle("T.O.I.S: PROJECT S");
    menu.SetSubtitle("MAIN MENU");
    menu.SetFooter("ESC: MENU   ENTER: SELECT");

    // Cambiar dinamicamente la etiqueta de inicio || Dynamically change the start label
    menu.SetItems({
        gameStarted ? "CONTINUE" : "START TOUR",
        "SETTINGS",
        "EXIT"
        });
    menu.SetSelectedIndex(0);
}

// Configurar y desplegar la interfaz del menu de opciones || Configure and display the settings menu interface
void ShowSettingsMenu(MenuRenderer& menu, const SoundManager& sound, bool hitboxDebug)
{
    currentMenuScreen = MenuScreen::Settings;
    menu.SetTitle("SETTINGS");
    menu.SetSubtitle("SETTINGS");
    menu.SetFooter("ENTER: SWITCH   ESC: BACK");

    // Mostrar el estado actual de las configuraciones || Show current status of settings
    menu.SetItems({
        sound.IsAmbientEnabled() ? "AUDIO: ON" : "AUDIO: OFF",
        hitboxDebug ? "HITBOX: ON" : "HITBOX: OFF",
        "BACK"
        });
}

// Alternar el estado de apertura y visibilidad del menu || Toggle menu opening and visibility state
void SetMenuOpen(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool open)
{
    menuOpen = open;
    menu.SetVisible(open);
    firstMouse = true;

    if (open)
    {
        // Activar cursor y detener audio al abrir el menu || Enable cursor and stop audio when opening menu
        ShowMainMenu(menu);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        sound.StopAmbient();
    }
    else
    {
        // Ocultar cursor y reanudar audio al cerrar el menu || Hide cursor and resume audio when closing menu
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (gameStarted)
        {
            sound.PlayAmbientIfEnabled();
        }
    }
}
