//Codigo Principal Proyecto extrusor

//Define
#define PIN_termo1 7
#define PIN_termo2 8
#define PIN_termo3 9
#define CONFIG_TCSCK_PIN      10  // SPI SCK
#define CONFIG_TCCS_PIN       11  // SPI CS
#define CONFIG_TCDO_PIN       12  // SPI MISO

#define factorConversion 300

#define duracionIntro 4000
#define periodoCalefaccion 1000
#define debugMenu 1
#define pantallaDelay 2


#define FLECHA_ARRIBA 0x5E
#define FLECHA_ABAJO 0x76
#define FLECHA_IZQUIERDA 0x7F
#define FLECHA_DERECHA 0x7E
#define TECLADO_ENTER 'e'
#define TECLADO_ATRAS 's'
#define TECLADO_BORRAR 'i'
#define TECLADO_ABAJO 'b'
#define TECLADO_PUNTO '.'

//includes
#include "max6675.h"
#include <Wire.h> 
#include "LiquidCrystal_I2Cmejorado.h"
#include "TECLADO.h"

//Objetos
MAX6675 thermocouple1(CONFIG_TCSCK_PIN, CONFIG_TCCS_PIN, CONFIG_TCDO_PIN);
MAX6675 thermocouple2(CONFIG_TCSCK_PIN, CONFIG_TCCS_PIN, CONFIG_TCDO_PIN);
MAX6675 thermocouple3(CONFIG_TCSCK_PIN, CONFIG_TCCS_PIN, CONFIG_TCDO_PIN);

LiquidCrystal_I2Cmejorado lcd(0x27,16,2);
Teclado4x4 teclado(28, 29, 23, 22, 26, 27, 24, 25, "789s456b123d.0ei");

//Valores estáticos


//variables globales 
unsigned long APS;
unsigned long tiempoActual;
unsigned long tiempoCalefaccion;
unsigned long tiempoMenu;
bool calefaccionON, tecladoON;
int8_t valorTermo1, valorTermo2, valorTermo3;
float tempActual1, tempActual2, tempActual3;
float consignaTemp1, consignaTemp2, consignaTemp3;
int8_t pulsacion, nEntrada;
int8_t estadoMenu, pantallaMenu;
//valor en milisegundos del retardo entre pasos del motor
int consignaVelocidad; 





