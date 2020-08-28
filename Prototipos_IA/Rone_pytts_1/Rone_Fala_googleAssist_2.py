from gtts import gTTS
import os


voz = gTTS("Fala galera, vamos usar o google assistente na nossa voz", lang='pt')
voz.save("voz1.mp3")

os.system('voz1.mp3')
