import pyttsx3

en = pyttsx3.init()
en.say("VAMOS PROGRAMAR")
en.setProperty('voice', b'brazil')
en.runAndWait()