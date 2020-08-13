//Codigo Principal Proyecto extrusor

//Pines
#define CALENTADOR1_PIN 7
#define CALENTADOR2_PIN 8
#define CALENTADOR3_PIN 9
#define CONFIG_TERMO1_SCK_PIN      10  // SPI SCK
#define CONFIG_TERMO1_CS_PIN       11  // SPI CS
#define CONFIG_TERMO1_DO_PIN       12  // SPI MISO
#define CONFIG_MOTOR_PSO_PIN    42
#define CONFIG_MOTOR_DIR_PIN    43
//  CONFIG_I2C_PIN1 20
//  CONFIG_I2C_PIN2 21
#define TECLADO_F1_PIN  28
#define TECLADO_F2_PIN  29
#define TECLADO_F3_PIN  23
#define TECLADO_F4_PIN  22
#define TECLADO_C1_PIN  26
#define TECLADO_C2_PIN  27
#define TECLADO_C3_PIN  24
#define TECLADO_C4_PIN  25


//Defines
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
#define TECLADO_INPUT_STRING "789s456b123d.0ei"

//Inclusiones
#include "max6675.h"
#include <Wire.h> 
#include "LiquidCrystal_I2Cmejorado.h"
#include "TECLADO.h"

//Instanciacion de objetos
MAX6675 thermocouple1(CONFIG_TERMO1_SCK_PIN, CONFIG_TERMO1_CS_PIN, CONFIG_TERMO1_DO_PIN);
MAX6675 thermocouple2(CONFIG_TERMO1_SCK_PIN, CONFIG_TERMO1_CS_PIN, CONFIG_TERMO1_DO_PIN);
MAX6675 thermocouple3(CONFIG_TERMO1_SCK_PIN, CONFIG_TERMO1_CS_PIN, CONFIG_TERMO1_DO_PIN);

LiquidCrystal_I2Cmejorado lcd(0x27,16,2);
Teclado4x4 teclado(TECLADO_F1_PIN, TECLADO_F2_PIN, TECLADO_F3_PIN, TECLADO_F4_PIN, TECLADO_C1_PIN, TECLADO_C2_PIN, TECLADO_C3_PIN, TECLADO_C4_PIN, TECLADO_INPUT_STRING);

//Valores constantes


