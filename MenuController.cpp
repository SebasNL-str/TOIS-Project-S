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
    const char* VolumeFooter = "LEFT/RIGHT: VOLUME   ESC: BACK";

    const char* StartLabel = "START TOUR";
    const char* ContinueLabel = "CONTINUE";
    const char* SettingsLabel = "SETTINGS";
    const char* HelpLabel = "HELP";
    const char* CreditsLabel = "CREDITS";
    const char* ExitLabel = "EXIT";
    const char* BackLabel = "BACK";

    std::string GetVolumeLabel(int volume)
    {
        return "VOLUME: <" + std::to_string(volume) + "%>";
    }

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

namespace
{
    void ApplyMenuVisualStyle(MenuRenderer& menu)
    {
        MenuSettings& settings = menu.EditSettings();
        settings.drawPanel = gameStarted;
        settings.useBackgroundImage = !gameStarted;
        settings.backgroundColor = gameStarted
            ? glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
            : glm::vec4(0.03f, 0.03f, 0.04f, 1.0f);
        settings.panelCenterXPercent = gameStarted ? 0.29f : 0.50f;
        settings.panelCenterYPercent = 0.50f;
        settings.panelColor = gameStarted
            ? glm::vec4(0.03f, 0.03f, 0.04f, 0.62f)
            : glm::vec4(0.08f, 0.08f, 0.10f, 0.0f);
    }
}

// Configurar y desplegar la interfaz del menu principal || Configure and display the main menu interface
void ShowMainMenu(MenuRenderer& menu)
{
    currentMenuScreen = MenuScreen::Main;
    ApplyMenuVisualStyle(menu);
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
    ApplyMenuVisualStyle(menu);
    menu.SetTitle(MenuContent::SettingsTitle);
    menu.SetSubtitle(MenuContent::SettingsLabel);
    menu.SetFooter(MenuContent::VolumeFooter);

    // Mostrar el estado actual de las configuraciones || Show current status of settings
    menu.SetItems({
        MenuContent::GetVolumeLabel(sound.GetMasterVolume()),
        MenuContent::BackLabel
        });
}

// Configurar y desplegar la seccion de ayuda || Configure and display help section
void ShowHelpMenu(MenuRenderer& menu)
{
    currentMenuScreen = MenuScreen::Help;
    ApplyMenuVisualStyle(menu);
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
    ApplyMenuVisualStyle(menu);
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
        if (!gameStarted && !menu.HasIntroAnimationPlayed())
        {
            menu.StartIntroAnimation();
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        sound.StopAmbient();
    }
    else
    {
        // Ocultar cursor y reanudar audio al cerrar el menu || Hide cursor and resume audio when closing menu
        menu.SkipIntroAnimation();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (gameStarted)
        {
            sound.PlayAmbientIfEnabled();
        }
    }
}
