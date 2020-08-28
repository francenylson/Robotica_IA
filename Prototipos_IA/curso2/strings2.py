nome = 'Francenylson'  # nunca 1nome=
idade = 50
e_maior = idade > 18
altura = 1.85
peso = 93
imc = (peso/(altura*2))
print(nome, 'tem', idade, 'anos de idade e seu imc é', imc)  #primeira possibilidade
print(f'{nome} tem {idade} anos de idade e seu imc é {imc}')  #segunda possibilidade
print(f'{nome} tem {idade} anos de idade e seu imc é {imc:.2f}')  #terceira possibilidade
print('{} tem {} anos de idade e seu imc é {:.2f}'.format(nome, idade, imc))  #quarta possibilidade
print('{0} tem {1} anos de idade e seu imc é {2:.2f}'.format(nome, idade, imc))  #quinta possibilidade
print('{n} tem {i} anos de idade e seu imc é {im:.2f}'.format(n=nome, i=idade, im=imc))  #sexta possibilidade
print('{n} tem {i} anos de idade e seu imc é {im:.2f}'.format(n=nome, i=idade, im=imc))  #sexta possibilidade
