#ifndef MENU_H
#define MENU_H

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm.hpp>
#include <SOIL2/SOIL2.h>

#include "Shader.h"

struct MenuSettings
{
    std::string title = "TOIS PROJECT S";
    std::string subtitle = "MENU PRINCIPAL";
    std::string footer = "ESC: MENU  ENTER: SELECCIONAR";
    std::vector<std::string> items = {
        "JUGAR",
        "CONFIGURACIONES",
        "SALIR"
    };

    glm::vec4 backgroundColor = glm::vec4(0.03f, 0.03f, 0.04f, 1.0f);
    glm::vec4 panelColor = glm::vec4(0.08f, 0.08f, 0.10f, 0.92f);
    glm::vec4 titleColor = glm::vec4(0.85f, 0.78f, 0.55f, 1.0f);
    glm::vec4 textColor = glm::vec4(0.88f, 0.88f, 0.86f, 1.0f);
    glm::vec4 selectedTextColor = glm::vec4(0.05f, 0.05f, 0.06f, 1.0f);
    glm::vec4 selectedColor = glm::vec4(0.85f, 0.78f, 0.55f, 0.95f);
    glm::vec4 footerColor = glm::vec4(0.62f, 0.62f, 0.60f, 1.0f);

    GLfloat panelWidth = 430.0f;
    GLfloat titleScale = 4.0f;
    GLfloat subtitleScale = 2.0f;
    GLfloat itemScale = 2.5f;
    GLfloat footerScale = 1.5f;
    bool visible = true;
    bool useBackgroundImage = true;
    std::string backgroundImagePath = "Resources/MenuBackground/menu.jpg";
};

