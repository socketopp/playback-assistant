#!/usr/bin/python

import pyaudio
import speech_recognition as sr
import pyttsx3
import time
import os
import sys
import subprocess
import logging
from threading import Thread
from pynput.keyboard import Key, Listener

class VoiceAssistant(object):
    """
    Voice assisant based of speech_recognition and pyaudio.
    """    
    response = {
        "success": True,
        "error": None,
        "transcription": None
    }
    previous_key = None

    def __init__(self, wake_word):
        """
        Initialize `VoiceAssistant` class.
        PARAM:
            -- wake_word (String) :  Wake word for speech recognition.
        """
        self.wake_word = wake_word.lower()

    def reset_response(self):
        self.response =  {
            "success": True,
            "error": None,
            "transcription": None
        }

    def speak(self, text):
        engine = pyttsx3.init()
        engine.say(text)
        engine.runAndWait()

    def match(self, text, words):
        match = False
        for word in words:
            match = text.lower().count(word.lower()) > 0
            if match:
                return match
        return match

    def get_audio(self):
        r = sr.Recognizer()
        with sr.Microphone() as source:
            r.adjust_for_ambient_noise(source)
            audio = r.listen(source, phrase_time_limit=3)
            said = ""
            try:
                said = r.recognize_google(audio)
            except Exception as e:
                print("Exception: " + str(e))
            except r.UnknownValueError:
                print("Sphinx could not understand audio")
        if said:
            return said.lower()
        else:
            self.speak("No instruction")
            return

    def change(self):
        device = self.get_audio()
        response = False
        if device:
            if self.match(device, ["speakers", "speaker"]):
                self.speak("Turning on speakers")
                subprocess.run([r'Playback.exe', '-change',
                                'config.json', "speakers"])

            elif self.match(device, ["headphones", "headphone", "headset", "headsets"]):
                self.speak("Turning on headphones")
                subprocess.run([r'Playback.exe', '-change',
                                'config.json', "headphones"])

            elif self.match(device, ["switch", "change"]):
                self.switch_playback_device()

            else:
                self.speak("I think you said " + device)
                self.change()

        else:
            self.change()

    def handle_request(self, recognizer, audio):
        try:
            self.response["transcription"] = recognizer.recognize_google(audio)
        except sr.RequestError:
            # API was unreachable or unresponsive
            self.response["success"] = False
            self.response["error"] = "API unavailable"
        except sr.UnknownValueError:
            # speech was unintelligible
            self.response["error"] = "Unable to recognize speech"
        return self.response

    def process_speech(self, speech):
        if self.match(speech, ["hey " + self.wake_word]):
            if self.match(speech, ["switch", "change", "shange"]):
                self.switch_playback_device()
            else:
                self.speak(self.wake_word + "at your service")
                self.change()

    def log(self, response):
        logging.info("--- START OF MESSAGE ---")
        logging.info("Time: " + time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime()))
        logging.info("Error: " + response["error"])
        logging.info("Transcription: " + str(response["transcription"]))
        logging.info("Success: " + str(response["success"]))
        logging.info("--- END OF MESSAGE --- \n")

    def switch_playback_device(self):
        self.speak("Switching playback devices")
        subprocess.run([r'Playback.exe', '-switch', 'config.json'])

    def start_assistant(self):
        self.speak("Initializing Voice assistant with wake word: " + self.wake_word)
        logging.basicConfig(filename='log_history.log', level=logging.INFO)
        recognizer = sr.Recognizer()
        microphone = sr.Microphone()
        with microphone as source:
            while True:
                recognizer.adjust_for_ambient_noise(source)
                audio = recognizer.listen(source, phrase_time_limit=3)
                response = self.handle_request(recognizer, audio)
                if response["success"] and response["transcription"]:
                    self.process_speech(response["transcription"])
                else:
                    self.log(response)

                self.reset_response()

    def on_press(self, key):
        if key == Key.f5 and self.previous_key == key.ctrl_l:
            self.switch_playback_device()
        else:
            self.previous_key = key

    def manual_switch(self):
        with Listener(
            on_press=self.on_press) as listener:
            listener.join()

    def run(self):
        t1 = Thread(target = self.manual_switch).start()
        t2 = Thread(target = self.start_assistant).start()

def main(wake_word):
    va = VoiceAssistant(wake_word)
    va.run()

if __name__ == '__main__':
    wake_word = sys.argv[1] if len(sys.argv) == 2 else "computer"
    main(wake_word) 


