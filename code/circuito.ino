#include <LiquidCrystal.h> // Para el LCD
#include <IRremote.h> // Para el Control IR
#include <Servo.h>

// Definimos los pines de cada componente 
#define SENSOR_TEMPERATURA A0
#define LED_1 13
#define LED_2 12 
#define CONTROL_IR 11
#define SERVOMOTOR 9

#define limiteTemperatura 50 

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); 
IRrecv irRecibidor(CONTROL_IR); // Indicamos que pin recibirá los datos
Servo servoMotor; 

// Variables globales
int temperatura; // Guarda la temperatura leida por el sensor
bool encendido = false; // Indica si el sistema esta encendido o apagado
String estacion; // Guarda la estacion del año

void setup()
{
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  
  lcd.begin(16, 2); // Inicializa el LCD con 16 columnas y 2 filas
  servoMotor.attach(SERVOMOTOR); // Inicializa el ServoMotor
  irRecibidor.begin(CONTROL_IR, DISABLE_LED_FEEDBACK); // Inicializa el Control IR
}

void loop()
{
  int temperaturaLeida = analogRead(SENSOR_TEMPERATURA); // Recibe un valor de 0 a 1023
  // Mapea el ese valor a un rango de -40 a 125, que son los limites del sensor de temperatura
  temperatura = map(temperaturaLeida, 20, 350, -40, 125); 

  ControlRemotoIR(); // Controla el sistema con el control remoto  

  if (encendido) {
    EjecutarPrograma(temperatura);
  } else {
    digitalWrite(LED_1, LOW);
    lcd.clear(); // Limpiamos el LCD
  }
}

void EjecutarPrograma(int temp){
  EstacionAnio(temp); // Indica la estacion del año segun la temperatura
  AlarmaIncendio(temp);
  ImprimirTemperatura(temp);
  digitalWrite(LED_1, HIGH);
}

void ControlRemotoIR()
{
  if (irRecibidor.decode()) {
    switch (irRecibidor.decodedIRData.decodedRawData) {
      case 0xFF00BF00: // Boton de encendido
        encendido = !encendido;
        break;
      // ... Proximas Funcionalidades 
      default:
        break;
    }
    irRecibidor.resume(); // Recibe el siguiente dato
  }
  delay(120); // Evitar rebotes
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
  else{
    estacion = "";
  }
}
// Imprime la temperatura en el LCD junto a la estacion del año
void ImprimirTemperatura(int temp)
{
  lcd.clear();
  lcd.print("Temp: "+ (String) temp + "C");
  lcd.setCursor(0, 1);
  lcd.print(estacion);
  delay(200);
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
    lcd.setCursor(0, 1);
    lcd.print("Incendio!!!");
    EncenderServo(600);
    lcd.clear();
  }
}