class MenuRenderer
{
public:
    MenuRenderer() :
        shader("Resources/Shaders/menu.vs", "Resources/Shaders/menu.frag"),
        VAO(0),
        VBO(0),
        backgroundVAO(0),
        backgroundVBO(0),
        backgroundTexture(0),
        width(1.0f),
        height(1.0f),
        selectedIndex(0),
        backgroundLoadAttempted(false),
        backgroundTextureLoaded(false)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));

        glBindVertexArray(0);

        glGenVertexArrays(1, &backgroundVAO);
        glGenBuffers(1, &backgroundVBO);

        glBindVertexArray(backgroundVAO);
        glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(2 * sizeof(GLfloat)));

        glBindVertexArray(0);

        BuildFont();
    }

    ~MenuRenderer()
    {
        Destroy();
    }

    void Destroy()
    {
        if (VBO != 0)
        {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }

        if (VAO != 0)
        {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }

        if (backgroundVBO != 0)
        {
            glDeleteBuffers(1, &backgroundVBO);
            backgroundVBO = 0;
        }

        if (backgroundVAO != 0)
        {
            glDeleteVertexArrays(1, &backgroundVAO);
            backgroundVAO = 0;
        }

        if (backgroundTexture != 0)
        {
            glDeleteTextures(1, &backgroundTexture);
            backgroundTexture = 0;
        }
    }

    void Configure(const MenuSettings& newSettings)
    {
        bool backgroundChanged = settings.backgroundImagePath != newSettings.backgroundImagePath ||
            settings.useBackgroundImage != newSettings.useBackgroundImage;
        settings = newSettings;
        if (backgroundChanged)
        {
            ResetBackgroundTexture();
        }
        NormalizeSelection();
    }

    MenuSettings& EditSettings()
    {
        return settings;
    }

    const MenuSettings& GetSettings() const
    {
        return settings;
    }

    void SetTitle(const std::string& title)
    {
        settings.title = title;
    }

    void SetSubtitle(const std::string& subtitle)
    {
        settings.subtitle = subtitle;
    }

    void SetFooter(const std::string& footer)
    {
        settings.footer = footer;
    }

    void SetBackgroundColor(const glm::vec4& color)
    {
        settings.backgroundColor = color;
        settings.useBackgroundImage = false;
    }

    void SetBackgroundImage(const std::string& path)
    {
        settings.backgroundImagePath = path;
        settings.useBackgroundImage = !path.empty();
        ResetBackgroundTexture();
    }

    void ClearBackgroundImage()
    {
        settings.backgroundImagePath.clear();
        settings.useBackgroundImage = false;
        ResetBackgroundTexture();
    }

    void SetItems(const std::vector<std::string>& items)
    {
        settings.items = items;
        NormalizeSelection();
    }

    void SetItemText(std::size_t index, const std::string& text)
    {
        if (index < settings.items.size())
        {
            settings.items[index] = text;
        }
    }

    void SetVisible(bool visible)
    {
        settings.visible = visible;
    }

    bool IsVisible() const
    {
        return settings.visible;
    }

    void SetSelectedIndex(int index)
    {
        selectedIndex = index;
        NormalizeSelection();
    }

    int GetSelectedIndex() const
    {
        return selectedIndex;
    }

    void MoveSelection(int direction)
    {
        if (settings.items.empty())
        {
            selectedIndex = 0;
            return;
        }

        int itemCount = static_cast<int>(settings.items.size());
        selectedIndex = (selectedIndex + direction + itemCount) % itemCount;
    }

    void Render(int screenWidth, int screenHeight)
    {
        if (!settings.visible)
        {
            return;
        }

        Begin(screenWidth, screenHeight);

        DrawBackground();

        GLfloat panelWidth = std::min(settings.panelWidth, width - 40.0f);
        if (panelWidth < 260.0f)
        {
            panelWidth = width - 20.0f;
        }

        GLfloat panelHeight = 220.0f + static_cast<GLfloat>(settings.items.size()) * 46.0f;
        panelHeight = std::min(panelHeight, height - 40.0f);

        GLfloat panelX = (width - panelWidth) * 0.5f;
        GLfloat panelY = (height - panelHeight) * 0.5f;

        DrawRect(panelX, panelY, panelWidth, panelHeight, settings.panelColor);

        DrawCenteredText(settings.title, panelX, panelY + 34.0f, panelWidth, settings.titleScale, settings.titleColor);
        DrawCenteredText(settings.subtitle, panelX, panelY + 82.0f, panelWidth, settings.subtitleScale, settings.textColor);

        GLfloat itemY = panelY + 130.0f;
        GLfloat itemHeight = 34.0f;
        GLfloat itemPadding = 28.0f;

        for (std::size_t i = 0; i < settings.items.size(); ++i)
        {
            GLfloat rowY = itemY + static_cast<GLfloat>(i) * 46.0f;
            bool selected = static_cast<int>(i) == selectedIndex;

            if (selected)
            {
                DrawRect(panelX + itemPadding, rowY - 8.0f, panelWidth - itemPadding * 2.0f, itemHeight, settings.selectedColor);
            }

            DrawCenteredText(settings.items[i], panelX, rowY, panelWidth, settings.itemScale, selected ? settings.selectedTextColor : settings.textColor);
        }

        DrawCenteredText(settings.footer, panelX, panelY + panelHeight - 40.0f, panelWidth, settings.footerScale, settings.footerColor);

        End();
    }

    void Begin(int screenWidth, int screenHeight)
    {
        width = static_cast<GLfloat>(screenWidth > 0 ? screenWidth : 1);
        height = static_cast<GLfloat>(screenHeight > 0 ? screenHeight : 1);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader.Use();
    }

    void End()
    {
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    void DrawRect(GLfloat x, GLfloat y, GLfloat w, GLfloat h, const glm::vec3& color)
    {
        DrawRect(x, y, w, h, glm::vec4(color, 1.0f));
    }

    void DrawRect(GLfloat x, GLfloat y, GLfloat w, GLfloat h, const glm::vec4& color)
    {
        std::vector<GLfloat> vertices;
        PushQuad(vertices, x, y, w, h);
        DrawVertices(vertices, color);
    }

    void DrawText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, const glm::vec3& color)
    {
        DrawText(text, x, y, scale, glm::vec4(color, 1.0f));
    }

    void DrawText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, const glm::vec4& color)
    {
        GLfloat cursorX = x;

        for (char rawChar : text)
        {
            char c = static_cast<char>(std::toupper(static_cast<unsigned char>(rawChar)));

            if (c == ' ')
            {
                cursorX += 6.0f * scale;
                continue;
            }

            auto glyph = font.find(c);
            if (glyph == font.end())
            {
                cursorX += 6.0f * scale;
                continue;
            }

            std::vector<GLfloat> vertices;

            for (int row = 0; row < 7; row++)
            {
                for (int col = 0; col < 5; col++)
                {
                    if (glyph->second[row][col] == '1')
                    {
                        PushQuad(vertices, cursorX + col * scale, y + row * scale, scale, scale);
                    }
                }
            }

            DrawVertices(vertices, color);
            cursorX += 6.0f * scale;
        }
    }

    GLfloat MeasureText(const std::string& text, GLfloat scale)
    {
        return static_cast<GLfloat>(text.size()) * 6.0f * scale;
    }

