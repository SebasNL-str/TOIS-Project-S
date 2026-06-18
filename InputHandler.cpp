#include "InputHandler.h"
#include "Camera.h"
#include "MenuController.h"

// Vinculacion externa de variables para el control del estado global || External linking of variables for global state control
extern Camera camera;
extern float deltaTime;
extern bool gameStarted;
extern bool menuOpen;
extern bool firstMouse;
extern float lastX;
extern float lastY;
extern MenuScreen currentMenuScreen;
extern bool tourFadeActive;
extern float tourFadeOpacity;

namespace
{
    void RefreshSettingsMenu(MenuRenderer& menu, const SoundManager& sound, bool hitboxDebug, int selectedIndex)
    {
        ShowSettingsMenu(menu, sound, hitboxDebug);
        menu.SetSelectedIndex(selectedIndex);
    }

    void ActivateSelectedMenuItem(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool& hitboxDebug)
    {
        if (currentMenuScreen == MenuScreen::Main)
        {
            // Acciones del menu principal || Main menu actions
            switch (menu.GetSelectedIndex())
            {
            case 0:
                gameStarted = true;
                tourFadeActive = true;
                tourFadeOpacity = 1.0f;
                SetMenuOpen(window, menu, sound, false);
                break;
            case 1:
                ShowSettingsMenu(menu, sound, hitboxDebug);
                menu.SetSelectedIndex(0);
                break;
            case 2:
                ShowHelpMenu(menu);
                break;
            case 3:
                ShowCreditsMenu(menu);
                break;
            case 4:
                glfwSetWindowShouldClose(window, true);
                break;
            }
        }
        else if (currentMenuScreen == MenuScreen::Settings)
        {
            int selectedIndex = menu.GetSelectedIndex();
            if (selectedIndex == 1)
            {
                ShowMainMenu(menu);
            }
        }
        else if (currentMenuScreen == MenuScreen::Help || currentMenuScreen == MenuScreen::Credits)
        {
            if (menu.GetSelectedIndex() == static_cast<int>(menu.GetSettings().items.size()) - 1)
            {
                ShowMainMenu(menu);
            }
        }
    }
}

// Procesar los comandos de entrada durante el estado de juego || Process input commands during gameplay state
void processGameplayInput(GLFWwindow* window, bool& flashlightEnabled, bool& bloomEnabled)
{
    static bool fWasPressed = false;

    // Control de traslacion de la camara en primera persona || First person camera translation control
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // Deteccion por flanco de la tecla de linterna || Edge detection for the flashlight key
    bool fPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (fPressed && !fWasPressed)
    {
        flashlightEnabled = !flashlightEnabled;
    }
    fWasPressed = fPressed;

    // Dentro de tu función de procesamiento de teclado:
    static bool bKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        if (!bKeyPressed) {
            bloomEnabled = !bloomEnabled;
            std::cout << "Bloom: " << (bloomEnabled ? "ACTIVADO" : "DESACTIVADO") << std::endl;
            bKeyPressed = true;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
        bKeyPressed = false;
    }
}

// Procesar la navegacion del usuario dentro de la interfaz || Process user navigation inside the interface
void processMenuInput(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool& hitboxDebug)
{
    static bool escWasPressed = false;
    static bool upWasPressed = false;
    static bool downWasPressed = false;
    static bool leftWasPressed = false;
    static bool rightWasPressed = false;
    static bool enterWasPressed = false;
    static bool mouseWasPressed = false;

    // Gestion de la tecla de escape para volver o alternar el menu || Management of escape key to return or toggle the menu
    bool escPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    if (escPressed && !escWasPressed)
    {
        if (menuOpen && currentMenuScreen != MenuScreen::Main)
        {
            ShowMainMenu(menu);
        }
        else if (gameStarted)
        {
            SetMenuOpen(window, menu, sound, !menuOpen);
        }
        else
        {
            SetMenuOpen(window, menu, sound, true);
        }
    }
    escWasPressed = escPressed;

    if (!menuOpen) return;

    // Capturar flancos de navegacion y seleccion de la interfaz || Capture navigation and selection edges of the interface
    bool upPressed = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool downPressed = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool leftPressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool rightPressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    bool enterPressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    int previousSelection = menu.GetSelectedIndex();
    if (upPressed && !upWasPressed) menu.MoveSelection(-1);
    if (downPressed && !downWasPressed) menu.MoveSelection(1);
    if (menu.GetSelectedIndex() != previousSelection)
    {
        sound.PlayButtonHover();
    }

    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    int hoveredIndex = menu.GetItemIndexAt(static_cast<GLfloat>(mouseX), static_cast<GLfloat>(mouseY));
    if (hoveredIndex >= 0 && hoveredIndex != menu.GetSelectedIndex())
    {
        menu.SetSelectedIndex(hoveredIndex);
        sound.PlayButtonHover();
    }

    if (currentMenuScreen == MenuScreen::Settings && menu.GetSelectedIndex() == 0)
    {
        int volumeDirection = 0;
        if (leftPressed && !leftWasPressed) volumeDirection -= 1;
        if (rightPressed && !rightWasPressed) volumeDirection += 1;

        if (volumeDirection != 0)
        {
            sound.SetMasterVolume(sound.GetMasterVolume() + volumeDirection * 10);
            RefreshSettingsMenu(menu, sound, hitboxDebug, 0);
            sound.PlayButtonHover();
        }
    }

    // Procesar las acciones correspondientes al presionar enter || Process corresponding actions upon pressing enter
    if (enterPressed && !enterWasPressed)
    {
        ActivateSelectedMenuItem(window, menu, sound, hitboxDebug);
    }

    if (mousePressed && !mouseWasPressed && hoveredIndex >= 0)
    {
        menu.SetSelectedIndex(hoveredIndex);
        if (currentMenuScreen == MenuScreen::Settings && hoveredIndex == 0)
        {
            int volumeDirection = mouseX < static_cast<double>(menu.GetVolumeControlMidpointX(0)) ? -1 : 1;
            sound.SetMasterVolume(sound.GetMasterVolume() + volumeDirection * 10);
            RefreshSettingsMenu(menu, sound, hitboxDebug, 0);
            sound.PlayButtonHover();
        }
        else
        {
            ActivateSelectedMenuItem(window, menu, sound, hitboxDebug);
        }
    }

    upWasPressed = upPressed;
    downWasPressed = downPressed;
    leftWasPressed = leftPressed;
    rightWasPressed = rightPressed;
    enterWasPressed = enterPressed;
    mouseWasPressed = mousePressed;
}

// Funcion callback para el calculo de la orientacion del raton || Callback function for mouse orientation calculation
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Ignorar si el menu esta activo o el juego no ha iniciado || Ignore if menu is active or game has not started
    if (menuOpen || !gameStarted)
    {
        firstMouse = true;
        return;
    }

    // Inicializar coordenadas en el primer movimiento || Initialize coordinates on first movement
    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    // Calcular diferencias de desplazamiento relativo || Calculate relative displacement offsets
    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    // Enviar deltas para actualizar rotaciones de la camara || Send deltas to update camera rotations
    camera.ProcessMouseMovement(xoffset, yoffset);
}
