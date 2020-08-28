import pyttsx3

nova_velocidade = 50

en = pyttsx3.init()
en.setProperty('rate', 125)
en.setProperty('volume', 0.9 )
en.say("Fala galera do projeto peti aluno maker digital, estamos evoluindo para processamento de linguagem natural e esse é o primeiro teste")
en.setProperty('voice', b'brazil')
voice = en.getProperty('voices')[6]
en.setProperty('voice', voice.id)
en.runAndWait()