void setup(){
    //inicializar Objetos
    lcd.init();
    lcd.backlight();
    Serial.begin(9600);
    teclado.configura();

    //inicializar variables
    valorTermo1,valorTermo2, valorTermo3, consignaTemp1, consignaTemp2, 
    consignaTemp3, calefaccionON,tecladoON, estadoMenu, consignaVelocidad = 0;
    
    pinMode(LED_BUILTIN,OUTPUT);
    
    //Estado inicial
    introMenu();
}
void loop(){
    //if (digitalRead (LED_BUILTIN)>0) {digitalWrite(LED_BUILTIN,LOW);}
    //else {digitalWrite(LED_BUILTIN,HIGH);}
    //Serial.println (tiempoActual);
    //delay (100);



    //guardar tiempo por loop (APS)
    APS = (millis() - tiempoActual);
    if (Serial) {
        Serial.print ("Tiempo LOOP: ");
        Serial.println (APS);
    }

    //registrar tiempo actual
    tiempoActual = millis();
    
    //control de errores: variables
    if (Serial){
        Serial.print ("estadoMenu: ");
        Serial.print (estadoMenu);
        Serial.print ("  pantallaMenu: ");
        Serial.print (pantallaMenu);
        Serial.print ("  pulsacion: ");
        Serial.println (pulsacion);
        

    }


    //Gestionar termopares
    if (calefaccionON == 1){
         if (periodoCalefaccion <=(tiempoActual-tiempoCalefaccion)){
            tiempoCalefaccion = tiempoActual;
            tempActual1 = thermocouple1.readCelsius();
            tempActual2 = thermocouple2.readCelsius();
            tempActual3 = thermocouple3.readCelsius(); 
            
            if ((tempActual1 > consignaTemp1) && (valorTermo1== HIGH )){
                //Apagamos la resistencia1
                digitalWrite(PIN_termo1,LOW);
                valorTermo1 = LOW;
            }
            if ((tempActual1 <= consignaTemp1) && (valorTermo1 == LOW )){
                //Encendemos la resistencia1
                digitalWrite(PIN_termo1,HIGH);
                valorTermo1 = HIGH;
            }
            if ((tempActual2 > consignaTemp2) && (valorTermo2 == HIGH )){
                //Apagamos la resistencia1
                digitalWrite(PIN_termo2,LOW);
                valorTermo2 = LOW;
            }
            if ((tempActual2 <= consignaTemp2) && (valorTermo2 == LOW )){
                //Encendemos la resistencia1
                digitalWrite(PIN_termo2,HIGH);
                valorTermo2 = HIGH;
            }
            if ((tempActual3 > consignaTemp3) && (valorTermo3 == HIGH )){
                //Apagamos la resistencia1
                digitalWrite(PIN_termo3,LOW);
                valorTermo3 = LOW;
            }
            if ((tempActual3 <= consignaTemp3) && (valorTermo3 == LOW )){
                //Encendemos la resistencia1
                digitalWrite(PIN_termo3,HIGH);
                valorTermo3 = HIGH;
            }
        }
    }
    if (calefaccionON == 0){
        //Apagamos la resistencia 1, 2 y 3
        if (valorTermo1 != LOW){ 
            digitalWrite(PIN_termo1,LOW);
            valorTermo1 = LOW;
            }
        if (valorTermo2 != LOW){ 
            digitalWrite(PIN_termo2,LOW);
            valorTermo2 = LOW;
            }
        if (valorTermo3 != LOW){ 
            digitalWrite(PIN_termo3,LOW);
            valorTermo3 = LOW;
            }     
    }
    
    //Gestionar motor
    //Gestionar entrada usuario(Serie)
    //Gestionar entrada usuario (teclado)
    pulsacion=teclado.comprueba();
    if (tecladoON) {
    switch(pulsacion){       
                case '0':
                    nEntrada = nEntrada*10;
                    lcd.print(0);
                break;
                case '1':
                    nEntrada = nEntrada*10 +1;
                    lcd.print(1);
                break;
                case '2':
                    nEntrada = nEntrada*10 +2;
                    lcd.print(2);
                break;
                case '3':
                    nEntrada = nEntrada*10 +3;
                    lcd.print(3);
                break;
                case '4':
                    nEntrada = nEntrada*10 +4;
                    lcd.print(4);
                break;
                case '5':
                    nEntrada = nEntrada*10 +5;
                    lcd.print(5);
                break;
                case '6':
                    nEntrada = nEntrada*10 +6;
                    lcd.print(6);
                break;
                case '7':
                    nEntrada = nEntrada*10 +7;
                    lcd.print(7);
                break;
                case '8':
                    nEntrada = nEntrada*10 +8;
                    lcd.print(8);
                break;
                case '9':
                    nEntrada = nEntrada*10 +9;
                    lcd.print(9);
                break;
                case TECLADO_BORRAR:              
                    nEntrada = nEntrada / 10;
                    lcd.setCursor(0,1);
                    lcd.print("        ");
                    lcd.setCursor(0,1);
                    lcd.print(nEntrada);
                break;
            }
        tiempoMenu = tiempoActual;
        pulsacion = 0;
    }
    

    //Gestionar salida pantalla
    switch(pantallaMenu) {
        case 1:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("AUTO   >1");
                lcd.setCursor(0,1);
                lcd.print("MANUAL >2");
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
                
            }
        break;
        case 2:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
            }
        break;
        case 3:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("Introduce vel:");
                lcd.setCursor(10,1);
                lcd.print("BACK");
                lcd.print(FLECHA_ARRIBA);
                lcd.setCursor(0,1);
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
            }
        break;
        case 4:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("Introduce temp:");
                lcd.setCursor(10,1);
                lcd.print("BACK");
                lcd.print(FLECHA_ARRIBA);
                lcd.setCursor(0,1);
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
            }

        break;
    }
    switch(estadoMenu) {
        case 0: //INTRODUCCION CON ESPERA
            if (tiempoActual-tiempoMenu >= duracionIntro){
                estadoMenu = 1;
                lcd.clearNoDelay();
                pantallaMenu = 1;
                tiempoMenu = tiempoActual;
            } 
        break;
        case 1: //MENU SELECCION AUTO-MANUAL
            
            if(pulsacion == '1') {estadoMenu = 2;
            lcd.clearNoDelay();
            pantallaMenu = 2;
            tiempoMenu = tiempoActual;
            }
            if(pulsacion == '2') {estadoMenu = 3;
            lcd.clearNoDelay();
            pantallaMenu = 3;
            tiempoMenu = tiempoActual;
            tecladoON = 1;
            }
            pulsacion = 0;
        break;
        case 2: //MENU AUTO

            if(pulsacion == TECLADO_ATRAS) {estadoMenu = 1;
            lcd.clearNoDelay();
            pantallaMenu = 1;
            tiempoMenu = tiempoActual;
            }
            //selección de distintos polímeros
            pulsacion = 0;
        break;
        case 3: //MENU MANUAL CONSIGNA VELOCIDAD MAX:14 caracteres
            
            if(pulsacion == TECLADO_ATRAS) {estadoMenu = 1;
            lcd.clearNoDelay();
            pantallaMenu = 1;
            tiempoMenu = tiempoActual;
            }
            if(pulsacion == TECLADO_ENTER) {estadoMenu = 4;
            consignaVelocidad = factorConversion/nEntrada;  
            nEntrada = 0;
            lcd.clearNoDelay();
            pantallaMenu = 4;
            tiempoMenu = tiempoActual;
            }

            pulsacion = 0;
            
        case 4:
            pulsacion = 0;
        break;
        case 5:
            pulsacion = 0;
        break;
        case 6:
            pulsacion = 0;
        break;
        case 7:
            pulsacion = 0;
        break;
        case 8:
            pulsacion = 0;
        break;
        case 9:
            pulsacion = 0;
        break;
        case 10:
            pulsacion = 0;
        break;    
    }

}



