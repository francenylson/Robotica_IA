import pyttsx3
import time

from datetime import datetime

agora = datetime.now()

nome3 = "Jonas"
hora = agora.hour
minuto = agora.minute



while True:
    texto = input("DIGITE A MINHA FALA :")  # "Fala " + str (nome3) + "sejam bem vindos, agora são " + str (hora) + "horas e " +str (minuto) + "minutos"
    minhafala = texto
    en = pyttsx3.init()
    en.setProperty('rate', 135)
    en.setProperty('volume', 1)
    en.setProperty('voice', b'brazil')
    en.say(minhafala)
    en.runAndWait()
