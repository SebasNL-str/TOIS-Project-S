#ifndef MENU_H
#define MENU_H

// std includes
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// GL includes
#include <GL/glew.h>
#include <glm.hpp>

// SOIL2 include
#include <SOIL2/SOIL2.h>

// Project include
#include "MenuAnimation.h"
#include "Shader.h"

struct MenuSettings
{
    // Textos del menu || Menu texts
    std::string title = "TOIS PROJECT S";
    std::string subtitle = "MENU PRINCIPAL";
    std::string footer = "ESC: MENU  ENTER: SELECCIONAR";
    std::vector<std::string> items = {
        "JUGAR",
        "CONFIGURACIONES",
        "SALIR"
    };

    // Colores de la interfaz || Interface colors
    glm::vec4 backgroundColor = glm::vec4(0.03f, 0.03f, 0.04f, 1.0f);
    glm::vec4 panelColor = glm::vec4(0.08f, 0.08f, 0.10f, 0.92f);
    glm::vec4 titleColor = glm::vec4(0.85f, 0.78f, 0.55f, 1.0f);
    glm::vec4 textColor = glm::vec4(0.88f, 0.88f, 0.86f, 1.0f);
    glm::vec4 selectedTextColor = glm::vec4(0.05f, 0.05f, 0.06f, 1.0f);
    glm::vec4 selectedColor = glm::vec4(0.85f, 0.78f, 0.55f, 0.95f);
    glm::vec4 footerColor = glm::vec4(0.62f, 0.62f, 0.60f, 1.0f);
    bool drawPanel = false;

    // Dimensiones y escalas visuales || Dimensions and visual scales
    GLfloat panelWidth = 430.0f;
    GLfloat panelCenterXPercent = 0.50f;
    GLfloat panelCenterYPercent = 0.50f;
    GLfloat titleScale = 4.0f;
    GLfloat subtitleScale = 2.0f;
    GLfloat itemScale = 2.5f;
    GLfloat footerScale = 1.5f;
    bool visible = false;
    bool onlyBackSelectable = false;
    bool useBackgroundImage = true;
    std::string backgroundImagePath = "Resources/MenuBackground/menu2.png";

    // Layout especial de dos columnas para la pantalla de creditos || Special two-column layout for the credits screen
    bool useCreditsLayout = false;
    std::string creditsLeftHeader = "DEVELOPED BY";
    std::vector<std::string> creditsLeftLines;
    std::string creditsRightHeader = "TECHNOLOGIES";
    std::vector<std::string> creditsRightLines;
    std::string creditsBottomText;
};

class MenuRenderer
{
public:
    // Constructor e inicializacion de variables || Constructor and variables initialization
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
        // Generar y configurar VAO/VBO del panel || Generate and configure panel VAO/VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));

        glBindVertexArray(0);

        // Generar y configurar VAO/VBO del fondo || Generate and configure background VAO/VBO
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

