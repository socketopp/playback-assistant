#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include "policyconfig.h"

struct AudioDevice {
    std::wstring id;
    std::wstring name;
    AudioDevice() :id(L""), name(L""){}
};

class Audio {
    public:
        Audio();
        ~Audio();
        Audio(std::wstring, std::wstring);
        HRESULT setDefaultAudioPlaybackDevice( LPCWSTR );
        void printConnectedDevices();
        void printLoadedDevices();
        void switchAudioDevice(std::wstring, std::wstring);
        HRESULT loadAudioDevices();
        const AudioDevice& getDefaultDevice();
        std::wstring getIdByName(std::wstring);
        void releaseDeviceEnumerator();
        HRESULT initializeDeviceEnumerator();
        HRESULT initDevices();
    private:
        AudioDevice& findDefaultAudioDevice(AudioDevice& device);
        void setDefaultDevice(AudioDevice& device);
        IMMDeviceEnumerator* pEnum;
        std::wstring headset;
        std::wstring speakers;
        void addDevice(IMMDevice* device, LPWSTR pwszID);
        AudioDevice defaultDevice;
        std::vector<AudioDevice> audioDevices;

};


#endif // AUDIO_H
