import pyttsx3

nova_velocidade = 50

en = pyttsx3.init()

while nova_velocidade <=300:
    en.setProperty('rate', nova_velocidade)
    en.say("Fala galera do projeto peti aluno maker digital")
    en.setProperty('voice', b'brazil')
    en.runAndWait()
    nova_velocidade = nova_velocidade + 50