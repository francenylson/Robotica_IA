from gtts import gTTS
from pygame import mixer


voz = gTTS("Fala galera, vamos usar o google assistente na nossa voz", lang='pt')
voz.save("voz.mp3")

mixer.init()
mixer.music.load('voz.mp3')
mixer.music.play()

x = input ('Digite algo para finalizar . . .')

