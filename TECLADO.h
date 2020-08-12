#ifndef TECLADO4X4_H_
#define TECLADO4X4_H_
#include <Arduino.h>

class Teclado4x4 {
private:
uint8_t _salidasFilas[4], _entradasColumnas[4];
char _caracteres[17];
uint8_t _pulsado, _fila, _columna;
unsigned long _t;
public:
Teclado4x4(uint8_t F1, uint8_t F2, uint8_t F3, uint8_t F4,
uint8_t C1, uint8_t C2, uint8_t C3, uint8_t C4, char const * caracteres);
// Constructor al que le pasamos:
// - F1, F2, F3, F4: salidas digitales de la placa Arduino conectadas a las señales
// de las filas en el teclado
// - C1, C2, C3, C4: entradas digitales de la placa Arduino conectadas a las
// señales de las columnas en el teclado
// - caracteres: La pulsación de una tecla genera un carácter. Se indican los caracteres
// generados por la pulsación de cada una de las 16 teclas, yendo por filas:
// fila 1 columna 1, fila 1 columna 2, ..., fila 1 columna 4, fila 2 columna 1, ...,
// fila 4 columna 4
void configura();
// Método a utilizar en setup() para inicializar las señales de manejo del teclado
char comprueba();
// Método a utilizar en loop() para comprobar si se ha detectado la pulsación de una tecla
// Devuelve:
// - Un buleano falso si no se detectó la pulsación
// - El carácter correspondiente a una tecla si se detectó su pulsación
};
#endif
