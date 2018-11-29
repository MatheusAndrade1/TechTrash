/*TECH TRASH
Autores: Emerson Cardoso, Felipe Boni Vilalta, João Gabriel, Matheus Andrade
Novembro 2018
Hardware utilizado:
  NodeMCU ESP 8266 Devikit
  Sensor ultrassonicoHC-SR04
  CI 4050
*/
//Bibliotecas do nodemcu
#include <ESP8266WiFi.h> //This library provides ESP8266 specific Wi-Fi routines we are calling to connect to network.
#include <WiFiClient.h> //Creates a client that can connect to to a specified internet IP address
#include <ESP8266WebServer.h> //Possui as funções para hospedar a páginas Web
//=======================

//Biblioteca sensor ultrassonico
#include <Ultrasonic.h> //Carrega a biblioteca do sensor ultrassonico
//=======================

//Definição dos pinos do sensor ultrassonico
#define pino_trigger 4 
#define pino_echo 5 //Iremos utilizá-lo na interrupção
//=======================


const char* ssid = "TECHTRASH";
const char* password = "12345678";

ESP8266WebServer server(80); // Declaração da página Web Server na porta 80, porta padrão da Web

Ultrasonic ultrasonic(pino_trigger, pino_echo); //Inicializa o sensor nos pinos definidos acima

const int led = 2;
float cmMsec, lixo; //Variaveis de medicao

//====================Endereço Web server sendo acessado
void handleRoot() {
  String textoHTML;
  if(cmMsec>=0 && cmMsec<=100)
  {
    textoHTML += cmMsec;
    textoHTML += "%";
  }else
  {
    textoHTML += "0.00%";
  }  
  server.send(200, "text/html", textoHTML);
}
//=======================


//====================Caso o acesso não tenha ocorrido com sucesso
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri(); //URI da Web server
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
//=======================


void setup(void){
  Serial.begin(115200); //Iniciando o monitor serial na mesma velocidade de comunicação do ESP
  Serial.flush();
  WiFi.mode(WIFI_STA); //Station (STA) mode is used to get ESP module connected to a Wi-Fi network established by an access point.
  WiFi.begin(ssid, password); //Actual connection to Wi-Fi is initialized
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }
    
  Serial.println("");
  Serial.println("TECH TRASH");
  Serial.println("Emerson Cardoso, Felipe Boni Vilalta, João Gabriel, Matheus Andrade");
  Serial.println("");
  Serial.print("Conectado na rede: ");
  Serial.println(ssid);
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP()); //Imprimi o endereço IP definido para o ESP
  
  server.on("/", handleRoot); //Tell the server what URIs it needs to respond to:

  server.on("/inline", []()
  {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound); //Caso o servidor não seja localizado chama a função handleNotFound

  server.begin(); //Inicia o Web server
  Serial.println("HTTP server started");
  //attachInterrupt(digitalPinToInterrupt(pino_echo), medida, LOW); 
  
  //wifi_set_sleep_type(LIGHT_SLEEP_T);
  //gpio_pin_wakeup_enable(16,GPIO_PIN_INTR_LOLEVEL);
  //ESP.deepSleep(1e6);
  wdt_enable(WDTO_2S); //Definindo a rotina watchdogs para reset em 1 segundos
}

void medida()
{
  long microsec = ultrasonic.timing();  //Le os dados do sensor com o tempo de retorno do sinal
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);  //Calcula a distancia em cm a partir do tempo de retorno em microsegundos
  cmMsec=map(24-cmMsec,0,24,0,100); //Regra de tres para calcular a porcentagem de carregamento do lixo
  server.handleClient();
  delay(1000);
}

void loop(void){  
  medida();
  wdt_reset ( );
}
