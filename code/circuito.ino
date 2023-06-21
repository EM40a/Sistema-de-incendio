#include <LiquidCrystal.h> // Para el LCD
#include <IRremote.h> // Para el Control IR
#include <Servo.h>

// Definimos los pines de cada componente 
#define SENSOR_TEMPERATURA A0
#define LED_1 13
#define LED_2 12 
#define CONTROL_IR 11
#define SERVOMOTOR 9

#define limiteTemperatura 50 // El limite de temperatura limite antes de que se active la alarma

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); 

IRrecv irRecibidor(CONTROL_IR); // Indicamos que pin recibirá los datos
decode_results irDato; // Guarda los datos recibidos

Servo servoMotor; 

// Variables globales
int temperatura; // Guarda la temperatura leida por el sensor
bool encendido = false; // Indica si el sistema esta encendido o apagado
String estacion; // Guarda la estacion del año

void setup()
{
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);

  int temperaturaLeida = analogRead(SENSOR_TEMPERATURA); // Recibe un valor de 0 a 1023
  // Mapea el ese valor a un rango de -40 a 125, que son los limites del sensor de temperatura
  temperatura = map(temperaturaLeida, 20, 350, -40, 125); 

  lcd.begin(16, 2); // Inicializa el LCD con 16 columnas y 2 filas
  servoMotor.attach(SERVOMOTOR); // Inicializa el ServoMotor
  irRecibidor.begin(CONTROL_IR, DISABLE_LED_FEEDBACK); // Inicializa el Control IR
}

void loop()
{
  ControlRemotoIR(); // Controla el sistema con el control remoto  
  EstacionAnio(temperatura); // Indica la estacion del año segun la temperatura

  if (encendido) {
    AlarmaIncendio(temperatura);
    ImprimirTemperatura(temperatura);
    digitalWrite(LED_1, HIGH);
  } else {
    digitalWrite(LED_1, LOW);
    lcd.clear(); // Limpiamos el LCD
  }
}

void ControlRemotoIR()
{
  if (irRecibidor.decode()) {
    switch (irRecibidor.decodedIRData.decodedRawData) {
      case 0xFF00BF00: // Boton de encendido
        encendido = !encendido;
        break;

      case 0xF50ABF00: //Boton de subir
        temperatura += 5;
        break;

      case 0xF708BF00: //Boton de Bajar
        temperatura -= 5;
        break;

      default:
        break;
    }
    irRecibidor.resume(); // Recibe el siguiente dato
  }
  delay(100); // Evitar rebotes
}

void EstacionAnio(int temp)
{
  if (temp >= 25 && temp <= limiteTemperatura){
    estacion = "Verano";
  } else if (temp >= 15 and temp < 25){
    estacion = "Primavera";
  } else if (temp >= 10 and temp < 15){
    estacion = "Otonio";
  } else if (temp >= -5 and temp < 10){
    estacion = "Invierno";
  }
  else {
    estacion = "";
  }
}
// Imprime la temperatura en el LCD junto a la estacion del año
void ImprimirTemperatura(int temp)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print(estacion);
  delay(600);
}

// Prende un led y apaga el otro
void PrendeApagaLed(int led_1, int led_2)
{
  digitalWrite(led_1, HIGH);
  digitalWrite(led_2, LOW);
}

// Enciende el servo motor y prende un led
void EncenderServo(int tiempo)
{
  servoMotor.write(90); // Gira el servo motor 90 grados 
  PrendeApagaLed(LED_2, LED_1);
  delay(tiempo);
  servoMotor.write(0); // Gira el servo motor a su posicion inicial
  PrendeApagaLed(LED_1, LED_2);
  delay(tiempo);
}

// Activa el mecanismo en caso de sobrepasar la temperatura limite
void AlarmaIncendio(int temp)
{
  if (temp >= limiteTemperatura) {
    lcd.clear();
    lcd.print("Incendio!!!");
    EncenderServo(600);
  }
}