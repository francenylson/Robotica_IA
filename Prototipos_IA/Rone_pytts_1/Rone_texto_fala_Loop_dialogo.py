import speech_recognition as sr
import pyttsx3

from datetime import datetime

recon = sr.Recognizer()

agora = datetime.now()

fala = pyttsx3.init()


nome = "Francenylson"
nome2 = "Risoneide"
nome3 = "Jonas"
nome4 = "Jéssica"
hora = agora.hour
minuto = agora.minute

texto = "Fala " + str (nome) + str (nome2) + str (nome3) + str (nome4) + "sejam bem vindos, agora são " + str (hora) + "horas e " +str (minuto) + "minutos"

en = pyttsx3.init()
en.setProperty('rate', 135)
en.setProperty('volume', 1)
en.setProperty('voice', b'brazil')
en.say(texto)
en.runAndWait()

with en.say as source:
    while True:
        print('Digite algo para eu falar ...')
        fala = en.say(source)


