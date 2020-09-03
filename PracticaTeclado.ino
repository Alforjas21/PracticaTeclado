//Codigo Principal Proyecto extrusor

//Pines
#define PIN_termo1 5
#define PIN_termo2 6
#define PIN_termo3 7
#define CONFIG_TERMO1_SCK_PIN      32  // SPI SCK
#define CONFIG_TERMO1_CS_PIN       33  // SPI CS
#define CONFIG_TERMO1_DO_PIN       34  // SPI 
#define CONFIG_TERMO2_SCK_PIN      35  // SPI SCK
#define CONFIG_TERMO2_CS_PIN       36  // SPI CS
#define CONFIG_TERMO2_DO_PIN       37  // SPI MISO
#define CONFIG_TERMO3_SCK_PIN      38  // SPI SCK
#define CONFIG_TERMO3_CS_PIN       39  // SPI CS
#define CONFIG_TERMO3_DO_PIN       40  // SPI MISO
#define CONFIG_MOTOR_PSO_PIN       53
#define CONFIG_MOTOR_DIR_PIN       52
//11, 12 & 13 ->Timer1
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

//Delfines
#define RelayON    0
#define RelayOFF   1
#define CONFIG_MOTOR_DIR   1
#define factorConversion 300

#define refrescoProceso    800
#define duracionIntro      2000
#define periodoCalefaccion 1000
#define periodoTimer       4000
#define pantallaDelay      10
#define pasos              200

#define direccionLCD     0x27
#define columnasLCD      20
#define filasLCD         4
#define LCD_FLECHA_IZQUIERDA 0x7F
#define LCD_FLECHA_DERECHA   0x7E
#define LCD_GRADO            0xDF
#define LCD_FLECHA_ARRIBA { 0x00, 0x04, 0x0e, 0x15, 0x04, 0x04, 0x04, 0x00 }
#define LCD_FLECHA_ABAJO  { 0x00, 0x04, 0x04, 0x04, 0x15, 0x0e, 0x04, 0x00 }
#define LCD_FLECHA_ATRAS  { 0x00, 0x16, 0x19, 0x1d, 0x01, 0x11, 0x0e, 0x00 }
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
#include "TimerOne.h"

//Objetos
MAX6675 thermocouple1(CONFIG_TERMO1_SCK_PIN, CONFIG_TERMO1_CS_PIN, CONFIG_TERMO1_DO_PIN);
MAX6675 thermocouple2(CONFIG_TERMO2_SCK_PIN, CONFIG_TERMO2_CS_PIN, CONFIG_TERMO2_DO_PIN);
MAX6675 thermocouple3(CONFIG_TERMO3_SCK_PIN, CONFIG_TERMO3_CS_PIN, CONFIG_TERMO3_DO_PIN);

LiquidCrystal_I2Cmejorado lcd(direccionLCD,columnasLCD,filasLCD);
Teclado4x4 teclado(TECLADO_F1_PIN, TECLADO_F2_PIN, TECLADO_F3_PIN, TECLADO_F4_PIN, TECLADO_C1_PIN, TECLADO_C2_PIN, TECLADO_C3_PIN, TECLADO_C4_PIN, TECLADO_INPUT_STRING);

