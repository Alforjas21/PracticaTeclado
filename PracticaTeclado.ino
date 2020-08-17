//Codigo Principal Proyecto extrusor

//Pines
#define PIN_termo1 7
#define PIN_termo2 8
#define PIN_termo3 9
#define CONFIG_TERMO1_SCK_PIN      10  // SPI SCK
#define CONFIG_TERMO1_CS_PIN       11  // SPI CS
#define CONFIG_TERMO1_DO_PIN       12  // SPI 
#define CONFIG_TERMO2_SCK_PIN      13  // SPI SCK
#define CONFIG_TERMO2_CS_PIN       14  // SPI CS
#define CONFIG_TERMO2_DO_PIN       15  // SPI MISO
#define CONFIG_TERMO3_SCK_PIN      16  // SPI SCK
#define CONFIG_TERMO3_CS_PIN       17  // SPI CS
#define CONFIG_TERMO3_DO_PIN       18  // SPI MISO
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

#define refrescoProceso 800
#define duracionIntro 4000
#define periodoCalefaccion 1000
#define debugMenu 1
#define pantallaDelay 2
#define pasos 200


#define FLECHA_ARRIBA    0x5E
#define FLECHA_ABAJO     0x76
#define FLECHA_IZQUIERDA 0x7F
#define FLECHA_DERECHA   0x7E
#define TECLADO_ENTER  'e'
#define TECLADO_ATRAS  '.'
#define TECLADO_BORRAR 'i'
#define TECLADO_ABAJO  'b'
#define TECLADO_ARRIBA 's'
#define TECLADO_INPUT_STRING "789s456b123d.0ei"

//includes
#include "max6675.h"
#include <Wire.h> 
#include "LiquidCrystal_I2Cmejorado.h"
#include "TECLADO.h"


//Objetos
MAX6675 thermocouple1(CONFIG_TERMO1_SCK_PIN, CONFIG_TERMO1_CS_PIN, CONFIG_TERMO1_DO_PIN);
MAX6675 thermocouple2(CONFIG_TERMO2_SCK_PIN, CONFIG_TERMO2_CS_PIN, CONFIG_TERMO2_DO_PIN);
MAX6675 thermocouple3(CONFIG_TERMO3_SCK_PIN, CONFIG_TERMO3_CS_PIN, CONFIG_TERMO3_DO_PIN);

LiquidCrystal_I2Cmejorado lcd(0x27,16,2);
Teclado4x4 teclado(TECLADO_F1_PIN, TECLADO_F2_PIN, TECLADO_F3_PIN, TECLADO_F4_PIN, TECLADO_C1_PIN, TECLADO_C2_PIN, TECLADO_C3_PIN, TECLADO_C4_PIN, TECLADO_INPUT_STRING);

//Valores estáticos


//variables globales 
unsigned long APS;
unsigned long tiempoActual;
unsigned long tiempoCalefaccion;
unsigned long tiempoMenu;
unsigned long tiempoProcesando;
bool calefaccionON, leerValor, introducirVelocidad, introducirTemperatura;
int8_t valorTermo1, valorTermo2, valorTermo3;
float tempActual1, tempActual2, tempActual3;
float consignaTemp1, consignaTemp2, consignaTemp3;
int8_t pulsacion, pantallaSiguiente, pantallaAnterior;
int8_t estadoMenu, pantallaMenu, estadoAnterior, estadoSiguiente;
//valor en milisegundos del retardo entre pasos del motor
int consignaVelocidad, nEntrada, velocidad, i;
struct valorPredefinido
{
    String nombre;
    int consignaTemperatura;
    int consignaVelocidad;
};
valorPredefinido preset[6];


