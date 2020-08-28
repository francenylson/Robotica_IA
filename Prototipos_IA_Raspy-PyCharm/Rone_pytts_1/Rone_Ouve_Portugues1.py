import speech_recognition as sr

recon = sr.Recognizer()

with sr.Microphone() as source:
    recon.adjust_for_ambient_noise(source, duration=2)
    print('Diga algo . . .')
    audio = recon.listen(source)

print(recon.recognize_google(audio, language='pt'))