//variables globales 
unsigned long APS;
unsigned long tiempoActual;
unsigned long tiempoCalefaccion;
unsigned long tiempoMenu;
bool calefaccionON, tecladoON, introducirVelocidad, introducirTemperatura;
int8_t valorTermo1, valorTermo2, valorTermo3;
float tempActual1, tempActual2, tempActual3;
float consignaTemp1, consignaTemp2, consignaTemp3;
int8_t pulsacion, pantallaSiguiente, pantallaAnterior;
int8_t estadoMenu, pantallaMenu, estadoAnterior, estadoSiguiente;
//valor en milisegundos del retardo entre pasos del motor
int consignaVelocidad, nEntrada; 





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
    if (calefaccionON){
         if (periodoCalefaccion <=(tiempoActual-tiempoCalefaccion)){
            tiempoCalefaccion = tiempoActual;
            tempActual1 = thermocouple1.readCelsius();
            tempActual2 = thermocouple2.readCelsius();
            tempActual3 = thermocouple3.readCelsius(); 
            
            if ((tempActual1 > consignaTemp1) && (valorTermo1== HIGH )){
                //Apagamos la resistencia1
                digitalWrite(CALENTADOR1_PIN,LOW);
                valorTermo1 = LOW;
            }
            if ((tempActual1 <= consignaTemp1) && (valorTermo1 == LOW )){
                //Encendemos la resistencia1
                digitalWrite(CALENTADOR1_PIN,HIGH);
                valorTermo1 = HIGH;
            }
            if ((tempActual2 > consignaTemp2) && (valorTermo2 == HIGH )){
                //Apagamos la resistencia1
                digitalWrite(CALENTADOR2_PIN,LOW);
                valorTermo2 = LOW;
            }
            if ((tempActual2 <= consignaTemp2) && (valorTermo2 == LOW )){
                //Encendemos la resistencia1
                digitalWrite(CALENTADOR2_PIN,HIGH);
                valorTermo2 = HIGH;
            }
            if ((tempActual3 > consignaTemp3) && (valorTermo3 == HIGH )){
                //Apagamos la resistencia1
                digitalWrite(CALENTADOR3_PIN,LOW);
                valorTermo3 = LOW;
            }
            if ((tempActual3 <= consignaTemp3) && (valorTermo3 == LOW )){
                //Encendemos la resistencia1
                digitalWrite(CALENTADOR3_PIN,HIGH);
                valorTermo3 = HIGH;
            }
        }
    }
    else {
        //Apagamos la resistencia 1, 2 y 3 si procede
        if (valorTermo1 != LOW){ 
            digitalWrite(CALENTADOR1_PIN,LOW);
            valorTermo1 = LOW;
            }
        if (valorTermo2 != LOW){ 
            digitalWrite(CALENTADOR2_PIN,LOW);
            valorTermo2 = LOW;
            }
        if (valorTermo3 != LOW){ 
            digitalWrite(CALENTADOR3_PIN,LOW);
            valorTermo3 = LOW;
            }     
    }
    
    //Gestionar motor

    //Gestionar entrada usuario(Serie)

    //Gestionar entrada usuario (teclado)
    pulsacion=teclado.comprueba();
    if (tecladoON) {
        switch(pulsacion){ 
            case TECLADO_ATRAS:
                estadoMenu = estadoAnterior;
                lcd.clearNoDelay();
                pantallaMenu = pantallaAnterior;
                tecladoON = 0;
            break;      
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
                if (nEntrada > 0) lcd.print(nEntrada);
            break;
            case TECLADO_ENTER:
                estadoMenu = estadoSiguiente;
                if (introducirVelocidad){
                consignaVelocidad = factorConversion/nEntrada;
                introducirVelocidad = 0;
                }
                if (introducirTemperatura){
                consignaTemp1, consignaTemp2, consignaTemp3 = nEntrada;
                introducirTemperatura = 0;
                }
                nEntrada = 0;
                lcd.clearNoDelay();
                pantallaMenu = pantallaSiguiente;
                tecladoON = 0;
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
                lcd.print("Menu AUTO");
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
        case 5:


        break;
    }
    //Gestionar flujo de menu
    switch(estadoMenu) {
        case 0: //INTRODUCCION CON ESPERA
            if (tiempoActual-tiempoMenu >= duracionIntro){
                lcd.clearNoDelay();
                pantallaMenu = 1;
                tiempoMenu = tiempoActual;
                estadoMenu = 1;
            } 
        break;
        case 1: //MENU SELECCION AUTO-MANUAL
            if(pulsacion == '1') {
                lcd.clearNoDelay();
                pantallaMenu,estadoMenu = 2;
                pulsacion = 0;
                tiempoMenu = tiempoActual;
            }
            if(pulsacion == '2') {
                lcd.clearNoDelay();
                pantallaMenu, estadoMenu = 3;
                pulsacion = 0;
                tiempoMenu = tiempoActual;
            }
        break;
        case 2: //MENU AUTO
            estadoAnterior   = 1;
            pantallaAnterior = 1;
            tecladoON = 1;
        break;
        case 3: //MENU MANUAL CONSIGNA VELOCIDAD MAX:14 caracteres
            estadoAnterior = 1;
            pantallaAnterior = 1;
            estadoSiguiente = 4;
            pantallaSiguiente = 4;
            introducirVelocidad = 1;
            tecladoON = 1;
        case 4: //MENU MANUAL CONSIGNA TEMPERATURA 
            estadoAnterior = 3;
            pantallaAnterior = 3;
            estadoSiguiente = 5;
            pantallaSiguiente = 5;
            introducirTemperatura = 1;
            tecladoON = 1;
        break;
        case 5: //MENU ¿COMENZAR OPERACIÓN?
            estadoAnterior = 1;
            pantallaAnterior = 1;
            estadoSiguiente = 6;
            pantallaSiguiente = 6;
            tecladoON = 1;
        break;
        case 6: 
            estadoAnterior = 1;
            pantallaAnterior = 1;
            estadoSiguiente = 1;
            pantallaSiguiente = 1;
            tecladoON = 1;
        break;        
        case 7: 
            estadoAnterior = 1;
            pantallaAnterior = 1;
            estadoSiguiente = 1;
            pantallaSiguiente = 1;
            tecladoON = 1;
        break;        
        case 8: 
            estadoAnterior = 1;
            pantallaAnterior = 1;
            estadoSiguiente = 1;
            pantallaSiguiente = 1;
            tecladoON = 1;
        break;        
        case 9: 
            estadoAnterior = 1;
            pantallaAnterior = 1;
            estadoSiguiente = 1;
            pantallaSiguiente = 1;
            tecladoON = 1;
        break;        
        case 10: 
            estadoAnterior = 1;
            pantallaAnterior = 1;
            estadoSiguiente = 1;
            pantallaSiguiente = 1;
            tecladoON = 1;
        break;
    }

}

//Funciones

/////////////////////Pantalla inicial //////////////////////////////
void introMenu(){ 
 lcd.clear();
 lcd.setCursor(2,0);
 lcd.print("Garci-Extrusor:");
 lcd.setCursor(1,1);
 lcd.print("Cargando...");
}