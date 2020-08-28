
from subprocess import call

#os.system('espeak -v pt -s 140') #atentar para pontos, aspas simples e duplas e espaços

#fala = input('DIGITAL A FALA DO ROBÔ :')
fala = "olá mundo"


call(['espeak', '-vpt+f1', '-s 150',  fala])

#os.system('espeak -v pt -s 140 ') #atentar para pontos, aspas simples e duplas e espaços

