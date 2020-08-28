String inputString = " " ;         // uma string para conter os dados de entrada
bool stringComplete = false ;  // se a string está completa

# define  pinoLampada  8

void  setup () {
  // inicializar serial:
  Serial. começar ( 115200 );
  // reserve 200 bytes para o inputString:
  inputString. reserva ( 200 );
  
  pinMode (pinoLampada, OUTPUT);
}

void  loop () {
  // imprime a string quando uma nova linha chega:
  if (stringComplete) {
    // limpar a string:
    se (inputString. startsWith ( " Ligar " )) {
      // digitalWrite (pinoLampada,! digitalRead (LED_BUILTIN)); // alternar
      digitalWrite (pinoLampada, HIGH);
      Serial. imprimir ( " Assistente Mil Grau. Luz ligada. " );
    } else  if (inputString. startsWith ( " desligar " )) {
      digitalWrite (pinoLampada, LOW);
      Serial. imprimir ( " Assistente Mil Grau. Luz desligada. " );
    }
     
    inputString = " " ;
    stringComplete = false ;
  }
}

/ *
  SerialEvent ocorre sempre que um novo dado chega no RX serial do hardware. este
  rotina é executada entre cada execução do loop de tempo (), portanto, usar o atraso dentro do loop pode
  resposta de atraso. Vários bytes de dados podem estar disponíveis.
* /
void  serialEvent () {
  while (Serial. available ()) {
    // obtém o novo byte:
    char inChar = ( char ) Serial. read ();
    // adicione-o ao inputString:
    inputString + = inChar;
    // se o caractere de entrada é uma nova linha, defina um sinalizador para que o loop principal possa
    // faça algo sobre isso:
    if (inChar == ' \ n ' ) {
      stringComplete = true ;
    }
  }
}
