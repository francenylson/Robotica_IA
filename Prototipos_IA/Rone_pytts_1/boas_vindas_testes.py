import pyttsx3

from datetime import datetime

agora = datetime.now()

fala = input('Digite algo a Falar ...')

nome = "Francenylson"
nome2 = "Risoneide"
nome3 = "Jonas"
nome4 = "Jéssica"
hora = agora.hour
minuto = agora.minute

texto = "Fala " + str(nome) + str(nome2) + str(nome3) + str(nome4) + "sejam bem vindos, agora são " + str(
    hora) + "horas e " + str(minuto) + "minutos"



with pyttsx3.init() as source:
    while True:
        print('Fale algo por favor')
        en = pyttsx3.init()
        en.setProperty('rate', 135)
        en.setProperty('volume', 1)
        en.setProperty('voice', b'brazil')
        en.say(texto)
        en.runAndWait()
        fale = fala.listen(source)
        print(fala.en.say(fale))