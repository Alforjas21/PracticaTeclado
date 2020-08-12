#include "TECLADO.h"

Teclado4x4::Teclado4x4(uint8_t F1, uint8_t F2, uint8_t F3, uint8_t F4,
uint8_t C1, uint8_t C2, uint8_t C3, uint8_t C4, char const * caracteres) {
// Constructor al que le pasamos:
// - F1, F2, F3, F4: salidas digitales de la placa Arduino conectadas a las señales
// de las filas en el teclado
// - C1, C2, C3, C4: entradas digitales de la placa Arduino conectadas a las
// señales de las columnas en el teclado
// - caracteres: La pulsación de una tecla genera un carácter. Aquí se
// indican los caracteres
// generados por la pulsación de cada una de las 16 teclas, yendo por
// filas: fila 1 columna 1, fila 1 columna 2, ..., fila 1 columna 4, fila
// 2 columna 1, ..., fila 4 columna 4
_salidasFilas[0] = F1;
_salidasFilas[1] = F2;
_salidasFilas[2] = F3;
_salidasFilas[3] = F4;
_entradasColumnas[0] = C1;
_entradasColumnas[1] = C2;
_entradasColumnas[2] = C3;
_entradasColumnas[3] = C4;
strcpy(_caracteres, caracteres);
}
void Teclado4x4::configura() {
// Método a utilizar en setup() para inicializar las señales de manejo del
// teclado
int i;
for (i = 0; i < 4; i++) {
pinMode(_entradasColumnas[i], INPUT_PULLUP);
pinMode(_salidasFilas[i], INPUT);
}
_pulsado = 0;
_t = millis();
}
char Teclado4x4::comprueba() {
// Método a utilizar en loop() para comprobar si se ha detectado la
// pulsación de una tecla. Devuelve:
// - Un buleano falso si no se detectó una nueva pulsación
// - El carácter correspondiente a una tecla si se detectó su pulsación
uint8_t iSalida, iEntrada; // Para barrer entradas y salidas
if (millis() - _t < 5)
return 0;
else
_t = millis();
// Para que haya como mínimo un intervalo de 5 ms entre ejecuciones
// de las siguientes instrucciones para evitar rebotes
if (_pulsado) { // Si había una tecla pulsada ...
pinMode(_salidasFilas[_fila], OUTPUT);
digitalWrite(_salidasFilas[_fila], LOW);
// Activa la salida de la fila actual y la pone a nivel bajo
_pulsado = digitalRead(_entradasColumnas[_columna]) == LOW;
// La tecla sigue pulsada si la entrada en la columna actual está a 0
pinMode(_salidasFilas[_fila], INPUT);
// Desactiva la salida de la fila
if (!_pulsado) // Si se dejó de pulsar ...
_t = millis(); // otros 10 ms para evitar rebotes
return 0; // Indica que no es una nueva pulsación
} else { // Si no se estaba pulsando una tecla ...
for (iSalida = 0; iSalida < 4 && !_pulsado; iSalida++) {
// Recorre todas las salidas o hasta que se detecta una nueva
// pulsación
pinMode(_salidasFilas[iSalida], OUTPUT);
digitalWrite(_salidasFilas[iSalida], LOW);
// Activa la salida y la pone a nivel bajo
for (iEntrada = 0; iEntrada < 4 && !_pulsado; iEntrada++) {
// Recorre todas las entradas o hasta que se detecte una nueva
// pulsación
if (digitalRead(_entradasColumnas[iEntrada]) == LOW) {
// Si la entrada está a nivel bajo, hay una nueva pulsación
_pulsado = 1; // Indica que hay una nueva pulsación
_fila = iSalida;
_columna = iEntrada;
// Guarda la fila y columna de la tecla pulsada
}
}
pinMode(_salidasFilas[iSalida], INPUT);
// Desactiva la salida y la convierte en una entrada
}
if (_pulsado)
return _caracteres[_fila * 4 + _columna];
else
return 0;
// Si se detectó la pulsación, devuelve el caracter correspondiente a la
// tecla pulsada
}
}