/////////////////////Pantalla inicial //////////////////////////////
void introMenu(){ 
 lcd.clear();  // Borra el  LCD
 lcd.setCursor(2,0);         // Se posiciona en la Columna 3, Fila 0
 lcd.print("Garci-Extrusor:");
 lcd.setCursor(1,1);
 lcd.print("Cargando...");
}
/////////////////////Menu_1  //////////////////////////////////
/* void menu1(){ 
   lcd.homeNoDelay();
   lcd.print("ON    >1  ");
   lcd.setCursor(10,0);
   lcd.print("OFF >2");   
   lcd.setCursor(0,1);
   lcd.print("INTER >3  "); 
   lcd.setCursor(10,1);
   lcd.print("MAS >ENT");
}

/////////////////////Menu_2  //////////////////////////////////
void menu2(){ 
   lcd.homeNoDelay();
   lcd.print("LUZ  >1   ");
   lcd.setCursor(8,0);
   lcd.print("TIEMPO>2");
   lcd.setCursor(0,1);
   lcd.print("MENU >BACK       ");
}
/////////////////////accion_2  //////////////////////////////////
void accion2(){ 
  if(pulsacion == '1') contador=3;
  if(pulsacion == '2') contador=4;
  if(pulsacion == 'i') contador=1;
}
/////////////////////Menu_3  //////////////////////////////////
void menu3(){ 
   lcd.homeNoDelay();
   lcd.print("LUZ :           ");
   lcd.setCursor(5,0);
   lcd.print(analogRead(A0));  
   lcd.setCursor(0,1);
   lcd.print("MENU>BACK   <--0");
}
/////////////////////accion_3  //////////////////////////////////
void accion3(){ 
  if(pulsacion == 'i') contador=1;
  if(pulsacion == '0') contador=2;
}
/////////////////////Menu_4 //////////////////////////////////
void menu4(){ 
   unsigned long tiempo1 = (millis()/1000);  
   lcd.homeNoDelay();
   lcd.print("TIEMPO:            ");
   lcd.setCursor(8,0);
   lcd.print(tiempo1);  
   lcd.setCursor(0,1);
   lcd.print("MENU>BACK   <--0");
}
/////////////////////accion_4  //////////////////////////////////
void accion4(){ 
  if(pulsacion == 'i') contador=1;
  if(pulsacion == '0') contador=2;
}
 */