private:
    Shader shader;
    GLuint VAO;
    GLuint VBO;
    GLuint backgroundVAO;
    GLuint backgroundVBO;
    GLuint backgroundTexture;
    GLfloat width;
    GLfloat height;
    int selectedIndex;
    MenuSettings settings;
    std::string loadedBackgroundPath;
    bool backgroundLoadAttempted;
    bool backgroundTextureLoaded;
    std::map<char, std::vector<std::string>> font;

    void NormalizeSelection()
    {
        if (settings.items.empty())
        {
            selectedIndex = 0;
            return;
        }

        int lastIndex = static_cast<int>(settings.items.size()) - 1;
        if (selectedIndex < 0)
        {
            selectedIndex = 0;
        }
        else if (selectedIndex > lastIndex)
        {
            selectedIndex = lastIndex;
        }
    }

    void DrawCenteredText(const std::string& text, GLfloat x, GLfloat y, GLfloat areaWidth, GLfloat scale, const glm::vec4& color)
    {
        GLfloat textWidth = MeasureText(text, scale);
        GLfloat textX = x + (areaWidth - textWidth) * 0.5f;
        DrawText(text, textX, y, scale, color);
    }

    void DrawBackground()
    {
        if (settings.useBackgroundImage)
        {
            if (EnsureBackgroundTexture())
            {
                DrawBackgroundTexture();
                return;
            }
        }

        DrawRect(0.0f, 0.0f, width, height, settings.backgroundColor);
    }

    void ResetBackgroundTexture()
    {
        if (backgroundTexture != 0)
        {
            glDeleteTextures(1, &backgroundTexture);
            backgroundTexture = 0;
        }

        loadedBackgroundPath.clear();
        backgroundLoadAttempted = false;
        backgroundTextureLoaded = false;
    }

    bool EnsureBackgroundTexture()
    {
        if (backgroundTextureLoaded && loadedBackgroundPath == settings.backgroundImagePath)
        {
            return backgroundTexture != 0;
        }

        if (backgroundLoadAttempted && loadedBackgroundPath == settings.backgroundImagePath)
        {
            return backgroundTexture != 0;
        }

        ResetBackgroundTexture();
        loadedBackgroundPath = settings.backgroundImagePath;
        backgroundLoadAttempted = true;

        if (settings.backgroundImagePath.empty())
        {
            std::cout << "MENU::BACKGROUND_IMAGE_EMPTY - usa SetBackgroundImage(\"Resources/MenuBackground/menu.jpg\") o cambia MenuSettings::backgroundImagePath." << std::endl;
            return false;
        }

        std::ifstream imageFile(settings.backgroundImagePath.c_str(), std::ios::binary);
        if (!imageFile.good())
        {
            std::cout << "MENU::BACKGROUND_IMAGE_NOT_FOUND: " << settings.backgroundImagePath << std::endl;
            std::cout << "MENU::EXPECTED_PATH - ruta relativa al directorio desde donde corre el .exe. Ejemplo: Resources/MenuBackground/menu.jpg" << std::endl;
            std::cout << "MENU::SUPPORTED_FORMATS - png, jpg, jpeg, bmp, tga u otros formatos soportados por stb_image." << std::endl;
            return false;
        }
        imageFile.close();

        int imageWidth = 0;
        int imageHeight = 0;
        int imageChannels = 0;

        unsigned char* data = SOIL_load_image(settings.backgroundImagePath.c_str(), &imageWidth, &imageHeight, &imageChannels, SOIL_LOAD_RGBA);

        if (!data)
        {
            std::cout << "MENU::BACKGROUND_IMAGE_LOAD_FAILED: " << settings.backgroundImagePath << std::endl;
            return false;
        }

        glGenTextures(1, &backgroundTexture);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        free(data);

        backgroundTextureLoaded = true;
        std::cout << "MENU::BACKGROUND_IMAGE_LOADED: " << settings.backgroundImagePath
            << " (" << imageWidth << "x" << imageHeight << ", canales: " << imageChannels << ")" << std::endl;
        return true;
    }

    void DrawBackgroundTexture()
    {
        GLfloat vertices[] = {
            -1.0f,  1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 1.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f
        };

        shader.Use();
        glUniform1i(glGetUniformLocation(shader.ID, "useTexture"), GL_TRUE);
        glUniform1i(glGetUniformLocation(shader.ID, "uiTexture"), 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

        glBindVertexArray(backgroundVAO);
        glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(glGetUniformLocation(shader.ID, "useTexture"), GL_FALSE);
    }

    void PushQuad(std::vector<GLfloat>& vertices, GLfloat x, GLfloat y, GLfloat w, GLfloat h)
    {
        GLfloat left = (x / width) * 2.0f - 1.0f;
        GLfloat right = ((x + w) / width) * 2.0f - 1.0f;
        GLfloat top = 1.0f - (y / height) * 2.0f;
        GLfloat bottom = 1.0f - ((y + h) / height) * 2.0f;

        GLfloat quad[] = {
            left, top,
            left, bottom,
            right, bottom,
            left, top,
            right, bottom,
            right, top
        };

        vertices.insert(vertices.end(), quad, quad + 12);
    }

    void DrawVertices(const std::vector<GLfloat>& vertices, const glm::vec4& color)
    {
        if (vertices.empty())
        {
            return;
        }

        shader.Use();
        glUniform1i(glGetUniformLocation(shader.ID, "useTexture"), GL_FALSE);
        glUniform4f(glGetUniformLocation(shader.ID, "uiColor"), color.r, color.g, color.b, color.a);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 2));
        glBindVertexArray(0);
    }

    void BuildFont()
    {
        font['A'] = { "01110", "10001", "10001", "11111", "10001", "10001", "10001" };
        font['B'] = { "11110", "10001", "10001", "11110", "10001", "10001", "11110" };
        font['C'] = { "01111", "10000", "10000", "10000", "10000", "10000", "01111" };
        font['D'] = { "11110", "10001", "10001", "10001", "10001", "10001", "11110" };
        font['E'] = { "11111", "10000", "10000", "11110", "10000", "10000", "11111" };
        font['F'] = { "11111", "10000", "10000", "11110", "10000", "10000", "10000" };
        font['G'] = { "01111", "10000", "10000", "10011", "10001", "10001", "01111" };
        font['H'] = { "10001", "10001", "10001", "11111", "10001", "10001", "10001" };
        font['I'] = { "11111", "00100", "00100", "00100", "00100", "00100", "11111" };
        font['J'] = { "11111", "00010", "00010", "00010", "00010", "10010", "01100" };
        font['K'] = { "10001", "10010", "10100", "11000", "10100", "10010", "10001" };
        font['L'] = { "10000", "10000", "10000", "10000", "10000", "10000", "11111" };
        font['M'] = { "10001", "11011", "10101", "10101", "10001", "10001", "10001" };
        font['N'] = { "10001", "11001", "10101", "10011", "10001", "10001", "10001" };
        font['O'] = { "01110", "10001", "10001", "10001", "10001", "10001", "01110" };
        font['P'] = { "11110", "10001", "10001", "11110", "10000", "10000", "10000" };
        font['Q'] = { "01110", "10001", "10001", "10001", "10101", "10010", "01101" };
        font['R'] = { "11110", "10001", "10001", "11110", "10100", "10010", "10001" };
        font['S'] = { "01111", "10000", "10000", "01110", "00001", "00001", "11110" };
        font['T'] = { "11111", "00100", "00100", "00100", "00100", "00100", "00100" };
        font['U'] = { "10001", "10001", "10001", "10001", "10001", "10001", "01110" };
        font['V'] = { "10001", "10001", "10001", "10001", "10001", "01010", "00100" };
        font['W'] = { "10001", "10001", "10001", "10101", "10101", "10101", "01010" };
        font['X'] = { "10001", "10001", "01010", "00100", "01010", "10001", "10001" };
        font['Y'] = { "10001", "10001", "01010", "00100", "00100", "00100", "00100" };
        font['Z'] = { "11111", "00001", "00010", "00100", "01000", "10000", "11111" };
        font['0'] = { "01110", "10001", "10011", "10101", "11001", "10001", "01110" };
        font['1'] = { "00100", "01100", "00100", "00100", "00100", "00100", "01110" };
        font['2'] = { "01110", "10001", "00001", "00010", "00100", "01000", "11111" };
        font['3'] = { "11110", "00001", "00001", "01110", "00001", "00001", "11110" };
        font['4'] = { "10010", "10010", "10010", "11111", "00010", "00010", "00010" };
        font['5'] = { "11111", "10000", "10000", "11110", "00001", "00001", "11110" };
        font['6'] = { "01111", "10000", "10000", "11110", "10001", "10001", "01110" };
        font['7'] = { "11111", "00001", "00010", "00100", "01000", "01000", "01000" };
        font['8'] = { "01110", "10001", "10001", "01110", "10001", "10001", "01110" };
        font['9'] = { "01110", "10001", "10001", "01111", "00001", "00001", "11110" };
        font['-'] = { "00000", "00000", "00000", "11111", "00000", "00000", "00000" };
        font['>'] = { "10000", "01000", "00100", "00010", "00100", "01000", "10000" };
        font[':'] = { "00000", "00100", "00100", "00000", "00100", "00100", "00000" };
    }
};

#endif
