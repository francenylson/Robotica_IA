import pyttsx3

en = pyttsx3.init()
en.setProperty('rate', 50)
en.say("Fala galera do projeto peti aluno maker digital, estamos evoluindo para processamento de linguagem natural e esse é o primeiro teste")
en.setProperty('voice', b'brazil')
en.runAndWait()