void setup(){
    //inicializar Objetos
    lcd.init();
    lcd.backlight();
    Serial.begin(9600);
    teclado.configura();
    
    //inicializar motor
    pinMode(CONFIG_MOTOR_PSO_PIN, OUTPUT);
    pinMode(CONFIG_MOTOR_DIR_PIN, OUTPUT);

    //inicializar variables
    valorTermo1,valorTermo2, valorTermo3, consignaTemp1, consignaTemp2, 
    consignaTemp3, calefaccionON, leerValor, estadoMenu, consignaVelocidad = 0;
    preset[0] = {"PLA           ",220,60};
    preset[1] = {"ABS           ",260,60};
    preset[2] = {"PET           ",250,60};
    preset[3] = {"NYLON         ",260,60};
    preset[4] = {"POLICARBONATO ",310,60};
    preset[5] = {"POLIPROPILENO ",220,60};
    
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

    //digitalWrite(LED_BUILTIN, estadoMenu == 6);

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
    if (leerValor) {
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
                tiempoMenu = tiempoActual;
                leerValor = 0;
                pulsacion = 0; 
            break;
            case TECLADO_ATRAS:
                estadoMenu = estadoAnterior;
                lcd.clearNoDelay();
                pantallaMenu = pantallaAnterior;
                tiempoMenu = tiempoActual;
                leerValor = 0;
                pulsacion = 0;  
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
                lcd.setCursor(0,1);
                lcd.print(i+1);
                lcd.print(" ");
                lcd.print(preset[i].nombre);
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
                i = 0;
            }
        break;
        case 3:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("Introduce vel:");
                lcd.setCursor(10,1);
                lcd.print("BACK");
                lcd.setCursor(0,1);

                leerValor = 1;

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
                lcd.setCursor(0,1);

                leerValor = 1;

                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
            }
        break;
        case 5:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("INICIAR PROCESO?");
                lcd.setCursor(0,1);
                lcd.print("SI>   1|NO>    2");
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
            }
        break;
        case 6:
            if (tiempoActual - tiempoProcesando >= refrescoProceso){
                lcd.homeNoDelay();
                lcd.print("T1:");
                lcd.print(tempActual1);
                lcd.print("  T2:");
                lcd.print(tempActual2);
                lcd.setCursor(0,1);
                lcd.print("T3:");
                lcd.print(tempActual3);
                lcd.print("  V:");
                lcd.print(velocidad);
                lcd.print("FIN");
                tiempoMenu = tiempoActual;
                tiempoProcesando = tiempoActual;
            }       
        break;
        case 7:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("PAUSAR PROCESO?");
                lcd.setCursor(0,1);
                lcd.print("SI>   1|NO>    2");
                tiempoMenu   = tiempoActual;
                pantallaMenu = -1;
            }
        break;
        case 8:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("REANUDAR PROCESO?");
                lcd.setCursor(0,1);
                lcd.print("SI>   1|NO>    2");
                tiempoMenu   = tiempoActual;
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
            //leerValor = 1;
            }
            pulsacion = 0;
        break;
        case 2: //MENU AUTO
            if(pulsacion == TECLADO_ATRAS) {
                estadoMenu = 1;
                lcd.clearNoDelay();
                pantallaMenu = 1;
                tiempoMenu = tiempoActual;
            }
            if(pulsacion == 's' && tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.setCursor(0,1);
                i--;
                if(i == -1) i = 5;
                i = i%6;
                lcd.print(i+1);
                lcd.print(" ");
                lcd.print(preset[i].nombre);
                tiempoMenu = tiempoActual;
            }
            if(pulsacion == 'b' && tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.setCursor(0,1);
                i++;
                i = i%6;
                lcd.print(i+1);
                lcd.print(" ");
                lcd.print(preset[i].nombre);
                tiempoMenu = tiempoActual;
            }
            if(pulsacion == TECLADO_ENTER) {
                consignaTemp1, consignaTemp2, consignaTemp3 = preset[i].consignaTemperatura;
                consignaVelocidad = factorConversion/preset[i].consignaVelocidad;
                estadoMenu = 9;
                lcd.clearNoDelay();
                pantallaMenu = 5;
                tiempoMenu = tiempoActual;
                i = 0;
            }
            pulsacion        = 0;
        break;
        case 3: //MENU MANUAL CONSIGNA VELOCIDAD MAX:14 caracteres
            estadoAnterior      = 1;
            pantallaAnterior    = 1;
            estadoSiguiente     = 4;
            pantallaSiguiente   = 4;
            introducirVelocidad = 1;
            //leerValor         = 1;
            pulsacion           = 0;
        break;    
        case 4: //MENU MANUAL CONSIGNA TEMPERATURA 
            estadoAnterior        = 3;
            pantallaAnterior      = 3;
            estadoSiguiente       = 5;
            pantallaSiguiente     = 5;
            introducirTemperatura = 1;
            //leerValor           = 1;
            pulsacion             = 0;
        break;
        case 5: //MENU ¿COMENZAR OPERACIÓN?
            if(pulsacion == '1') {estadoMenu = 6;
            lcd.clearNoDelay();
            pantallaMenu = 6;
            tiempoMenu = tiempoActual;
            }
            if(pulsacion == '2') {estadoMenu = 4;
            lcd.clearNoDelay();
            pantallaMenu = 4;
            tiempoMenu = tiempoActual;
            }
            pulsacion = 0;
        break;
        case 6: //PROCESO: ARRANCAMOS MOTOR Y HEATERS
            if(pulsacion == 'e') {estadoMenu = 7;
            lcd.clearNoDelay();
            pantallaMenu = 7;
            tiempoMenu = tiempoActual;
            }
            pulsacion     = 0;
            calefaccionON = 1;
        break;
        case 7: //¿PAUSAMOS EL MOTOR Y HEATERS?
            if(pulsacion == '1') {estadoMenu = 8;
                lcd.clearNoDelay();
                pantallaMenu = 8;
                tiempoMenu = tiempoActual;
            }
            if(pulsacion == '2') {estadoMenu = 6;
                lcd.clearNoDelay();
                pantallaMenu = 6;
                tiempoMenu = tiempoActual;
            }
            pulsacion = 0;
        break;
        case 8: //PROCESO: DETENEMOS MOTOR Y HEATERS, ¿REANUDAR PROCESO O FINALIZAR?
            calefaccionON = 0;
            //detenemos motor
            if(pulsacion == '1') {estadoMenu = 6; //FINALIZAR
                lcd.clearNoDelay();
                pantallaMenu = 6;
                tiempoMenu = tiempoActual;
            }
            if(pulsacion == '2') {estadoMenu = 1; //REANUDAR
                lcd.clearNoDelay();
                pantallaMenu = 1;
                tiempoMenu = tiempoActual;
            }
            pulsacion = 0;
        break;
        case 9: //MENU ¿COMENZAR OPERACIÓN AUTO?
            if(pulsacion == '1') {estadoMenu = 6;
            lcd.clearNoDelay();
            pantallaMenu = 6;
            tiempoMenu = tiempoActual;
            }
            if(pulsacion == '2') {estadoMenu = 2;
            lcd.clearNoDelay();
            pantallaMenu = 2;
            tiempoMenu = tiempoActual;
            }
            pulsacion = 0;
        break;
    }
}

/////////////////////Pantalla inicial //////////////////////////////
void introMenu() { 
    lcd.clear();  // Borra el  LCD
    lcd.setCursor(2,0);         // Se posiciona en la Columna 3, Fila 0
    lcd.print("Garci-Extrusor:");
    lcd.setCursor(1,1);
    lcd.print("Cargando...");
}


