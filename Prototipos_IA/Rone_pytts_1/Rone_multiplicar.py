import speech_recognition as sr

recon = sr.Recognizer()

with sr.Microphone() as source:
    while True:
        print('FALE QUAL CÁLCULO DEVO FAZER...')
        audio = recon.listen(source)

        txsoma = recon.recognize_google(audio, language='pt')
        if txsoma =="fechar": break

        print('SUA MULTIPLICAÇÃO : ' + str(txsoma))
        print('RESULTADO : ' + str(int(txsoma[0]) * int(txsoma[4])))
