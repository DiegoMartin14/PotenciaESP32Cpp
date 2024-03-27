#include <Arduino.h>
#include <driver/adc.h>

// Definición de pines
const int TEMP_SENSOR_PIN = 36; // Pin del sensor de temperatura
const int PWM_OUTPUT_PIN = 2; // Pin de salida PWM
const int INTERRUPT_PIN = 4; // Pin de la interrupción externa

// Canales PWM y ADC
const int PWM_CHANNEL = 0;  // Canal PWM
const int ADC_CHANNEL_TEMP = ADC1_CHANNEL_0;    // Canal ADC para sensor de temperatura

// Variables globales
volatile int Comenzar = 0;
volatile int Resultado = 0;
volatile int porcentaje;
int Hist = 0;
int Actual = 0;
float rango = 0.11;
float multiplicar = 0;
int Set = 0;
volatile int Contar = 0;
const int TOP = 201;
short adc_valor;
float divisor = 9.31;
float temp;
char Caracter = 'f';
char Menu = 'f';
char entrar = 'f';
int bandaBaja;
int bandaAlta;
int Paso;
void IRAM_ATTR isr_int0();

void setup() {
    Serial.begin(9600);

    // Configuración de los pines de entrada analógica
    analogReadResolution(10); // Resolución de 10 bits
    analogSetAttenuation(ADC_11db); // Tensión de referencia de 3.3V

    // Configuración de la salida PWM
    ledcSetup(PWM_CHANNEL, 9900, 8); // Canal 0, frecuencia de 9900 Hz, resolución de 8 bits
    ledcAttachPin(PWM_OUTPUT_PIN, PWM_CHANNEL); // Asigna el pin GPIO 2 al canal 0

    // Configuración de la interrupción externa
    pinMode(INTERRUPT_PIN, INPUT_PULLDOWN); // Configura el pin INTERRUPT_PIN como entrada con pull-down
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), isr_int0, RISING); // Interrupción en flanco ascendente en el pin INTERRUPT_PIN
}

void loop() {

  if (Serial.available() > 0) { // Verificar si hay datos disponibles para leer
    Caracter = Serial.read(); // Leer un caracter desde el monitor serie
  }  
  if (Caracter == 't'){
    //Serial.write(12);
    Serial.print("Temperatura del sensor:");
    Serial.println(Actual);
    Serial.println(bandaAlta);
    Serial.println(bandaBaja);
    Caracter = 'f';
  }
  
    if (Caracter == 'm'){
        Serial.println("Menu seteo:");
        Serial.println("S = Setear temperatura");
        Serial.println("H = Setear Histeresis");
    }

    while (Caracter == 'm'){
        if (Serial.available() > 0) { // Verificar si hay datos disponibles para leer
            entrar = Serial.read(); // Leer un caracter desde el monitor serie
            Caracter = 'f';
        }        
    }

    if (entrar == 'h'){
        Serial.println("Setea la histeresis:");
        while (entrar == 'h'){
            
            if (Serial.available() > 0){
            Hist = Serial.parseInt();
            entrar = 'p';
            }
        while (entrar == 'p'){    
        Serial.print("histeresis Seteada:");
        Serial.println(Hist); 
        entrar = 'f';
        }   
        }
    }

        
     if (entrar == 's'){
        int multiplicar;
        Serial.println("Setea la temperatura:");
        while (entrar == 's'){
            
            if (Serial.available() > 0){
            multiplicar = Serial.parseInt();
            multiplicar = multiplicar * 10;
            entrar = 'l';
            }
        }
            while (entrar == 'l'){
                int sumar;
                if (Serial.available() > 0){
                sumar = Serial.parseInt();
                Set = multiplicar + sumar;
                Set = Set / 10;
                entrar = 'f';
                }
            }
        Serial.print("temperatura Seteada:");
        Serial.println(Set);    
 
     }
        
    multiplicar = analogRead(TEMP_SENSOR_PIN);
    Actual = multiplicar * rango;
    if (Actual > 110){
        Actual = 110;
    }
    
    // Procesamiento del control de temperatura con histeresis
    if (Comenzar == 1) {
        bandaAlta = Set + Hist;
        bandaBaja = Set - Hist;
        Resultado = bandaAlta - Actual;

            if (Actual > bandaAlta){
                Contar = 0;
                ledcWrite(PWM_CHANNEL, Contar);
                Paso = 1;
            }
            
            if (bandaAlta >= Actual >= bandaBaja ){
                if (Paso == 1){
                    Contar = 0;
                    ledcWrite(PWM_CHANNEL, Contar);
                     
                }
                Resultado *= 100;
                porcentaje = Resultado / bandaAlta;
                porcentaje *= TOP;
                Contar = porcentaje / 100;
                ledcWrite(PWM_CHANNEL, Contar); // Establece el valor de duty cycle
            }

            if(Actual < bandaBaja){
                Paso = 0;
                Resultado *= 100;
                porcentaje = Resultado / bandaAlta;
                porcentaje *= TOP;
                Contar = porcentaje / 100;
                ledcWrite(PWM_CHANNEL, Contar); // Establece el valor de duty cycle
                }
                Comenzar = 0;                                 
            }       
        }



void IRAM_ATTR isr_int0(){
    Comenzar = 1;
    //Serial.println("Interrupción detectada.");
}

