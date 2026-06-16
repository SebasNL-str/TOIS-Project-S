#include "MenuController.h"

// Vinculacion externa de variables de control global || External linking of global control variables
extern bool gameStarted;
extern bool menuOpen;
extern bool firstMouse;
extern MenuScreen currentMenuScreen;

// Textos editables del menu || Editable menu texts
namespace MenuContent
{
    const char* MainTitle = "T.O.I.S: PROJECT S";
    const char* MainSubtitle = "MAIN MENU";
    const char* SettingsTitle = "SETTINGS";
    const char* HelpTitle = "HELP";
    const char* CreditsTitle = "CREDITS";

    const char* SelectFooter = "ESC: MENU   ENTER: SELECT";
    const char* BackFooter = "ESC: BACK   ENTER: SELECT";
    const char* ToggleFooter = "ENTER: SWITCH   ESC: BACK";

    const char* StartLabel = "START TOUR";
    const char* ContinueLabel = "CONTINUE";
    const char* SettingsLabel = "SETTINGS";
    const char* HelpLabel = "HELP";
    const char* CreditsLabel = "CREDITS";
    const char* ExitLabel = "EXIT";
    const char* BackLabel = "BACK";

    const char* AudioOnLabel = "AUDIO: ON";
    const char* AudioOffLabel = "AUDIO: OFF";

    const std::vector<std::string> HelpItems = {
        "W A S D - MOVE",
        "MOUSE - LOOK",
        "F - FLASHLIGHT",
        "ESC - MENU",
        "ENTER - SELECT",
        BackLabel
    };

    const std::vector<std::string> CreditItems = {
        "TOIS PROJECT S",
        "DEVELOPED BY",
        "BRENES RUEDA MICHAEL ISMAEL",
        "NARVAEZ LOPEZ JOSE SEBASTIAN",
        "ORTIZ RODRIGEZ BRADLY ALEXANDER",
        "TECHNOLOGIES USED",
        "OPENGL GLFW ASSIMP GLEW",
        "GLM SOIL2 WINMM WIN32",
        BackLabel
    };
}

// Configurar y desplegar la interfaz del menu principal || Configure and display the main menu interface
void ShowMainMenu(MenuRenderer& menu)
{
    currentMenuScreen = MenuScreen::Main;
    menu.SetTitle(MenuContent::MainTitle);
    menu.SetSubtitle(MenuContent::MainSubtitle);
    menu.SetFooter(MenuContent::SelectFooter);

    // Cambiar dinamicamente la etiqueta de inicio || Dynamically change the start label
    menu.SetItems({
        gameStarted ? MenuContent::ContinueLabel : MenuContent::StartLabel,
        MenuContent::SettingsLabel,
        MenuContent::HelpLabel,
        MenuContent::CreditsLabel,
        MenuContent::ExitLabel
        });
    menu.SetSelectedIndex(0);
}

// Configurar y desplegar la interfaz del menu de opciones || Configure and display the settings menu interface
void ShowSettingsMenu(MenuRenderer& menu, const SoundManager& sound, bool hitboxDebug)
{
    currentMenuScreen = MenuScreen::Settings;
    menu.SetTitle(MenuContent::SettingsTitle);
    menu.SetSubtitle(MenuContent::SettingsLabel);
    menu.SetFooter(MenuContent::ToggleFooter);

    // Mostrar el estado actual de las configuraciones || Show current status of settings
    menu.SetItems({
        sound.IsAmbientEnabled() ? MenuContent::AudioOnLabel : MenuContent::AudioOffLabel,
        MenuContent::BackLabel
        });
}

// Configurar y desplegar la seccion de ayuda || Configure and display help section
void ShowHelpMenu(MenuRenderer& menu)
{
    currentMenuScreen = MenuScreen::Help;
    menu.SetTitle(MenuContent::HelpTitle);
    menu.SetSubtitle("CONTROLS");
    menu.SetFooter(MenuContent::BackFooter);
    menu.SetItems(MenuContent::HelpItems);
    menu.SetSelectedIndex(0);
}

// Configurar y desplegar la seccion de creditos || Configure and display credits section
void ShowCreditsMenu(MenuRenderer& menu)
{
    currentMenuScreen = MenuScreen::Credits;
    menu.SetTitle(MenuContent::CreditsTitle);
    menu.SetSubtitle("TEAM");
    menu.SetFooter(MenuContent::BackFooter);
    menu.SetItems(MenuContent::CreditItems);
    menu.SetSelectedIndex(0);
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
