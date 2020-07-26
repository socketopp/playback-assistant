#include <windows.h>
#include <fstream>
#include <include/json/json.h>
#include "include/policyconfig.h"
#include "include/audio.h"

using namespace std;

string streamToString(ifstream &stream){
    std::ostringstream out;
    out << stream.rdbuf();
    return out.str();
}

int stringToWString(std::wstring &ws, const std::string &s){
    std::wstring wsTmp(s.begin(), s.end());
    ws = wsTmp;
    return 0;
}

Json::Value readFromJson(ifstream& ifs){
    Json::Value config;
    JSONCPP_STRING err;
    string json = streamToString(ifs);
    const auto rawJsonLength = static_cast<int>(json.length());
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(json.c_str(), json.c_str() + rawJsonLength, &config, &err)){
        std::cout << "Error while reading json." << std::endl;
        return EXIT_FAILURE;
    }
    return config;
}

int main(int argc, char* argv[])
{
    string headphones = "";
    string speakers = "";
    wstring wS;
    wstring wH;
    wstring aS;
    Json::Value config;
    Audio audio;

    if (FAILED(audio.initializeDeviceEnumerator())){
        return -1;
    }
    if (FAILED(audio.initDevices())){
        return -1;
    }

    if (argc > 2){
        ifstream ifs(argv[2]);
        config = readFromJson(ifs);

        if (config == EXIT_FAILURE)
            return -1;

        if (strcmp(argv[1], "-switch") == 0) {

            string instruction = argv[1];
            speakers = config["speakers"].asString();
            headphones = config["headphones"].asString();
            if (speakers.length() < 1 || headphones.length() < 1){
                cerr << "Error: Did not find key in config.json." << endl;
                return -1;
            }
            stringToWString(wS, speakers);
            stringToWString(wH, headphones);
            audio.switchAudioDevice(wS, wH);

        }
        else if (argc > 3 && strcmp(argv[1], "-change") == 0) {

            string instruction = argv[1];
            string audioDevice = argv[3];
            std::for_each(audioDevice.begin(), audioDevice.end(), [](char & c){
                c = ::tolower(c);
            });
            bool faultyDeviceInput = audioDevice != "speakers" && audioDevice != "headphones";
            if (faultyDeviceInput){
                cerr << "Input Error: Wrong audioDevice: " << audioDevice  << "." << endl;
                cerr << "Please input possible devices specified in config.json, i.e. headphones or speakers." << endl;
                return -1;
            } else {
                string device = config[audioDevice].asString();
                if (device.length() < 1){
                    cerr << "Error: Did not find key in config.json." << endl;
                    return -1;
                }
                stringToWString(aS, device);
                wstring wName = audio.getIdByName(aS);
                audio.setDefaultAudioPlaybackDevice(wName.c_str());
            }
        }
        else
            cerr << "Unexpected error" << endl;
    }

    audio.releaseDeviceEnumerator();
    return 0;
}

