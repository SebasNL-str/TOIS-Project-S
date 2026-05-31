#include "InputHandler.h"
#include "Camera.h"
#include "MenuController.h"

// Traemos las globales necesarias para procesar movimientos de cámara y menús
extern Camera camera;
extern float deltaTime;
extern bool gameStarted;
extern bool menuOpen;
extern bool firstMouse;
extern float lastX;
extern float lastY;
extern MenuScreen currentMenuScreen;

void processGameplayInput(GLFWwindow* window, bool& flashlightEnabled)
{
    static bool fWasPressed = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    bool fPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
    if (fPressed && !fWasPressed)
    {
        flashlightEnabled = !flashlightEnabled;
    }
    fWasPressed = fPressed;
}

void processMenuInput(GLFWwindow* window, MenuRenderer& menu, SoundManager& sound, bool& hitboxDebug)
{
    static bool escWasPressed = false;
    static bool upWasPressed = false;
    static bool downWasPressed = false;
    static bool enterWasPressed = false;

    bool escPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    if (escPressed && !escWasPressed)
    {
        if (menuOpen && currentMenuScreen == MenuScreen::Settings)
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

    bool upPressed = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    bool downPressed = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    bool enterPressed = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (upPressed && !upWasPressed) menu.MoveSelection(-1);
    if (downPressed && !downWasPressed) menu.MoveSelection(1);

    if (enterPressed && !enterWasPressed)
    {
        if (currentMenuScreen == MenuScreen::Main)
        {
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
                glfwSetWindowShouldClose(window, true);
                break;
            }
        }
        else if (currentMenuScreen == MenuScreen::Settings)
        {
            int selectedIndex = menu.GetSelectedIndex();
            switch (selectedIndex)
            {
            case 0:
                sound.ToggleAmbient();
                ShowSettingsMenu(menu, sound, hitboxDebug);
                menu.SetSelectedIndex(selectedIndex);
                break;
            case 1:
                hitboxDebug = !hitboxDebug;
                ShowSettingsMenu(menu, sound, hitboxDebug);
                menu.SetSelectedIndex(selectedIndex);
                break;
            case 2:
                ShowMainMenu(menu);
                break;
            }
        }
    }

    upWasPressed = upPressed;
    downWasPressed = downPressed;
    enterWasPressed = enterPressed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (menuOpen || !gameStarted)
    {
        firstMouse = true;
        return;
    }

    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(xoffset, yoffset);
}