#include "../include/audio.h"

using namespace std;

#define SAFE_RELEASE(punk)  \
              if ((punk) != nullptr)  \
                { (punk)->Release(); (punk) = nullptr; }

#define RETURN_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }

Audio::Audio() : pEnum(nullptr) {}

Audio::Audio(wstring speakers, wstring headset) : headset(headset), speakers(speakers) {
    Audio();
}

Audio::~Audio(void)
{
    releaseDeviceEnumerator();
}

void Audio::releaseDeviceEnumerator()
{
    SAFE_RELEASE(pEnum)
}

HRESULT Audio::initializeDeviceEnumerator()
{
    CoInitialize(NULL);
    HRESULT hr = S_OK;
    if(!pEnum)
    {
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);

    }
    if (FAILED(hr)){
        cerr << "Error: Creating MMDeviceEnumerator instance" << endl;;

    }
    return hr;
}

HRESULT Audio::setDefaultAudioPlaybackDevice( LPCWSTR devID )
{
    IPolicyConfig *pPolicyConfig;
    ERole reserved = eConsole;

    HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfig), (LPVOID *)&pPolicyConfig);

    if (SUCCEEDED(hr))
    {
        hr = pPolicyConfig->SetDefaultEndpoint(devID, reserved);
        pPolicyConfig->Release();

    }
    else{
        cerr << "Error whille settings the default audio device" << endl;
    }
    return hr;
}

void Audio::printConnectedDevices(){

    IMMDeviceCollection* pCollection = NULL;
//    DWORD mask = DEVICE_STATE_ACTIVE || DEVICE_STATE_UNPLUGGED;
    HRESULT hr = pEnum->EnumAudioEndpoints( eRender, DEVICE_STATE_ACTIVE, &pCollection);
    UINT count = 0;
    hr = pCollection->GetCount(&count);

    for (int i = 0; i < (int)count; i++)
    {
        IMMDevice* pEndPoint = NULL;
        hr = pCollection->Item(i, &pEndPoint);
        LPWSTR wstrID = NULL;
        hr = pEndPoint->GetId(&wstrID);
        IPropertyStore* pProps = NULL;
        HRESULT hr = pEndPoint->OpenPropertyStore(STGM_READ, &pProps);
        PROPVARIANT varName;
        PropVariantInit(&varName);
        hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
        wcout << varName.pwszVal<< endl;
        PropVariantClear(&varName);
    }
}

AudioDevice& Audio::findDefaultAudioDevice(AudioDevice& device){
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (void**)&pEnum);
    LPWSTR pwszID = NULL;
    IMMDevice *immDevice;
    hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &immDevice);
    RETURN_ON_ERROR(hr);
    hr = immDevice->GetId(&pwszID);
    RETURN_ON_ERROR(hr);
    {
        auto it = std::find_if(audioDevices.begin(), audioDevices.end(), [=](const AudioDevice& audioDevice){
            return audioDevice.id == pwszID;
        });
        if (it != audioDevices.end()) {
            device = *it;
        }
    }

    Exit:
        CoTaskMemFree(pwszID);
        SAFE_RELEASE(immDevice);
        return device;
}

HRESULT Audio::loadAudioDevices(){
    audioDevices.clear();

    IMMDeviceCollection *devicesCollection=NULL;
    IMMDevice *device=NULL;
    LPWSTR pwszID = NULL;
    HRESULT hr=pEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &devicesCollection);
    RETURN_ON_ERROR(hr)
    UINT count;
    hr=devicesCollection->GetCount(&count);
    RETURN_ON_ERROR(hr)
    for(UINT i=0;i<count;i++)
    {
        hr=devicesCollection->Item(i,&device);
        RETURN_ON_ERROR(hr)
        hr=device->GetId(&pwszID);
        RETURN_ON_ERROR(hr)
        addDevice(device, pwszID);
        CoTaskMemFree(pwszID);
        pwszID=NULL;
        SAFE_RELEASE(device)
    }
    Exit:
        CoTaskMemFree(pwszID);
        SAFE_RELEASE(devicesCollection)
        SAFE_RELEASE(device)
        return hr;
}

void Audio::addDevice(IMMDevice *device, LPWSTR pwszID){
    IPropertyStore *pProps = NULL;
    AudioDevice audioDevice;
    HRESULT hr = device->OpenPropertyStore(STGM_READ, &pProps);
    RETURN_ON_ERROR(hr)
    PROPVARIANT varName;
    PropVariantInit(&varName);
    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
    RETURN_ON_ERROR(hr)
    audioDevice.id.assign(pwszID);
    audioDevice.name.assign(varName.pwszVal);
    audioDevices.push_back(audioDevice);
 Exit:
    PropVariantClear(&varName);
    SAFE_RELEASE(pProps)
}

void Audio::printLoadedDevices(){
    std::for_each(
                  audioDevices.cbegin(),
                  audioDevices.cend(),
                  [] (const AudioDevice device) {std::wcout << device.name << "\n";}
                  );
}


const AudioDevice& Audio::getDefaultDevice(){
    return this->defaultDevice;
}

void Audio::setDefaultDevice(AudioDevice& device){
    this->defaultDevice=device;
}

HRESULT Audio::initDevices(){
    HRESULT hr = loadAudioDevices();
    if(FAILED(hr)){
       cout << "Error: Failed to load devices" << endl;
       return -1;
    }

    AudioDevice device;
    device = findDefaultAudioDevice(device);
    setDefaultDevice(device);
    return hr;

}

wstring Audio::getIdByName(std::wstring name){

    wstring id;
    auto it = std::find_if(audioDevices.begin(), audioDevices.end(), [=](const AudioDevice& device){
        return device.name == name;
    });
    if (it != audioDevices.end()) {
        id = it->id;
    }

    return id;

}

void Audio::switchAudioDevice(wstring headphones, wstring speakers){

    wstring headphonesId = getIdByName(headphones);
    wstring speakersId = getIdByName(speakers);
    AudioDevice device = getDefaultDevice();

    wstring audioDeviceId = device.id == speakersId ? headphonesId : speakersId;
    setDefaultAudioPlaybackDevice(audioDeviceId.c_str());
}
