from gtts import gTTS
from pygame import mixer
import os.path


diretorio = input('DIGITE O DIRETÓRIO DO ARQUIVO DE TEXTO ...')

teste = os.path.isfile(diretorio)

if teste == True:
    print('O ARQUIVO ESTÁ SENDO CARREGADO, AGUARDE POR FAVOR...')
    file_data = open(diretorio)
    file_data = file_data.read()



    voz = gTTS(file_data, lang='pt')
    voz.save('ceia.mp3')

    print('FALANDO . . .')

    mixer.init()
    mixer.music.load('ceia.mp3')
    mixer.music.play()

else:
    print(' DIRETÓTIO NÃO ENCONTRADO :(')