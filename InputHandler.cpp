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

// Procesar los comandos de entrada durante el estado de juego || Process input commands during gameplay state
void processGameplayInput(GLFWwindow* window, bool& flashlightEnabled)
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
}

// Procesar la navegacion del usuario dentro de la interfaz || Process user navigation inside the interface
void processMenuInput(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool& hitboxDebug)
{
    static bool escWasPressed = false;
    static bool upWasPressed = false;
    static bool downWasPressed = false;
    static bool enterWasPressed = false;

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
    bool enterPressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (upPressed && !upWasPressed) menu.MoveSelection(-1);
    if (downPressed && !downWasPressed) menu.MoveSelection(1);

    // Procesar las acciones correspondientes al presionar enter || Process corresponding actions upon pressing enter
    if (enterPressed && !enterWasPressed)
    {
        if (currentMenuScreen == MenuScreen::Main)
        {
            // Acciones del menu principal || Main menu actions
            switch (menu.GetSelectedIndex())
            {
            case 0:
                gameStarted = true;
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
            // Acciones del menu de configuraciones || Settings menu actions
            int selectedIndex = menu.GetSelectedIndex();
            switch (selectedIndex)
            {
            case 0:
                sound.ToggleAmbient();
                ShowSettingsMenu(menu, sound, hitboxDebug);
                menu.SetSelectedIndex(selectedIndex);
                break;
            case 1:
                ShowMainMenu(menu);
                break;
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

    upWasPressed = upPressed;
    downWasPressed = downPressed;
    enterWasPressed = enterPressed;
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
