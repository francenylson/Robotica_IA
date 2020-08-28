#!/usr/local/bin/python
# -*- coding: UTF-8 -*-

"""
  Esse projeto usa um sensor
  HC-SR04

  Modificado em 17 de agosto de 2016
  por Vitor Mazuco (vitor.mazuco@gmail.com)
"""

import RPi.GPIO as GPIO                   
import time                                
from adafruit_servokit import ServoKit
import board
import digitalio
import busio


GPIO.setmode(GPIO.BCM)

TRIG = 23                          #Pino 23 associado ao TRIG
ECHO = 24                          #Pino 24 associado ao ECHO
TRIG2 = 8
ECHO2 = 7

print("Medição de distância em andamento")

GPIO.setup(TRIG,GPIO.OUT)                  
GPIO.setup(ECHO,GPIO.IN)
GPIO.setup(TRIG2,GPIO.OUT)
GPIO.setup(ECHO2,GPIO.IN)

try:
	while True:
		#CONFIGURAR E DEFINIR OS SERVOS
		kit = ServoKit(channels=16)
		kit.servo[0].angle = 0 # INICIAR O SERVO DA CABEÇA HORIZONTAL EM POSIÇÃO 0
		kit.servo[1].angle = 0 #INICIAR O SERVO DA CABEÇA VERTICAL EM POSIÇÃO 0
		kit.servo[2].angle = 0 # INICIAR O SERVO DO BRAÇO DIREITO EM POSIÇÃO DE DESCANSO
		kit.servo[3].angle = 0 #INICIAR O SERVO DO BRAÇO ESQUERDO EM POSIÇÃO 0
		GPIO.output(TRIG, False)                 #Configura o pino TRIG como LOW
		GPIO.output(TRIG, False)
		print("SENSOR ESQUERDO ESTABILIZANDO...")
		print("SENSOR DIREITO ESTABILIZANDO...")
		time.sleep(1)                            #Espera por 2 segundos

		#SENSOR ESQUERDO
		GPIO.output(TRIG, True)                  #Configura o pino TRIG como HIGH
		time.sleep(0.00001)                      #Espera 0.00001 segundos
		GPIO.output(TRIG, False)                 #Configura o pino TRIG como LOW
		#SENSOR DIREITO
		GPIO.output(TRIG2, True)  # Configura o pino TRIG como HIGH
		time.sleep(0.00001)  # Espera 0.00001 segundos
		GPIO.output(TRIG2, False)  # Configura o pino TRIG como LOW

		# CALCULO DA DISTANCIA SENSOR ESQUERDA
		while GPIO.input(ECHO)==0:               #Checa se o pino ECHO é LOW
			inicio_de_pulso = time.time()        #Salva a última vez conhecida de pulso LOW

		while GPIO.input(ECHO)==1:               #Checa se o pino ECHO é HIGH
			fim_de_pulso = time.time()           #Salva a última vez conhecida de pulso HIGH 

			duracao_de_pulso = fim_de_pulso - inicio_de_pulso #Obter duração do pulso a uma variável

			distancia = duracao_de_pulso * 17150        #duração do pulso multiplicar por 17150 para obter distancia
			distancia = round(distancia, 2)            #Rodada com duas casas decimais

		# CÁLCULO DA DISTANCIA SENSOR DA DIREITA
		while GPIO.input(ECHO2) ==0:
			inicio_de_pulso2 = time.time()

		while GPIO.input(ECHO2)==1:
			fim_de_pulso2 = time.time()

			duracao_de_pulso2 = fim_de_pulso2

			distancia2 = duracao_de_pulso2 * 17150
			distancia2 = round(distancia2, 2)

			# TESTANDO O FUNCIONAMENTO DOS SENSORES DE APROXIMAÇÃO
			print("DISTANCIA SENSOR ESQUERDA : ", distancia)
			print("DISTANCIA SENSOR DIREITA : " , distancia2)

			# MOVIMENTOS HORIZONTAIS DA CABEÇA
		if distancia < 60 :      # SE A DISTANCIA ESTÁ MENOR QUE 60CM - ACIONAR SENSOR ESQUERDA
			print("DETECTADO MOVIMENTO A ESQUERDA EM :",distancia - 0.5,"cm") # ACIONA DETECÇÃO
		elif distancia2 < 60 :   # SE DISTANCIA2 ESTÁ MENOR QUE 60CM - ACIONAR SENSOR DIREITA
			print("DETECTADO MOVIMENTO A DIREITA EM :",distancia2 - 0.5,"cm")   # ACIONA DETECÇÃO
		elif distancia > 60 and distancia2 > 60:
			print("SEM DETECÇÃO DE MOVIMENTOS NAS LATERAIS, MANTER CABEÇA CENTRALIZADA")

# Ele limpa as portas GPIO quando é forncecido uma interrupçao no teclado CTRL + C 
except KeyboardInterrupt:
    pass
finally:
    GPIO.cleanup() # Limpa a programação atual das portas GPIO