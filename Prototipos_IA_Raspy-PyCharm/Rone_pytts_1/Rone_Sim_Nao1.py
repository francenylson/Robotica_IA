import speech_recognition as sr
import os

print('DESEJA VER A IMAGEM ESP ...')

recon = sr.Recognizer()

with sr. Microphone() as source:
    audio = recon.listen(source)
resposta = recon.recognize_google(audio, language='pt')

if resposta == "sim":
    print('ABRINDO SUA AGENDA...')
    os.system(r'C:\Users\Professora\Documents\CURSO INTRODUÇÃO A ROBOTICA EDUCACIONAL LIVRE - 2019\pesquisa-perfil-cef101.odt')
elif resposta =="não":
    print('SOLICITAÇÃO NEGADA ...')
