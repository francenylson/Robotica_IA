#include <Arduino.h>
#include "esp_task_wdt.h" // Lib do watchdog
#include <IOXhop_FirebaseESP32.h> // Lib do Firebase

#include <Wire.h> // Lib necessária para comunicação i2c
#include <SparkFunHTU21D.h> // Lib do sensor HTU21D

// Pinos do sensor  (i2c - canal 1)
#define SDA 21
#define SCL 22

// Objeto referente ao sensor 
HTU21D htudSensor;

// Objeto de conexão (wire) do sensor (O sensor usará o canal 1)
TwoWire wireSensor = TwoWire(1);

// Intervalo de envios de temperatura e umidade (5 segundos)
#define SEND_DATA_INTERVAL 5000

// Url do Firebase 
#define FIREBASE_HOST "NOMEDOPROJETO.firebaseio.com/" 
#define FIREBASE_AUTH "" 

// Nome da rede WiFi e senha
const char *ssid     = "SSIDWiFi"; 
const char *password = "SenhaWiFi"; 

// Pinos dos relés
const int luzLed = 19, luz = 18, ventilador = 5;

// Valores de temperatura e umidade que serão enviados para o Firebase
String temp = "", humd = "";

// Função que lê os valores do sensor
void readClimate()
{
  // Retorna 999 caso dê erro de leitura
  float h = htudSensor.readHumidity();
  float t = htudSensor.readTemperature();

  if(t < 900)
    temp = String(t);
  else
    temp = "";
  
  if(h < 900) 
    humd = String(h);
  else
    humd = "";
}

// Função que inicializa o sensor HTU21D
void htu21Begin()
{
  // Iniciamos a comunicação i2c
  wireSensor.begin(SDA, SCL, 400000);
  // Inicializamos o sensor
  htudSensor.begin(wireSensor);
}

// Função que executa um comando ativando e desativando os relés
String executeCommandFromFirebase(String cmd, String value)
{
  // Retira a primeira barra
  if(cmd.charAt(0) == '/')
    cmd = cmd.substring(1);

  // Exibe na serial o comando e o valor
  Serial.println(cmd);
  Serial.println(value);
  
  // Caso for um comando referente a lampada incandescente
  if(cmd.equals("AUTOMATION/LUZ"))
  {       
    // Se o valor for "LIGADA"
    if(value.equals("{\"STATUS\":\"LIGADA\"}"))
      // Ativamos o relé (lógica inversa)
      digitalWrite(luz, LOW);
    else // Caso contrário desativamos o relé
      // Desativamos o relé (lógica inversa)
      digitalWrite(luz, HIGH);
  }
  else
  // Caso for um comando referente a lampada led
  if(cmd.equals("AUTOMATION/LED"))
  {    
    if(value.equals("{\"STATUS\":\"LIGADO\"}"))
      // Ativamos o relé (lógica inversa)
      digitalWrite(luzLed, LOW);
    else // Caso contrário desativamos o relé
      digitalWrite(luzLed, HIGH);
  }
  else
  if(cmd.equals("AUTOMATION/VENTILADOR"))
  {    
    if(value.equals("{\"STATUS\":\"LIGADO\"}"))
      // Ativamos o relé (lógica inversa)
      digitalWrite(ventilador, LOW);      
    else // Caso contrário desativamos o relé
      digitalWrite(ventilador, HIGH);
  }
  else // Se não entrou em nenhum "if" acima, retornamos uma mensagem de comando inválido 
    return "Invalid command";

  // Se for um comando válido retornamos a mensagem "OK"
  return  "OK";
}