//variables globales
static String cadenaSalida="                    ";
unsigned long tiempoLoop;
static unsigned long tiempoActual,tiempoActualMicro;
unsigned long tiempoCalefaccion;
unsigned long tiempoMenu;
unsigned long tiempoProcesando;
static unsigned long tiempoMotor,microsMotor, periodoMotor, tiempoMotorActual, periodoMotorDebug;
bool calefaccionON, leerValor, introducirVelocidad, introducirTemperatura, estadoPinMotor, motorRun, consignaVelMax, consignaTempMax;
int8_t valorTermo1, valorTermo2, valorTermo3;
int tempActual1, tempActual2, tempActual3;
int consignaTemp1, consignaTemp2, consignaTemp3;
int8_t pulsacion, pantallaSiguiente, pantallaAnterior;
int8_t estadoMenu, pantallaMenu, estadoAnterior, estadoSiguiente;
char pantallaBuffer[columnasLCD][filasLCD];
unsigned char patronLCDFlechaArriba [8] = LCD_FLECHA_ARRIBA ;
unsigned char patronLCDFlechaAbajo  [8] = LCD_FLECHA_ABAJO ;
unsigned char patronLCDFlechaAtras  [8] = LCD_FLECHA_ATRAS;

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
    digitalWrite(CONFIG_MOTOR_DIR_PIN, CONFIG_MOTOR_DIR);

    //inicilizar TimerOne
    Timer1.initialize(periodoTimer);         
    Timer1.attachInterrupt(motorStep);

    //inicializar Termoresistencias
    pinMode(PIN_termo1,OUTPUT);
    pinMode(PIN_termo2,OUTPUT);
    pinMode(PIN_termo3,OUTPUT);
    digitalWrite(PIN_termo1,RelayOFF);
    digitalWrite(PIN_termo2,RelayOFF);
    digitalWrite(PIN_termo3,RelayOFF);

    //cargar caracteres especiales LCD
    lcd.createChar(0,patronLCDFlechaArriba);
    lcd.createChar(1,patronLCDFlechaAbajo);
    lcd.createChar(2,patronLCDFlechaAtras);

    //inicializar variables
    valorTermo1,valorTermo2, valorTermo3, consignaTemp1, consignaTemp2, 
    consignaTemp3, calefaccionON, leerValor, estadoMenu, consignaVelocidad = 0;
    preset[0] = {"PLA  ",220,60};
    preset[1] = {"ABS  ",260,60};
    preset[2] = {"PET  ",250,60};
    preset[3] = {"NYLON",260,60};
    preset[4] = {"PC   ",310,60};
    preset[5] = {"PP   ",220,60};
    
    //Estado inicial
    introMenu();
}
void loop(){
    //Actualizar tiempoActual
    tiempoActual = millis();

   //Gestionar termopares
    if (calefaccionON == 1){
         if (periodoCalefaccion <=(tiempoActual-tiempoCalefaccion)){
            tiempoCalefaccion = tiempoActual;
            tempActual1 = thermocouple1.readCelsius();
            tempActual2 = thermocouple2.readCelsius();
            tempActual3 = thermocouple3.readCelsius(); 
            
            if ((tempActual1 > consignaTemp1) && (valorTermo1)){
                //Apagamos la resistencia1
                digitalWrite(PIN_termo1,RelayOFF);
                valorTermo1 = false;
            }
            if ((tempActual1 <= consignaTemp1) && (!valorTermo1)){
                //Encendemos la resistencia1
                digitalWrite(PIN_termo1,RelayON);
                valorTermo1 = true;
            }
            if ((tempActual2 > consignaTemp2) && (valorTermo2)){
                //Apagamos la resistencia1
                digitalWrite(PIN_termo2,RelayOFF);
                valorTermo2 = false;
            }
            if ((tempActual2 <= consignaTemp2) && (valorTermo2)){
                //Encendemos la resistencia1
                digitalWrite(PIN_termo2,RelayON);
                valorTermo2 = true;
            }
            if ((tempActual3 > consignaTemp3) && (valorTermo3)){
                //Apagamos la resistencia1
                digitalWrite(PIN_termo3,RelayOFF);
                valorTermo3 = false;
            }
            if ((tempActual3 <= consignaTemp3) && (!valorTermo3)){
                //Encendemos la resistencia1
                digitalWrite(PIN_termo3,RelayON);
                valorTermo3 = true;
            }
        }
    }
    if (!calefaccionON){
        //Apagamos la resistencia 1, 2 y 3
        if (valorTermo1){ 
            digitalWrite(PIN_termo1,RelayOFF);
            valorTermo1 = false;
        }
        if (valorTermo2){ 
            digitalWrite(PIN_termo2,RelayOFF);
            valorTermo2 = false;
        }
        if (valorTermo3){ 
            digitalWrite(PIN_termo3,RelayOFF);
            valorTermo3 = false;
        }     
    }

    //Gestionar entrada usuario (teclado)
    pulsacion=teclado.comprueba();
    if (leerValor) {
        switch(pulsacion){ 
            case '0':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10;
                    lcd.print(0);
                }
            break;
            case '1':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10 +1;
                    lcd.print(1);
                }
            break;
            case '2':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10 +2;
                    lcd.print(2);
                }
            break;
            case '3':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10 +3;
                    lcd.print(3);
                }
            break;
            case '4':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10 +4;
                    lcd.print(4);
                }
            break;
            case '5':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10 +5;
                    lcd.print(5);
                }
            break;
            case '6':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10 +6;
                    lcd.print(6);
                }
            break;
            case '7':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10 +7;
                    lcd.print(7);
                }
            break;
            case '8':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10 +8;
                    lcd.print(8);
                }
            break;
            case '9':
                if (nEntrada >= 100){
                    nEntrada = nEntrada;
                }
                else{
                    nEntrada = nEntrada*10 +9;
                    lcd.print(9);
                }
            break;
            case TECLADO_BORRAR:              
                nEntrada = nEntrada / 10;
                lcd.setCursor(0,2);
                lcd.print("      ");
                lcd.setCursor(0,2);
                if (nEntrada > 0) lcd.print(nEntrada);
            break;
            case TECLADO_ENTER:              
                estadoMenu = estadoSiguiente;
                if (introducirVelocidad){
                    if (nEntrada > 60){
                        consignaVelocidad = 5;
                        introducirVelocidad = 0;
                    }
                    else{
                        consignaVelocidad = factorConversion/nEntrada;  
                        introducirVelocidad = 0;
                    }
                }
                if (introducirTemperatura){
                    if (nEntrada > 350){
                        consignaTemp1 = 350;
                        consignaTemp2 = 350;
                        consignaTemp3 = 350;
                        introducirTemperatura = 0;
                    }
                    else{
                        consignaTemp1 = nEntrada;
                        consignaTemp2 = nEntrada;
                        consignaTemp3 = nEntrada;  
                        introducirTemperatura = 0;
                    }
                }
                nEntrada = 0;
                lcd.clearNoDelay();
                pantallaMenu = pantallaSiguiente;
                tiempoMenu = tiempoActual;
                leerValor = 0;
                pulsacion = 0;
            break;
            case TECLADO_ATRAS:
                lcd.noBlink();
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
                lcd.print("SELECCION DE MODO:");
                lcd.setCursor(0,2);
                lcd.print("AUTOMATICO > Pulse 1");
                lcd.setCursor(0,3);
                lcd.print("MANUAL     > Pulse 2");
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
            }
        break;
        case 2:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("AUTO:");

                lcd.setCursor(0,1);
                lcd.print((char)LCD_FLECHA_DERECHA);
                lcd.print(i+1);
                lcd.print(" ");
                lcd.print(preset[i].nombre);

                lcd.setCursor(0,2);
                lcd.print(i+2);
                lcd.print(" ");
                lcd.print(preset[i+1].nombre);

                lcd.setCursor(0,3);
                lcd.print(i+3);
                lcd.print(" ");
                lcd.print(preset[i+2].nombre);

                lcd.setCursor(9,0);
                lcd.print("SUBIR > ");
                lcd.write(byte(0));
                lcd.setCursor(9,1);
                lcd.print("BAJAR > ");
                lcd.write(byte(1));
                lcd.setCursor(9,2);
                lcd.print("LISTO > ENT");
                lcd.setCursor(9,3);
                lcd.print("VOLVER> ");
                lcd.write(byte(2));
                lcd.setCursor(0,2);

                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
                i = 0;
            }
            /*lcd.setCursor(0,2);
                lcd.print((char)LCD_FLECHA_DERECHA);
                lcd.print(i+1);
                lcd.print(" ");
                lcd.print(preset[i].nombre);*/
        break;
        case 3:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("INTRODUCIR VEL(rpm):");
                lcd.setCursor(6,1);
                lcd.print("BORRAR  > ");
                lcd.print((char)LCD_FLECHA_IZQUIERDA);
                lcd.setCursor(6,2);
                lcd.print("ACEPTAR > ENT");
                lcd.setCursor(6,3);
                lcd.print("VOLVER  > ");
                lcd.write(byte(2));
                lcd.setCursor(0,2);
                //lcd.cursor();
                lcd.blink();
                leerValor = 1;
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
            }
        break;
        case 4:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("INTRODUCIR TEMP(C");
                lcd.print((char)LCD_GRADO);
                lcd.print("):");
                lcd.setCursor(6,1);
                lcd.print("BORRAR  > ");
                lcd.print((char)LCD_FLECHA_IZQUIERDA);
                lcd.setCursor(6,2);
                lcd.print("ACEPTAR > ENT");
                lcd.setCursor(6,3);
                lcd.print("VOLVER  > ");
                lcd.write(byte(2));
                lcd.setCursor(0,2);
                leerValor = 1;
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
            }
        break;
        case 5:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.noBlink();
                lcd.homeNoDelay();
                lcd.print("INICIAR PROCESO?");
                lcd.setCursor(0,2);
                lcd.print("SI  > Pulse 1");
                lcd.setCursor(0,3);
                lcd.print("NO  > Pulse 2");
                tiempoMenu = tiempoActual;
                pantallaMenu = -1;
            }
        break;
        case 6:
            if (tiempoActual - tiempoProcesando >= refrescoProceso){
                lcd.homeNoDelay();
                lcd.print("TEMP OBJETIVO: ");
                lcd.print(consignaTemp1);
                lcd.print((char)LCD_GRADO);
                lcd.print("C");
                lcd.setCursor(0,1);
                lcd.print("T1 = ");
                lcd.print(tempActual1);
                lcd.print((char)LCD_GRADO);
                lcd.print("C");
                lcd.setCursor(12,1);
                lcd.print("VEL = ");
                lcd.print(factorConversion/consignaVelocidad);
                lcd.setCursor(0,2);
                lcd.print("T2 = ");
                lcd.print(tempActual2);
                lcd.print((char)LCD_GRADO);
                lcd.print("C");
                lcd.setCursor(0,3);
                lcd.print("T3 = ");
                lcd.print(tempActual3);
                lcd.print((char)LCD_GRADO);
                lcd.print("C");
                lcd.setCursor(11,3);
                lcd.print("FIN > ENT");
                tiempoMenu       = tiempoActual;
                tiempoProcesando = tiempoActual;
                pantallaMenu = -1;
            }       
        break;
        case 8:
            if (tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.homeNoDelay();
                lcd.print("REANUDAR PROCESO?");
                lcd.setCursor(0,2);
                lcd.print("SI  > Pulse 1");
                lcd.setCursor(0,3);
                lcd.print("NO  > Pulse 2");
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

                lcd.print((char)LCD_FLECHA_DERECHA);
                lcd.print(i+1);
                lcd.print(" ");
                lcd.print(preset[i].nombre);

                if (i == 5){
                    lcd.setCursor(0,2);
                    lcd.print(i-4);
                    lcd.print(" ");
                    lcd.print(preset[i-5].nombre);
                }
                if (i < 5){
                    lcd.setCursor(0,2);
                    lcd.print(i+2);
                    lcd.print(" ");
                    lcd.print(preset[i+1].nombre);
                }
                if (i >= 4){
                    lcd.setCursor(0,3);
                    lcd.print(i-3);
                    lcd.print(" ");
                    lcd.print(preset[i-4].nombre);
                }
                if (i < 4) {
                    lcd.setCursor(0,3);
                    lcd.print(i+3);
                    lcd.print(" ");
                    lcd.print(preset[i+2].nombre);
                }
                tiempoMenu = tiempoActual;
            }
            if(pulsacion == 'b' && tiempoActual - tiempoMenu >= pantallaDelay) {
                lcd.setCursor(0,1);
                i++;
                i = i%6;
                lcd.print((char)LCD_FLECHA_DERECHA);
                lcd.print(i+1);
                lcd.print(" ");
                lcd.print(preset[i].nombre);

                if (i >= 5){
                    lcd.setCursor(0,2);
                    lcd.print(i-4);
                    lcd.print(" ");
                    lcd.print(preset[i-5].nombre);
                }
                if (i < 5){
                    lcd.setCursor(0,2);
                    lcd.print(i+2);
                    lcd.print(" ");
                    lcd.print(preset[i+1].nombre);
                }
                if (i >= 4){
                    lcd.setCursor(0,3);
                    lcd.print(i-3);
                    lcd.print(" ");
                    lcd.print(preset[i-4].nombre);
                }
                if (i < 4) {
                    lcd.setCursor(0,3);
                    lcd.print(i+3);
                    lcd.print(" ");
                    lcd.print(preset[i+2].nombre);
                }
                tiempoMenu = tiempoActual;
            }
            if(pulsacion == TECLADO_ENTER) {
                consignaTemp1 = preset[i].consignaTemperatura; 
                consignaTemp2 = preset[i].consignaTemperatura; 
                consignaTemp3 = preset[i].consignaTemperatura;
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
            pulsacion           = 0;
        break;    
        case 4: //MENU MANUAL CONSIGNA TEMPERATURA 
            estadoAnterior        = 3;
            pantallaAnterior      = 3;
            estadoSiguiente       = 5;
            pantallaSiguiente     = 5;
            introducirTemperatura = 1;
            pulsacion             = 0;
        break;
        case 5: //MENU ¿COMENZAR OPERACIÓN MANUAL?
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
            if(pulsacion == 'e') {estadoMenu = 8;
                lcd.clearNoDelay();
                pantallaMenu = 8;
                tiempoMenu = tiempoActual;
            }
            pulsacion     = 0;
            calefaccionON = 1;
            motorRun      = 1;
        break;
        case 8: //PROCESO: DETENEMOS MOTOR Y HEATERS, ¿REANUDAR PROCESO O FINALIZAR?
            calefaccionON = 0;
            motorRun      = 0;
            //detenemos motor
            if(pulsacion == '1') {estadoMenu = 6; //REANUDAR
                lcd.clearNoDelay();
                pantallaMenu = 6;
                tiempoMenu = tiempoActual;
            }
            if(pulsacion == '2') {estadoMenu = 1; //FINALIZAR
                lcd.clearNoDelay();
                consignaVelocidad = 0;
                consignaTemp1     = 0;
                consignaTemp2     = 0;
                consignaTemp3     = 0;
                pantallaMenu      = 1;
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
    lcd.setCursor(2,1);
    //lcd.write(byte (0));
    //lcd.write(byte (1));
    //lcd.write(byte (2));
    //lcd.print((char)LCD_GRADO);
    //lcd.print("\xDF");
    lcd.print("ALLSEAS-Extrusor:");
    lcd.setCursor(2,2);
    lcd.print("Cargando ...");
}
void motorStep() {
    microsMotor=micros();
    if (microsMotor - tiempoMotor >= consignaVelocidad*1000 && motorRun /* && (tempActual1 >= consignaTemp1 -10)*/){
        digitalWrite(CONFIG_MOTOR_PSO_PIN, HIGH);
        delayMicroseconds(100);
        digitalWrite(CONFIG_MOTOR_PSO_PIN, LOW);    
        tiempoMotor=microsMotor;
        periodoMotor=micros()-periodoMotorDebug; //Debug del periodo de Paso
        periodoMotorDebug = microsMotor;
    }
}