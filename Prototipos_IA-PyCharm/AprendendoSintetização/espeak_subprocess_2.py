import os
from subprocess import call

#os.system('espeak -v pt -s 140') #atentar para pontos, aspas simples e duplas e espaços

#fala = input('DIGITAL A FALA DO ROBÔ :')
fala = input('DIGITE A FALA DO RONE:')

voz = fala


call(["espeak", voz])

#os.system('espeak -v pt -s 140 ') #atentar para pontos, aspas simples e duplas e espaços

