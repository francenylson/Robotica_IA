import speech_recognition as sr

recon = sr.Recognizer()

with sr.Microphone() as source:
    while True:
        print('Fale algo por favor')
        audio = recon.listen(source)
        print(recon.recognize_sphinx(audio))

