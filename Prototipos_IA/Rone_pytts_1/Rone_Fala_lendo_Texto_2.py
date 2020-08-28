from gtts import gTTS
from pygame import mixer
import os.path


diretorio = input('DIGITE O DIRETÓRIO DO ARQUIVO DE TEXTO ...')

teste = diretorio

if teste == True:
    print('O ARQUIVO ESTÁ SENDO CARREGADO, AGUARDE POR FAVOR...')
    voz = gTTS(teste, lang='pt')

    print('FALANDO . . .')
    mixer.init()
    mixer.music.load(teste)
    mixer.music.play(teste)

else:
    print(' DIRETÓTIO NÃO ENCONTRADO :(')
    mixer.init()
    mixer.music.load("texto não encontrado")
    mixer.music.play()