// Função que ativa/desativa todos os relés de acordo com a mensagem recebida ao iniciar o Firebase (Sincronizando as saídas do ESP com os valores atuais do Firebase)
void syncFirebase(String value)
{
  /*
  EXEMPLO DE MENSAGEM RECEBIDA:
  "LED":{"STATUS":"LIGADO"},"LUZ":{"STATUS":"LIGADA"},"TEMPERATURA":{"STATUS":"26.27"},"UMIDADE":{"STATUS":"36.76"},"VENTILADOR":{"STATUS":"LIGADO"}}}
  */

  // Obtendo a substring referente a lampada led
  String aux = value.substring(value.indexOf("LED"));

  int posVar = aux.indexOf("STATUS\":\"")+9;
  String status = "";
  
  for(int i = posVar; aux.charAt(i)!='\"'; i++)
    status += aux.charAt(i);
  
  Serial.println("Status LED:"+status);

  // Sincronizando saída referente a lampada led
  if(status.equals("LIGADO"))
    digitalWrite(luzLed, LOW);
  else
  if(status.equals("DESLIGADO"))
    digitalWrite(luzLed, HIGH);
    
  status = "";  

  // Obtendo a substring referente a lampada incandescente
  aux = value.substring(value.indexOf("LUZ"));
  posVar = aux.indexOf("STATUS\":\"")+9;

  for(int i = posVar; aux.charAt(i)!='\"'; i++)
    status += aux.charAt(i);

  Serial.println("Status LUZ:"+status);
  // Sincronizando saída referente a lampada incandescente
  if(status.equals("LIGADA"))
    digitalWrite(luz, LOW);
  else
  if(status.equals("DESLIGADA"))
    digitalWrite(luz, HIGH);
    
  status = "";
  
  // Obtendo a substring referente ao ventilador
  aux = value.substring(value.indexOf("VENTILADOR"));
  posVar = aux.indexOf("STATUS\":\"")+9;

  for(int i = posVar; aux.charAt(i)!='\"'; i++)
    status += aux.charAt(i);
    
  Serial.println("Status VENTILADOR:"+status);
  // Sincronizando saída referente ao ventilador
  if(status.equals("LIGADO"))
    digitalWrite(ventilador, LOW);
  else
  if(status.equals("DESLIGADO"))
    digitalWrite(ventilador, HIGH);
}

// Função que inicia a conexão com o Firebase
void firebaseBegin()
{
  // Iniciamos a comunicação Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  
  
  // Setamos o callback (Executado a cada alteração do firebase)
  Firebase.stream("/", [](FirebaseStream stream) 
  {
    String path, value;  

    // Se o evento que vem do callback é de alteração "put"
    if(stream.getEvent() == "put") 
    {     
      // Obtemos os valores de path e value
      path = stream.getPath();
      value = stream.getDataString();   

      // Deixamos em maiúsculo
      path.toUpperCase();         
      value.toUpperCase();

      // Se for a mensagem inicial enviada pelo firebase assim que o ESP inicia sua comunicação
      if(value.startsWith("{\"AUTOMATION\""))
      {
        // Sincronizamos as saídas para os relés de acordo com a mensagem recebida
        syncFirebase(value);        
        Serial.println("Firebase sincronizado");
      }
      else
      // Se não for uma mensagem referente a temperatura e umidade, executamos um comando e exibimos na serial
      if(!path.equals("/AUTOMATION/UMIDADE/STATUS") && !path.equals("/AUTOMATION/TEMPERATURA/STATUS"))
        Serial.println(executeCommandFromFirebase(path, value));
    }    
  });
}

// Task que envia os dados de temperatura e umidadade para o Firebase de tempo em tempo
void sendSensorData(void *p)
{
  // Adicionamos a tarefa na lista de monitoramento do watchdog
  esp_task_wdt_add(NULL);

  while(true)
  {     
    // Resetamos o watchdog
    esp_task_wdt_reset();     
    // Lê os valores do sensor
    readClimate();  

    // Se foi possível ler o sensor
    if(temp != "" && humd != "")
    {
      // Enviamos para o Firebase
      Firebase.setString("/automation/temperatura/status", temp);
      Firebase.setString("/automation/umidade/status", humd);      
      Serial.println("sent");
    }
    // Aguardamos um tempo de 5 segundos
    delay(SEND_DATA_INTERVAL);
  }
}

// Função que inicia o WiFi
void wifiBegin()
{
  // Iniciamos o WiFi
  WiFi.begin(ssid, password);  
    
  Serial.println("Wifi Connecting");

  // Enquanto não estiver conectado exibimos um ponto
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    esp_task_wdt_reset();
    delay(1000);
  }
  // Exibimos "OK"
  Serial.println("OK");
}

void setup() 
{  
  // Iniciamos a serial
  Serial.begin(115200);

  // Inicializamos o watchdog com 15 segundos de timeout
  esp_task_wdt_init(15, true);

  // Setamos os pinos dos relés como saída
  pinMode(luz, OUTPUT);
  pinMode(luzLed, OUTPUT);    
  pinMode(ventilador, OUTPUT);   
  
  // Iniciamos o sensor
  htu21Begin();

  // Iniciamos o WiFi
  wifiBegin();
  // Iniciamos a conexão com o Firebase
  firebaseBegin();
  // Iniciamos a task de envio de temperatura e umidade
  xTaskCreatePinnedToCore(sendSensorData, "sendSensorData", 10000, NULL, 1, NULL, 0);
}

void loop() 
{
  
}
















