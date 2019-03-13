#if defined(ESP8266)
#include <ESP8266WiFi.h>        
#else
#include <WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <TelegramBot.h>
#include <Servo.h>



//Intervalo entre as checagens de novas mensagens
#define INTERVAL 1000

//Token do seu bot. Troque pela que o BotFather te mostrar
#define BOT_TOKEN "704641187:AAEtAIk7AbvF49HzkMpjGaY_NkN3gLpohFs"

//Troque pelo ssid e senha da sua rede WiFi
#define SSID "spotwifi"      //"AndroidProject"
#define PASSWORD "00110010" //"efacinho"

//Comandos aceitos
const String FEED = "A";
const String FEED2 = "B";
const String FEED3 = "C";
const String START = "/start";

//Cliente para conexões seguras
WiFiClientSecure client;
//Objeto com os métodos para comunicarmos pelo Telegram
UniversalTelegramBot bot(BOT_TOKEN, client);
//Tempo em que foi feita a última checagem
uint32_t lastCheckTime = 0;

//Quantidade de usuários que podem interagir com o bot
#define SENDER_ID_COUNT 2
//Ids dos usuários que podem interagir com o bot. 
//É possível verificar seu id pelo monitor serial ao enviar uma mensagem para o bot
String validSenderIds[SENDER_ID_COUNT] = {"576319400", "576319400"};

Servo name_servo;

int servo_position = 0;


void setup()
{
  Serial.begin(115200);

  //Inicializa o WiFi e se conecta à rede
  setupWiFi();
  name_servo.attach (D4);                //Servo
}

void setupWiFi()
{
  Serial.print("Connecting to SSID: ");
  Serial.println(SSID);

  //Inicia em modo station e se conecta à rede WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  //Enquanto não estiver conectado à rede
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  //Se chegou aqui está conectado
  Serial.println();
  Serial.println("Connected");
}

void funcNewMessages(int numNewMessages)
{
  for (int i=0; i<numNewMessages; i++) //para cada mensagem nova
  {
    String chatId = String(bot.messages[i].chat_id); //id do chat 
    String senderId = String(bot.messages[i].from_id); //id do contato
    
    Serial.println("senderId: " + senderId); //mostra no monitor serial o id de quem mandou a mensagem

    boolean validSender = validateSender(senderId); //verifica se é o id de um remetente da lista de remetentes válidos

    if(!validSender) //se não for um remetente válido
    {
      bot.sendMessage(chatId, "Desculpe mas você não tem permissão", "HTML"); //envia mensagem que não possui permissão e retorna sem fazer mais nada
      continue; //continua para a próxima iteração do for (vai para próxima mensgem, não executa o código abaixo)
    }
    
    String text = bot.messages[i].text; //texto que chegou

    if (text.equalsIgnoreCase(START))
    {
      funcStart(chatId, bot.messages[i].from_name); //mostra as opções
    }
    else if (text.equalsIgnoreCase(FEED))
    {
      funcFeed(chatId); // porcao pequena
    }
    else if (text.equalsIgnoreCase(FEED2))
    {
      funcFeed2(chatId); // porcao media
    }
    else if (text.equalsIgnoreCase(FEED3))
    {
      funcFeed3(chatId); // porcao grande
    }
    else
    {
      funcNotFound(chatId); //mostra mensagem que a opção não é válida e mostra as opções
    }
  }
}

boolean validateSender(String senderId)
{
  //Para cada id de usuário que pode interagir com este bot
  for(int i=0; i<SENDER_ID_COUNT; i++)
  {
    //Se o id do remetente faz parte do array retornamos que é válido
    if(senderId == validSenderIds[i])
    {
      return true;
    }
  }

  //Se chegou aqui significa que verificou todos os ids e não encontrou no array
  return false;
}

void funcStart(String chatId, String fromName)
{
  //Mostra Olá e o nome do contato seguido das mensagens válidas
  String message = "<b>Olá " + fromName + ".</b>\n";
  message += getCommands();
  bot.sendMessage(chatId, message, "HTML");
}

String getCommands()
{
  //String com a lista de mensagens que são válidas e explicação sobre o que faz
  String message = "Os comandos disponíveis são:\n\n";
  message += "<b>" + FEED  + "</b>: Para porção pequena \n";
  message += "<b>" + FEED2 + "</b>: Para porção media  \n";
  message += "<b>" + FEED3 + "</b>: Para porção grande \n";
   return message;
}

void funcFeed(String chatId)
{
  
    servo_position = 60;
    name_servo.write(servo_position);
    servo_position = 5;
    name_servo.write(servo_position);
    delay(1000);
    servo_position = 60;
    name_servo.write(servo_position);
   
  bot.sendMessage(chatId, "O seu pet está <b>alimentado</b>", "HTML");
   
}

void funcFeed2(String chatId)
{
  
    servo_position = 60;
    name_servo.write(servo_position);
    servo_position = 5;
    name_servo.write(servo_position);
    delay(1750);
    servo_position = 60;
    name_servo.write(servo_position);
    
  bot.sendMessage(chatId, "O seu pet está <b>alimentado</b>", "HTML");
   
}

void funcFeed3(String chatId)
{
  
    servo_position = 60;
    name_servo.write(servo_position);
    servo_position = 5;
    name_servo.write(servo_position);   
    delay(2250);
    servo_position = 60;
    name_servo.write(servo_position);
    
  bot.sendMessage(chatId, "O seu pet está <b>alimentado</b>", "HTML");
   
}
void funcNotFound(String chatId)
{
  //Envia mensagem dizendo que o comando não foi encontrado e mostra opções de comando válidos
  String message = "Comando não encontrado\n";
  message += getCommands();
  bot.sendMessage(chatId, message, "HTML");
}

void loop()
{
  //Tempo agora desde o boot
  uint32_t now = millis();
  
  //Se o tempo passado desde a última checagem for maior que o intervalo determinado
  if (now - lastCheckTime > INTERVAL) 
  {
    //Coloca o tempo de útlima checagem como agora e checa por mensagens
    lastCheckTime = now;
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    funcNewMessages(numNewMessages);
  }
}
