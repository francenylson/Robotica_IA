import pyttsx3

from datetime import datetime

agora = datetime.now()

nome3 = "Rone"
hora = agora.hour
minuto = agora.minute

texto = "Fala " + str (nome3) + "seja bem vindo, agora são " + str (hora) + "horas e " +str (minuto) + "minutos"

en = pyttsx3.init()
en.setProperty('rate', 135)
en.setProperty('volume', 1)
en.setProperty('voice', b'brazil')
en.say(texto)
en.runAndWait()