import pyttsx3
import time

from datetime import datetime

agora = datetime.now()

nome3 = "Jonas"
hora = agora.hour
minuto = agora.minute



while True:
    texto = input("DIGITE A MINHA FALA :  ")

    minhafala ="", texto
    en = pyttsx3.init()
    en.setProperty('rate', 175)
    en.setProperty('volume', 2)
    en.setProperty('voice', b'brazil')
    en.say(minhafala)
    en.runAndWait()
    