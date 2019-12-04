#ifndef TYPES_H
#define TYPES_H

#include <cinttypes>

union cmd   // -- команда в одном слове: либо одна 32-битная, либо две 16-битных -- //
{
    struct cmd16_struct {   // -- структура 16-битной команды -- //
        uint8_t t : 1;      // -- бит, определяющий длину команды, 0 - для 16-битной -- //
        uint8_t cop : 7;    // -- код операции -- //
        uint8_t r1 : 4;     // -- номер первого операнда-регистра -- //
        uint8_t r2 : 4;     // -- номер второго регистра -- //
    };
    struct cmd32_struct {
        uint8_t t : 1;          // -- бит, определяющий длину команды, 1 - для 32-битной -- //
        uint8_t cop : 7;
        uint8_t r1 : 4;
        uint8_t r2 : 4;
        uint16_t address : 16;  // -- адрес, константа, смещение -- //
    };
    cmd16_struct c16[2];
    cmd32_struct c32;
};

union data_t {  // -- тип данных (размер 32-бита), использумый при загрузки в память -- //
    int32_t i;  // -- знаковое число -- //
    uint32_t u; // -- беззнаковое число -- //
    float f;    // -- дробное число -- //
    cmd c;      // -- команда (или две 16-битных команды) в одном слове -- //
};

#endif // TYPES_H
