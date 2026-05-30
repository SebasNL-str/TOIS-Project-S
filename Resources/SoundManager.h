#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <iostream>
#include <string>

#pragma comment(lib, "winmm.lib")

typedef const char* LPCSTR;
typedef void* HMODULE;
typedef int BOOL;
typedef unsigned int DWORD;

extern "C" __declspec(dllimport) BOOL __stdcall PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);

const DWORD SND_SYNC = 0x0000;
const DWORD SND_ASYNC = 0x0001;
const DWORD SND_NODEFAULT = 0x0002;
const DWORD SND_LOOP = 0x0008;
const DWORD SND_FILENAME = 0x00020000;

class SoundManager
{
public:
    SoundManager(const std::string& ambientPath) :
        ambientPath(ambientPath),
        ambientEnabled(true),
        ambientPlaying(false)
    {
    }

    void PlayAmbient()
    {
        if (ambientPath.empty())
        {
            std::cout << "AUDIO::AMBIENT_PATH_EMPTY" << std::endl;
            return;
        }

        BOOL started = PlaySoundA(ambientPath.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
        if (!started)
        {
            std::cout << "AUDIO::AMBIENT_NOT_PLAYED: " << ambientPath << std::endl;
            ambientPlaying = false;
            return;
        }

        ambientPlaying = true;
    }

    void PlayAmbientIfEnabled()
    {
        if (ambientEnabled)
        {
            PlayAmbient();
        }
    }

    void StopAmbient()
    {
        PlaySoundA(nullptr, nullptr, SND_ASYNC);
        ambientPlaying = false;
    }

    void SetAmbientEnabled(bool enabled)
    {
        ambientEnabled = enabled;

        if (!ambientEnabled)
        {
            StopAmbient();
        }
    }

    void ToggleAmbient()
    {
        SetAmbientEnabled(!ambientEnabled);
    }

    void SetAmbientPath(const std::string& path)
    {
        ambientPath = path;

        if (ambientPlaying)
        {
            PlayAmbient();
        }
    }

    const std::string& GetAmbientPath() const
    {
        return ambientPath;
    }

    bool IsAmbientEnabled() const
    {
        return ambientEnabled;
    }

    bool IsAmbientPlaying() const
    {
        return ambientPlaying;
    }

private:
    std::string ambientPath;
    bool ambientEnabled;
    bool ambientPlaying;
};

#endif