        // Inicializar fuente del texto || Initialize text font
        BuildFont();
    }

    // Destructor || Destructor
    ~MenuRenderer()
    {
        Destroy();
    }

    // Liberar recursos de OpenGL || Release OpenGL resources
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

    // Actualizar configuracion del menu || Update menu configuration
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

    // Editar configuracion de la animacion inicial || Edit intro animation configuration
    MenuIntroAnimationSettings& EditIntroAnimationSettings()
    {
        return introAnimation.EditSettings();
    }

    // Iniciar animacion inicial del menu || Start menu intro animation
    void StartIntroAnimation()
    {
        introAnimation.Start();
    }

    // Saltar animacion inicial y usar posicion final || Skip intro animation and use final position
    void SkipIntroAnimation()
    {
        introAnimation.Skip();
    }

    bool HasIntroAnimationPlayed() const
    {
        return introAnimation.HasPlayed();
    }


    // Obtener configuracion para editar || Get configuration to edit
    MenuSettings& EditSettings()
    {
        return settings;
    }

    // Obtener configuracion de solo lectura || Get read-only configuration
    const MenuSettings& GetSettings() const
    {
        return settings;
    }

    // Cambiar titulo del menu || Change menu title
    void SetTitle(const std::string& title)
    {
        settings.title = title;
    }

    // Cambiar subtitulo del menu || Change menu subtitle
    void SetSubtitle(const std::string& subtitle)
    {
        settings.subtitle = subtitle;
    }

    // Cambiar pie de pagina || Change footer
    void SetFooter(const std::string& footer)
    {
        settings.footer = footer;
    }

    // Cambiar color de fondo || Change background color
    void SetBackgroundColor(const glm::vec4& color)
    {
        settings.backgroundColor = color;
        settings.useBackgroundImage = false;
    }

    // Cambiar imagen de fondo || Change background image
    void SetBackgroundImage(const std::string& path)
    {
        settings.backgroundImagePath = path;
        settings.useBackgroundImage = !path.empty();
        ResetBackgroundTexture();
    }

    // Quitar imagen de fondo || Remove background image
    void ClearBackgroundImage()
    {
        settings.backgroundImagePath.clear();
        settings.useBackgroundImage = false;
        ResetBackgroundTexture();
    }

    // Configurar el contenido de la pantalla de creditos (dos columnas + texto inferior) || Configure credits screen content (two columns + bottom text)
    void SetCreditsContent(
        const std::string& leftHeader,
        const std::vector<std::string>& leftLines,
        const std::string& rightHeader,
        const std::vector<std::string>& rightLines,
        const std::string& bottomText)
    {
        settings.creditsLeftHeader = leftHeader;
        settings.creditsLeftLines = leftLines;
        settings.creditsRightHeader = rightHeader;
        settings.creditsRightLines = rightLines;
        settings.creditsBottomText = bottomText;
    }

    // Cambiar lista de opciones || Change items list
    void SetItems(const std::vector<std::string>& items)
    {
        settings.items = items;
        NormalizeSelection();
    }

    // Cambiar texto de una opcion por indice || Change item text by index
    void SetItemText(std::size_t index, const std::string& text)
    {
        if (index < settings.items.size())
        {
            settings.items[index] = text;
        }
    }

    // Cambiar visibilidad || Change visibility
    void SetVisible(bool visible)
    {
        settings.visible = visible;
    }

    // Comprobar si es visible || Check if visible
    bool IsVisible() const
    {
        return settings.visible;
    }

    // Forzar indice de seleccion || Force selection index
    void SetSelectedIndex(int index)
    {
        selectedIndex = index;
        NormalizeSelection();
    }

    // Obtener indice seleccionado || Get selected index
    int GetSelectedIndex() const
    {
        return selectedIndex;
    }

    // Mover seleccion arriba o abajo || Move selection up or down
    void MoveSelection(int direction)
    {
        if (settings.items.empty())
        {
            selectedIndex = 0;
            return;
        }

        int itemCount = static_cast<int>(settings.items.size());
        for (int step = 0; step < itemCount; ++step)
        {
            selectedIndex = (selectedIndex + direction + itemCount) % itemCount;
            if (IsSelectableItem(selectedIndex))
            {
                return;
            }
        }
    }

    // Obtener item bajo coordenadas del mouse || Get item under mouse coordinates
    int GetItemIndexAt(GLfloat mouseX, GLfloat mouseY)
    {
        if (settings.items.empty() || introAnimation.IsPlaying())
        {
            return -1;
        }

        MenuLayout layout = CalculateLayout();

        if (settings.useCreditsLayout)
        {
            // En creditos solo existe el item BACK, dibujado como boton centrado mas grande || In credits only the BACK item exists, drawn as a bigger centered button
            if (!IsSelectableItem(0))
            {
                return -1;
            }

            GLfloat textAreaWidth = layout.panelWidth - 56.0f;
            GLfloat backScale = GetFittedScale(settings.items[0], layout.creditsBackScale, textAreaWidth);
            GLfloat backTextWidth = MeasureText(settings.items[0], backScale);
            GLfloat boxPadding = 36.0f;
            GLfloat boxWidth = backTextWidth + boxPadding * 2.0f;
            GLfloat boxX = layout.panelX + (layout.panelWidth - boxWidth) * 0.5f;

            if (mouseX >= boxX && mouseX <= boxX + boxWidth &&
                mouseY >= layout.itemY - 9.0f && mouseY <= layout.itemY - 9.0f + layout.creditsBackBoxHeight)
            {
                return 0;
            }

            return -1;
        }

        for (std::size_t i = 0; i < settings.items.size(); ++i)
        {
            if (!IsSelectableItem(i))
            {
                continue;
            }

            GLfloat rowY = layout.itemY + static_cast<GLfloat>(i) * layout.itemSpacing;
            if (mouseX >= layout.panelX + layout.itemPadding &&
                mouseX <= layout.panelX + layout.panelWidth - layout.itemPadding &&
                mouseY >= rowY - 8.0f &&
                mouseY <= rowY - 8.0f + layout.itemHeight)
            {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    // Obtener el punto medio del texto de una opcion || Get item text midpoint
    GLfloat GetItemTextMidpointX(std::size_t index)
    {
        if (index >= settings.items.size())
        {
            return width * 0.5f;
        }

        MenuLayout layout = CalculateLayout();
        GLfloat textAreaWidth = layout.panelWidth - 56.0f;
        GLfloat fittedItemScale = GetFittedScale(settings.items[index], settings.itemScale, textAreaWidth);
        GLfloat textWidth = MeasureText(settings.items[index], fittedItemScale);
        return layout.panelX + (layout.panelWidth - textWidth) * 0.5f + textWidth * 0.5f;
    }

    // Obtener el punto medio del control de volumen dentro de su texto || Get volume control midpoint inside its text
    GLfloat GetVolumeControlMidpointX(std::size_t index)
    {
        if (index >= settings.items.size())
        {
            return width * 0.5f;
        }

        const std::string& item = settings.items[index];
        std::size_t controlStart = item.find('<');
        std::size_t controlEnd = item.find('>');
        if (controlStart == std::string::npos || controlEnd == std::string::npos || controlEnd <= controlStart)
        {
            return GetItemTextMidpointX(index);
        }

        MenuLayout layout = CalculateLayout();
        GLfloat textAreaWidth = layout.panelWidth - 56.0f;
        GLfloat fittedItemScale = GetFittedScale(item, settings.itemScale, textAreaWidth);
        GLfloat textWidth = MeasureText(item, fittedItemScale);
        GLfloat textX = layout.panelX + (layout.panelWidth - textWidth) * 0.5f;
        GLfloat controlX = textX + MeasureText(item.substr(0, controlStart), fittedItemScale);
        GLfloat controlWidth = MeasureText(item.substr(controlStart, controlEnd - controlStart + 1), fittedItemScale);
        return controlX + controlWidth * 0.5f;
    }

    // Renderizar el menu completo || Render the entire menu
    void Render(int screenWidth, int screenHeight)
    {
        // Salir si no es visible || Exit if not visible
        if (!settings.visible)
        {
            return;
        }

        // Iniciar configuracion grafica || Begin graphic configuration
        Begin(screenWidth, screenHeight);

        // Dibujar el fondo || Draw background
        DrawBackground();

        MenuLayout layout = CalculateLayout();

        // Dibujar titulo y subtitulo || Draw title and subtitle
        GLfloat panelX = layout.panelX;
        GLfloat panelY = layout.panelY;
        GLfloat panelWidth = layout.panelWidth;
        GLfloat panelHeight = layout.panelHeight;
        GLfloat textAreaWidth = panelWidth - 56.0f;
        GLfloat titleScale = GetFittedScale(settings.title, settings.titleScale, textAreaWidth);
        GLfloat subtitleScale = GetFittedScale(settings.subtitle, settings.subtitleScale, textAreaWidth);
        GLfloat footerScale = GetFittedScale(settings.footer, settings.footerScale, textAreaWidth);

        if (settings.drawPanel)
        {
            DrawRect(panelX, panelY, panelWidth, panelHeight, settings.panelColor);
        }

        DrawAnimatedCenteredText(MenuAnimatedElement::Title, 0, settings.title, panelX, panelY + 34.0f, panelWidth, titleScale, settings.titleColor, panelWidth, panelHeight);
        DrawAnimatedCenteredText(MenuAnimatedElement::Subtitle, 0, settings.subtitle, panelX, panelY + 82.0f, panelWidth, subtitleScale, settings.textColor, panelWidth, panelHeight);

        // Configurar posiciones de la lista || Configure list positions
        GLfloat itemY = layout.itemY;
        GLfloat itemHeight = layout.itemHeight;
        GLfloat itemPadding = layout.itemPadding;

        if (settings.useCreditsLayout)
        {
            // Dibujar layout especial de dos columnas para creditos || Draw special two-column credits layout
            DrawCreditsContent(layout, panelX, panelWidth, textAreaWidth);
        }
        else
        {
            // Dibujar cada opcion de la lista || Draw each item from the list
            for (std::size_t i = 0; i < settings.items.size(); ++i)
            {
                GLfloat rowY = itemY + static_cast<GLfloat>(i) * layout.itemSpacing;
                bool selectable = IsSelectableItem(i);
                bool selected = selectable && static_cast<int>(i) == selectedIndex;

                // Dibujar indicador si la opcion esta seleccionada || Draw indicator if item is selected
                if (selected && !introAnimation.IsPlaying())
                {
                    DrawRect(panelX + itemPadding, rowY - 8.0f, panelWidth - itemPadding * 2.0f, itemHeight, settings.selectedColor);
                }

                // Dibujar texto de la opcion || Draw item text
                GLfloat fittedItemScale = GetFittedScale(settings.items[i], settings.itemScale, textAreaWidth);
                DrawAnimatedCenteredText(MenuAnimatedElement::Item, i, settings.items[i], panelX, rowY, panelWidth, fittedItemScale, selected && !introAnimation.IsPlaying() ? settings.selectedTextColor : settings.textColor, panelWidth, panelHeight);
            }
        }

        // Dibujar pie de pagina || Draw footer
        DrawAnimatedCenteredText(MenuAnimatedElement::Footer, settings.items.size(), settings.footer, panelX, panelY + panelHeight - 40.0f, panelWidth, footerScale, settings.footerColor, panelWidth, panelHeight);

        // Restaurar estado grafico || Restore graphic state
        End();
    }


    void RenderOverlay(int screenWidth, int screenHeight, const glm::vec4& color)
    {
        Begin(screenWidth, screenHeight);
        DrawRect(0.0f, 0.0f, width, height, color);
        End();
    }

    // Configurar estados de OpenGL para el menu || Configure OpenGL states for the menu
    void Begin(int screenWidth, int screenHeight)
    {
        // Forzar dimensiones minimas validas || Force minimum valid dimensions
        width = static_cast<GLfloat>(screenWidth > 0 ? screenWidth : 1);
        height = static_cast<GLfloat>(screenHeight > 0 ? screenHeight : 1);

        // Desactivar profundidad y activar transparencia || Disable depth and enable blending
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Activar shader || Use shader
        shader.Use();
    }

    // Restaurar configuracion original de OpenGL || Restore original OpenGL configuration
    void End()
    {
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    // Dibujar rectangulo con color RGB || Draw rectangle with RGB color
    void DrawRect(GLfloat x, GLfloat y, GLfloat w, GLfloat h, const glm::vec3& color)
    {
        DrawRect(x, y, w, h, glm::vec4(color, 1.0f));
    }

    // Dibujar rectangulo con color RGBA || Draw rectangle with RGBA color
    void DrawRect(GLfloat x, GLfloat y, GLfloat w, GLfloat h, const glm::vec4& color)
    {
        std::vector<GLfloat> vertices;
        PushQuad(vertices, x, y, w, h);
        DrawVertices(vertices, color);
    }

    // Dibujar texto con color RGB || Draw text with RGB color
    void DrawText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, const glm::vec3& color)
    {
        DrawText(text, x, y, scale, glm::vec4(color, 1.0f));
    }

    // Dibujar texto caracter por caracter || Draw text character by character
    void DrawText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, const glm::vec4& color)
    {
        GLfloat cursorX = x;

        // Recorrer cada caracter de la cadena || Loop through each character of the string
        for (char rawChar : text)
        {
            // Convertir letra a mayuscula || Convert letter to uppercase
            char c = static_cast<char>(std::toupper(static_cast<unsigned char>(rawChar)));

            // Avanzar espacio en blanco || Advance blank space
            if (c == ' ')
            {
                cursorX += 6.0f * scale;
                continue;
            }

            // Buscar el caracter en el mapa de la fuente || Look up character in the font map
            auto glyph = font.find(c);
            if (glyph == font.end())
            {
                cursorX += 6.0f * scale;
                continue;
            }

            std::vector<GLfloat> vertices;

            // Procesar matriz del glifo (7 filas x 5 columnas) || Process glyph matrix (7 rows x 5 columns)
            for (int row = 0; row < 7; row++)
            {
                for (int col = 0; col < 5; col++)
                {
                    // Crear un pixel por cada bit activo || Create a pixel for each active bit
                    if (glyph->second[row][col] == '1')
                    {
                        PushQuad(vertices, cursorX + col * scale, y + row * scale, scale, scale);
                    }
                }
            }

            // Dibujar el caracter procesado || Draw the processed character
            DrawVertices(vertices, color);
            cursorX += 6.0f * scale;
        }
    }

    // Calcular el ancho total del texto en pixeles || Calculate total text width in pixels
    GLfloat MeasureText(const std::string& text, GLfloat scale) const
    {
        return static_cast<GLfloat>(text.size()) * 6.0f * scale;
    }

    GLfloat GetFittedScale(const std::string& text, GLfloat preferredScale, GLfloat maxWidth)
    {
        GLfloat measuredWidth = MeasureText(text, preferredScale);
        if (measuredWidth <= maxWidth || measuredWidth <= 0.0f)
        {
            return preferredScale;
        }

        return std::max(1.25f, preferredScale * (maxWidth / measuredWidth));
    }

private:
    struct MenuLayout
    {
        GLfloat panelX = 0.0f;
        GLfloat panelY = 0.0f;
        GLfloat panelWidth = 0.0f;
        GLfloat panelHeight = 0.0f;
        GLfloat itemY = 0.0f;
        GLfloat itemHeight = 34.0f;
        GLfloat itemPadding = 28.0f;
        GLfloat itemSpacing = 46.0f;

        // Campos exclusivos del layout de creditos || Credits-layout-only fields
        GLfloat creditsColumnsY = 0.0f;
        GLfloat creditsLineHeight = 24.0f;
        GLfloat creditsBottomTextY = 0.0f;
        GLfloat creditsBackScale = 0.0f;
        GLfloat creditsBackBoxHeight = 0.0f;
    };

    // Variables miembros privadas || Private member variables
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
    MenuIntroAnimation introAnimation;
    std::string loadedBackgroundPath;
    bool backgroundLoadAttempted;
    bool backgroundTextureLoaded;
    std::map<char, std::vector<std::string>> font;

    MenuLayout CalculateLayout()
    {
        GLfloat horizontalPadding = 96.0f;
        GLfloat desiredPanelWidth = settings.panelWidth;
        desiredPanelWidth = std::max(desiredPanelWidth, MeasureText(settings.title, settings.titleScale) + horizontalPadding);
        desiredPanelWidth = std::max(desiredPanelWidth, MeasureText(settings.subtitle, settings.subtitleScale) + horizontalPadding);
        desiredPanelWidth = std::max(desiredPanelWidth, MeasureText(settings.footer, settings.footerScale) + horizontalPadding);

        if (settings.useCreditsLayout)
        {
            // Columna izquierda + columna derecha + separacion entre ambas || Left column + right column + gap between them
            GLfloat columnGap = 130.0f;
            GLfloat leftColumnWidth = MeasureWidestLine(settings.creditsLeftHeader, settings.creditsLeftLines, settings.itemScale);
            GLfloat rightColumnWidth = MeasureWidestLine(settings.creditsRightHeader, settings.creditsRightLines, settings.itemScale);
            desiredPanelWidth = std::max(desiredPanelWidth, leftColumnWidth + rightColumnWidth + columnGap + horizontalPadding);
            desiredPanelWidth = std::max(desiredPanelWidth, MeasureText(settings.creditsBottomText, settings.itemScale) + horizontalPadding);
        }
        else
        {
            for (const std::string& item : settings.items)
            {
                desiredPanelWidth = std::max(desiredPanelWidth, MeasureText(item, settings.itemScale) + horizontalPadding);
            }
        }

        MenuLayout layout;
        layout.panelWidth = std::min(desiredPanelWidth, width - 40.0f);
        if (layout.panelWidth < 260.0f)
        {
            layout.panelWidth = width - 20.0f;
        }

        if (settings.useCreditsLayout)
        {
            // Altura de fila para el texto de creditos (cabecera y nombres) || Row height for credits text (header and names)
            layout.creditsLineHeight = 7.0f * settings.itemScale + 11.0f;

            std::size_t leftRows = settings.creditsLeftLines.size();
            std::size_t rightRows = settings.creditsRightLines.size();
            std::size_t columnRows = std::max(leftRows, rightRows);

            // 1 fila de cabecera + 1 fila de espacio + filas de nombres || 1 header row + 1 spacer row + name rows
            GLfloat columnsBlockHeight = (2.0f + static_cast<GLfloat>(columnRows)) * layout.creditsLineHeight;
            GLfloat bottomTextGap = 26.0f;

            // BACK: escala mas grande que el resto de creditos + mas espacio antes de el || BACK: bigger scale than the rest of the credits + more space before it
            layout.creditsBackScale = settings.itemScale * 1.35f;
            layout.creditsBackBoxHeight = 7.0f * layout.creditsBackScale + 18.0f;
            GLfloat backGap = 64.0f;
            GLfloat bottomPadding = 90.0f;

            layout.panelHeight = 130.0f + columnsBlockHeight + bottomTextGap + layout.creditsLineHeight + backGap + layout.creditsBackBoxHeight + bottomPadding;
            layout.panelHeight = std::min(layout.panelHeight, height - 40.0f);

            layout.creditsColumnsY = 130.0f + layout.creditsLineHeight;
            layout.creditsBottomTextY = layout.creditsColumnsY + columnsBlockHeight + bottomTextGap;
            layout.itemY = layout.creditsBottomTextY + layout.creditsLineHeight + backGap + 9.0f;
        }
        else
        {
            layout.panelHeight = 220.0f + static_cast<GLfloat>(settings.items.size()) * layout.itemSpacing;
            layout.panelHeight = std::min(layout.panelHeight, height - 40.0f);
        }

        glm::vec2 menuPosition(
            width * settings.panelCenterXPercent - layout.panelWidth * 0.5f,
            height * settings.panelCenterYPercent - layout.panelHeight * 0.5f);
        layout.panelX = std::max(20.0f, std::min(menuPosition.x, width - layout.panelWidth - 20.0f));
        layout.panelY = std::max(20.0f, std::min(menuPosition.y, height - layout.panelHeight - 20.0f));

        if (settings.useCreditsLayout)
        {
            // Re-anclar las posiciones verticales relativas a la Y final del panel || Re-anchor vertical positions to the final panel Y
            layout.creditsColumnsY += layout.panelY;
            layout.creditsBottomTextY += layout.panelY;
            layout.itemY += layout.panelY;
        }
        else
        {
            layout.itemY = layout.panelY + 130.0f;
        }

        return layout;
    }

    // Medir el ancho de la linea mas larga entre una cabecera y una lista de lineas || Measure the widest line between a header and a list of lines
    GLfloat MeasureWidestLine(const std::string& header, const std::vector<std::string>& lines, GLfloat scale)
    {
        GLfloat widest = MeasureText(header, scale);
        for (const std::string& line : lines)
        {
            widest = std::max(widest, MeasureText(line, scale));
        }
        return widest;
    }

    // Ajustar el indice seleccionado dentro de los limites de la lista || Adjust selected index within items list limits
    bool IsSelectableItem(std::size_t index) const
    {
        if (index >= settings.items.size() || settings.items[index].empty())
        {
            return false;
        }

        return !settings.onlyBackSelectable || settings.items[index] == "BACK";
    }

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

        if (!IsSelectableItem(static_cast<std::size_t>(selectedIndex)))
        {
            int itemCount = static_cast<int>(settings.items.size());
            for (int step = 0; step < itemCount; ++step)
            {
                selectedIndex = (selectedIndex + 1) % itemCount;
                if (IsSelectableItem(static_cast<std::size_t>(selectedIndex)))
                {
                    return;
                }
            }
        }
    }

    // Dibujar texto centrado horizontalmente en un area || Draw horizontally centered text in an area
    void DrawCenteredText(const std::string& text, GLfloat x, GLfloat y, GLfloat areaWidth, GLfloat scale, const glm::vec4& color)
    {
        GLfloat textWidth = MeasureText(text, scale);
        GLfloat textX = x + (areaWidth - textWidth) * 0.5f;
        DrawText(text, textX, y, scale, color);
    }

    // Dibujar texto centrado aplicando animacion por elemento || Draw centered text with per-element animation
    void DrawAnimatedCenteredText(
        MenuAnimatedElement element,
        std::size_t itemIndex,
        const std::string& text,
        GLfloat x,
        GLfloat y,
        GLfloat areaWidth,
        GLfloat scale,
        const glm::vec4& color,
        GLfloat menuWidth,
        GLfloat menuHeight)
    {
        glm::vec2 finalPosition(x + areaWidth * 0.5f, y);
        MenuElementTransform transform = introAnimation.GetElementTransform(
            element,
            itemIndex,
            finalPosition,
            scale,
            width,
            height,
            menuWidth,
            menuHeight);

        GLfloat animatedTextWidth = MeasureText(text, transform.scale);
        glm::vec4 animatedColor = color;
        animatedColor.a *= transform.opacity;

        if (animatedColor.a <= 0.01f)
        {
            return;
        }

        DrawText(text, transform.position.x - animatedTextWidth * 0.5f, transform.position.y, transform.scale, animatedColor);
    }

    // Dibujar el contenido de la pantalla de creditos: dos columnas + texto inferior + BACK como boton con hover || Draw credits screen content: two columns + bottom text + BACK as a hoverable button
    void DrawCreditsContent(const MenuLayout& layout, GLfloat panelX, GLfloat panelWidth, GLfloat textAreaWidth)
    {
        // Mismo columnGap usado en CalculateLayout para que el ancho del panel siempre alcance || Same columnGap used in CalculateLayout so the panel width always fits
        GLfloat columnGap = 130.0f;
        GLfloat leftX = panelX + 22.0f;
        GLfloat columnsAreaWidth = panelWidth - 44.0f - columnGap;
        GLfloat rightX = leftX + columnsAreaWidth * 0.5f + columnGap;
        GLfloat lineHeight = layout.creditsLineHeight;

        // Cabeceras de columna, resaltadas con el color del titulo || Column headers, highlighted with the title color
        GLfloat rowY = layout.creditsColumnsY;
        DrawText(settings.creditsLeftHeader, leftX, rowY, settings.itemScale, settings.titleColor);
        DrawText(settings.creditsRightHeader, rightX, rowY, settings.itemScale, settings.titleColor);

        // Saltar una fila de espacio entre la cabecera y los nombres || Skip one spacer row between header and names
        rowY += lineHeight * 2.0f;

        std::size_t maxRows = std::max(settings.creditsLeftLines.size(), settings.creditsRightLines.size());
        for (std::size_t row = 0; row < maxRows; ++row)
        {
            if (row < settings.creditsLeftLines.size())
            {
                DrawText(settings.creditsLeftLines[row], leftX, rowY, settings.itemScale, settings.textColor);
            }
            if (row < settings.creditsRightLines.size())
            {
                DrawText(settings.creditsRightLines[row], rightX, rowY, settings.itemScale, settings.textColor);
            }
            rowY += lineHeight;
        }

        // Texto inferior centrado, ej. el nombre de la universidad || Centered bottom text, e.g. the university name
        if (!settings.creditsBottomText.empty())
        {
            GLfloat bottomScale = GetFittedScale(settings.creditsBottomText, settings.itemScale, textAreaWidth);
            DrawCenteredText(settings.creditsBottomText, panelX, layout.creditsBottomTextY, panelWidth, bottomScale, settings.footerColor);
        }

        // BACK como boton: texto centrado mas grande, con caja de hover/seleccion detras || BACK as a button: bigger centered text, with a hover/selection box behind it
        if (!settings.items.empty())
        {
            bool selectable = IsSelectableItem(0);
            bool selected = selectable && selectedIndex == 0;
            GLfloat backScale = GetFittedScale(settings.items[0], layout.creditsBackScale, textAreaWidth);

            if (selected && !introAnimation.IsPlaying())
            {
                GLfloat backTextWidth = MeasureText(settings.items[0], backScale);
                GLfloat boxPadding = 36.0f;
                GLfloat boxWidth = backTextWidth + boxPadding * 2.0f;
                GLfloat boxX = panelX + (panelWidth - boxWidth) * 0.5f;
                DrawRect(boxX, layout.itemY - 9.0f, boxWidth, layout.creditsBackBoxHeight, settings.selectedColor);
            }

            glm::vec4 backColor = (selected && !introAnimation.IsPlaying()) ? settings.selectedTextColor : settings.textColor;
            DrawAnimatedCenteredText(MenuAnimatedElement::Item, 0, settings.items[0], panelX, layout.itemY, panelWidth, backScale, backColor, panelWidth, layout.panelHeight);
        }
    }

    // Dibujar el fondo del menu (imagen o color solido) || Draw menu background (image or solid color)
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

    // Liberar textura actual y limpiar variables de control || Release current texture and clear control variables
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

    // Asegurar la carga correcta de la imagen de fondo en OpenGL || Ensure correct background image loading into OpenGL
    bool EnsureBackgroundTexture()
    {
        // Validar si la textura ya se encuentra cargada || Validate if texture is already loaded
        if (backgroundTextureLoaded && loadedBackgroundPath == settings.backgroundImagePath)
        {
            return backgroundTexture != 0;
        }

        // Validar si ya se intento cargar la textura previamente || Validate if texture loading was already attempted
        if (backgroundLoadAttempted && loadedBackgroundPath == settings.backgroundImagePath)
        {
            return backgroundTexture != 0;
        }

        ResetBackgroundTexture();
        loadedBackgroundPath = settings.backgroundImagePath;
        backgroundLoadAttempted = true;

        // Comprobar si la ruta de la imagen esta vacia || Check if image path is empty
        if (settings.backgroundImagePath.empty())
        {
            std::cout << "MENU::BACKGROUND_IMAGE_EMPTY - usa SetBackgroundImage(\"Resources/MenuBackground/menu.jpg\") o cambia MenuSettings::backgroundImagePath." << std::endl;
            return false;
        }

        std::string resolvedBackgroundPath = ResolveAssetPath(settings.backgroundImagePath);

        // Verificar existencia fisica del archivo || Verify physical existence of the file
        if (resolvedBackgroundPath.empty())
        {
            std::cout << "MENU::BACKGROUND_IMAGE_NOT_FOUND: " << settings.backgroundImagePath << std::endl;
            std::cout << "MENU::EXPECTED_PATH - ruta relativa al directorio desde donde corre el .exe. Ejemplo: Resources/MenuBackground/menu.jpg, ../Resources/MenuBackground/menu.jpg o ../../Resources/MenuBackground/menu.jpg" << std::endl;
            std::cout << "MENU::SUPPORTED_FORMATS - png, jpg, jpeg, bmp, tga u otros formatos soportados por stb_image." << std::endl;
            return false;
        }

        int imageWidth = 0;
        int imageHeight = 0;
        int imageChannels = 0;

        // Cargar los pixeles del archivo con SOIL || Load file pixels using SOIL
        unsigned char* data = SOIL_load_image(resolvedBackgroundPath.c_str(), &imageWidth, &imageHeight, &imageChannels, SOIL_LOAD_RGBA);

        if (!data)
        {
            std::cout << "MENU::BACKGROUND_IMAGE_LOAD_FAILED: " << resolvedBackgroundPath << std::endl;
            return false;
        }

        // Generar y configurar textura bidimensional de OpenGL || Generate and configure OpenGL 2D texture
        glGenTextures(1, &backgroundTexture);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Liberar buffer temporal de pixeles || Free temporary pixels buffer
        free(data);

        backgroundTextureLoaded = true;
        std::cout << "MENU::BACKGROUND_IMAGE_LOADED: " << resolvedBackgroundPath
            << " (" << imageWidth << "x" << imageHeight << ", canales: " << imageChannels << ")" << std::endl;
        return true;
    }

    bool AssetExists(const std::string& path) const
    {
        std::ifstream file(path.c_str(), std::ios::binary);
        return file.good();
    }

    std::string ResolveAssetPath(const std::string& path) const
    {
        std::vector<std::string> candidates = {
            path,
            "../" + path,
            "../../" + path,
            "../../../" + path
        };

        for (const std::string& candidate : candidates)
        {
            if (AssetExists(candidate))
            {
                return candidate;
            }
        }

        return "";
    }


    // Renderizar la textura de fondo en la pantalla || Render the background texture on the screen
    void DrawBackgroundTexture()
    {
        // Coordenadas de los triangulos con mapeo UV || Triangle coordinates with UV mapping
        GLfloat vertices[] = {
            -1.0f,  1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 1.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f
        };

        // Activar shader y enviar variables uniformes || Activate shader and send uniform variables
        shader.Use();
        glUniform1i(glGetUniformLocation(shader.ID, "useTexture"), GL_TRUE);
        glUniform1i(glGetUniformLocation(shader.ID, "uiTexture"), 0);

        // Enlazar textura de fondo || Bind background texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

        // Enviar datos de vertices y dibujar triangulos || Send vertex data and draw triangles
        glBindVertexArray(backgroundVAO);
        glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Restaurar estado de la textura y shader || Restore texture and shader state
        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(glGetUniformLocation(shader.ID, "useTexture"), GL_FALSE);
    }

    // Convertir coordenadas de pixeles a espacio normalizado (NDC) || Convert pixel coordinates to normalized device coordinates (NDC)
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

        // Insertar los puntos en el arreglo de vertices || Insert points into the vertices array
        vertices.insert(vertices.end(), quad, quad + 12);
    }

    // Dibujar los vertices de los rectangulos generados || Draw the vertices of the generated rectangles
    void DrawVertices(const std::vector<GLfloat>& vertices, const glm::vec4& color)
    {
        if (vertices.empty())
        {
            return;
        }

        // Configurar shader para color plano || Configure shader for flat color
        shader.Use();
        glUniform1i(glGetUniformLocation(shader.ID, "useTexture"), GL_FALSE);
        glUniform4f(glGetUniformLocation(shader.ID, "uiColor"), color.r, color.g, color.b, color.a);

        // Cargar buffer dinamico y dibujar la geometria || Load dynamic buffer and draw geometry
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 2));
        glBindVertexArray(0);
    }

    // Definir mapas de bits para cada caracter de la fuente || Define bit maps for each font character
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
        font['<'] = { "00001", "00010", "00100", "01000", "00100", "00010", "00001" };
        font['>'] = { "10000", "01000", "00100", "00010", "00100", "01000", "10000" };
        font[':'] = { "00000", "00100", "00100", "00000", "00100", "00100", "00000" };
        font['/'] = { "00001", "00010", "00010", "00100", "01000", "01000", "10000" };
        font['%'] = { "11001", "11010", "00010", "00100", "01000", "01011", "10011" };
        font['['] = { "01110", "01000", "01000", "01000", "01000", "01000", "01110" };
        font[']'] = { "01110", "00010", "00010", "00010", "00010", "00010", "01110" };
        font['#'] = { "01010", "11111", "01010", "01010", "11111", "01010", "01010" };
    }
};

#endif