# Playback Assisant

The Playback Assistant can do *one thing and one thing only*: change your computer's playback device. The assistant is capable of switching between headphones and speakers.  

This project consists of two parts, the voice assistant (Python) and the playback service (C++). The voice assistant is written with Python's speech_recognition library and playback service is written with the COM-interface IPolicyConfig by EreTIk that set default audio render endpoint. 

### Prerequisites
The playback service is built with MSVC 2017 64-bit compiler because IPolicyConfig is reverse engineered to set default audio render endpoint on Windows OS. So make sure you have installed MSVC and have Developer Command Prompt in Windows, it is also possible to compile this using QT if you don't want to compie this using the command line. 

The voice assistant require Python version 3.8.2 or higher. To run the Python script, use *pip install* for all the necessary libraries.

## Getting Started
Compile this in the root folder to generate .exe file. 
```
$ cl -o Playback.exe src/main.cpp src/audio.cpp dist/jsoncpp.cpp ole32.lib
```

In order to run the voice assistant you need to add your output devices in the config.json.
You can use the compiled C++ program to see your available output devices. 
```
$ ./Glitch.exe -print
2460G4 (NVIDIA High Definition Audio)
Speakers (Realtek(R) Audio)
Headset Earphone (HyperX Virtual Surround Sound)
```
Then just add corresponding device to the config file:
__Config.json__ 
```
{ "speakers" : "Speakers (Realtek(R) Audio)",
 "headphones" : "Headset Earphone (HyperX Virtual Surround Sound)"
}
```
## Basic Usage
The project needs __3__ files in the same folder to work properly:
```
voiceAssistant.py 
Playback.exe (compiled earlier)
config.json 
```
Running voiceAssistant.py starts the voice assistant with default wake word as *Computer*. You can change the wake word by adding it as argument when booting the voice assistant.  
```
python voiceAssistant.py <optional wake word>
```

Available commands: 
* *Hey Computer* *(switch/change)* = Switches playback devices. Hey computer is the wake word and switch/change is the instruction.

* Hey *Computer* = Voice assistant listens for further instructions:
    * Turn on *speaker(s)* = Turns on speakers
    * Turn on *(headphone(s)/headset(s))* = Turns on headphones
    * *Switch/Change* = Switches or changes the playback devices
### Manual switch

If the voice recognition isn't responding as desired, you can always press CTRL+F5 to switch between your playback devices. 

## Run at startup (Windows)

Create a .txt file with the following content:
hide_current_console.exe is located under /doc
```
start /b python <path to> voiceAssistant.py
<path to> hide_current_console.exe && exit
```
1. Change the suffix of the txt file to .bat and create a shortcut. 
2. Open Run and enter shell:startup. 
3. Cut the .bat file and paste in the startup folder.
Windows will now create a process that starts the voiceAssistant at startup. 
Notice (Make sure that all necessary files are in the same folder as voiceAssistant.py, i.e. Playback.exe and config.json)


## Built With

* [SpeechRecognition](https://pypi.org/project/SpeechRecognition/) - Library for performing speech recognition, with support for several engines and APIs, online and offline.
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) - C++ Json library

## License

This project is licensed under the Apache License - see the [LICENSE.md](LICENSE.md) file for details

