#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

// std includes
#include <iostream>
#include <string>

#pragma comment(lib, "winmm.lib")

// Definiciones de tipos para la API de Windows || Type definitions for Windows API
typedef const char* LPCSTR;
typedef void* HMODULE;
typedef int BOOL;
typedef unsigned int DWORD;

// Importacion de la funcion nativa para reproducir audio || Import of the native function to play audio
extern "C" __declspec(dllimport) BOOL __stdcall PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);
extern "C" __declspec(dllimport) unsigned int __stdcall waveOutSetVolume(HMODULE hwo, DWORD dwVolume);

// Constantes de configuracion de la API de sonido || Sound API configuration constants
const DWORD SND_SYNC = 0x0000;
const DWORD SND_ASYNC = 0x0001;
const DWORD SND_NODEFAULT = 0x0002;
const DWORD SND_LOOP = 0x0008;
const DWORD SND_FILENAME = 0x00020000;

// Clase para la gestion de audio del sistema || Class for system audio management
class SoundManager
{
public:
    // Constructor con inicializacion de estados || Constructor with states initialization
    SoundManager(const std::string& ambientPath) :
        ambientPath(ambientPath),
        buttonHoverPath("Resources/Sound/button_hover.wav"),
        ambientEnabled(true),
        ambientPlaying(false),
        masterVolume(100)
    {
        ApplyMasterVolume();
    }

    // Reproducir el sonido ambiental || Play ambient sound
    void PlayAmbient()
    {
        if (masterVolume <= 0)
        {
            StopAmbient();
            return;
        }

        // Validar si la ruta esta vacia || Check if path is empty
        if (ambientPath.empty())
        {
            std::cout << "AUDIO::AMBIENT_PATH_EMPTY" << std::endl;
            return;
        }

        // Iniciar la reproduccion asincrona en bucle || Start asynchronous looped playback
        ApplyMasterVolume();
        BOOL started = PlaySoundA(ambientPath.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
        if (!started)
        {
            std::cout << "AUDIO::AMBIENT_NOT_PLAYED: " << ambientPath << std::endl;
            ambientPlaying = false;
            return;
        }

        ambientPlaying = true;
    }

    // Reproducir audio solo si esta permitido || Play audio only if allowed
    void PlayAmbientIfEnabled()
    {
        if (ambientEnabled)
        {
            PlayAmbient();
        }
    }

    // Reproducir el sonido de seleccion del menu || Play menu selection sound
    void PlayButtonHover()
    {
        if (masterVolume <= 0 || buttonHoverPath.empty())
        {
            return;
        }

        ApplyMasterVolume();
        PlaySoundA(buttonHoverPath.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    }

    // Detener el sonido ambiental por completo || Stop ambient sound completely
    void StopAmbient()
    {
        PlaySoundA(nullptr, nullptr, SND_ASYNC);
        ambientPlaying = false;
    }

    // Activar o desactivar el sonido ambiental || Enable or disable ambient sound
    void SetAmbientEnabled(bool enabled)
    {
        ambientEnabled = enabled;

        if (!ambientEnabled)
        {
            StopAmbient();
        }
    }

    // Alternar el estado del sonido ambiental || Toggle ambient sound state
    void ToggleAmbient()
    {
        SetAmbientEnabled(!ambientEnabled);
    }

    // Cambiar la ruta del archivo de audio || Change audio file path
    void SetAmbientPath(const std::string& path)
    {
        ambientPath = path;

        if (ambientPlaying)
        {
            PlayAmbient();
        }
    }

    // Obtener la ruta del sonido ambiental || Get ambient sound path
    const std::string& GetAmbientPath() const
    {
        return ambientPath;
    }

    // Cambiar la ruta editable del sonido de botones || Change editable button sound path
    void SetButtonHoverPath(const std::string& path)
    {
        buttonHoverPath = path;
    }

    // Obtener la ruta del sonido de botones || Get button hover sound path
    const std::string& GetButtonHoverPath() const
    {
        return buttonHoverPath;
    }

    // Cambiar volumen general en porcentaje || Change master volume percentage
    void SetMasterVolume(int volume)
    {
        if (volume < 0) volume = 0;
        if (volume > 100) volume = 100;

        masterVolume = volume;
        ambientEnabled = masterVolume > 0;
        ApplyMasterVolume();

        if (masterVolume <= 0)
        {
            StopAmbient();
        }
        else if (ambientPlaying)
        {
            PlayAmbient();
        }
    }

    int GetMasterVolume() const
    {
        return masterVolume;
    }

    // Comprobar si el sonido esta habilitado || Check if sound is enabled
    bool IsAmbientEnabled() const
    {
        return ambientEnabled && masterVolume > 0;
    }

    // Comprobar si el sonido se esta reproduciendo || Check if sound is playing
    bool IsAmbientPlaying() const
    {
        return ambientPlaying;
    }

private:
    void ApplyMasterVolume()
    {
        DWORD value = static_cast<DWORD>((masterVolume * 0xFFFF) / 100);
        DWORD stereoVolume = (value & 0xFFFF) | (value << 16);
        waveOutSetVolume(nullptr, stereoVolume);
    }

    std::string ambientPath;
    std::string buttonHoverPath;
    bool ambientEnabled;
    bool ambientPlaying;
    int masterVolume;
};

#endif

