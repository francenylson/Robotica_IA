import pyttsx3

novo_volume = 0.20

en = pyttsx3.init()

while novo_volume <=1:
    en.setProperty('volume', novo_volume)
    en.say("Fala galera do projeto peti aluno maker digital")
    en.setProperty('voice', b'brazil')
    en.runAndWait()
    novo_volume = novo_volume + .20