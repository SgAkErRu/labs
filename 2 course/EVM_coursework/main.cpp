#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "cpu.h"

using namespace std;

bool Uploader(CPU &cpu, const string &filename)
{
    ifstream file(filename);

    if (!file.is_open()) return false;

    uint16_t address = 0;   // -- адрес памяти, по которому загружаются данные или команды -- //
    char symbol = ' ';  // -- текущий читаемый символ -- //
    while (symbol != 'e')   // -- пока не встретили символ 'e' -- //
    {
        string s;
        getline(file, s);
        istringstream ss(s);
        ss >> symbol;
        switch (symbol)
        {
            case 'a':   // -- установить адрес -- //
            {
                ss >> address;
                break;
            }
            case 'i': case 'u': // -- загрузка данных: целое, беззнаковое -- //
            {
                word chislo;
                ss >> chislo.data.u;
                cpu.ram.push(chislo, address);
                ++address;
                break;
            }
            case 'f': // -- загрузка данных: число с плавающей точкой -- //
            {
                word chislo;
                ss >> chislo.data.f; // -- читаем как число с плавающей точкой -- //
                cpu.ram.push(chislo, address);
                ++address;
                break;
            }
            case 'c':   // -- загрузка команды -- //
            {
                word command; // -- пример: c t cop r1 r2 (для 16-битной команды) -- //
                uint16_t temp;
                ss >> temp;     // -- читаем флаг длины команды t -- //
                if (temp == 0)  // -- если флаг t (type) равен 0, значит команда короткая (16 бит) -- //
                {
                    command.c.c16[0].t = 0;
                    ss >> temp; // -- читаем cop (код операции) -- //
                    command.c.c16[0].cop = uint8_t(temp);
                    ss >> temp; // -- читаем r1 -- //
                    command.c.c16[0].r1 = uint8_t(temp);
                    ss >> temp; // -- читаем r2 -- //
                    command.c.c16[0].r2 = uint8_t(temp);
                    getline(file,s);    // -- если была указана короткая команда, //
                    ss.clear();         // значит следующая строчка в файле тоже должна быть короткой командой -- //
                    ss.str(s);          // -- поэтому читаем следующую строку -- //
                    ss >> symbol;   // -- читаем символ -- //
                    ss >> temp;     // -- читаем тип команды -- //
                    command.c.c16[1].t = 0;
                    ss >> temp; // -- читаем cop (код операции) -- //
                    command.c.c16[1].cop = uint8_t(temp);
                    ss >> temp; // -- читаем r1 -- //
                    command.c.c16[1].r1 = uint8_t(temp);
                    ss >> temp; // -- читаем r2 -- //
                    command.c.c16[1].r2 = uint8_t(temp);
                }
                else // -- иначе длинная -- //
                {
                    command.c.c32.t = 1;
                    ss >> temp; // -- читаем cop (код операции) -- //
                    command.c.c32.cop = uint8_t(temp);
                    ss >> temp; // -- читаем r1 -- //
                    command.c.c32.r1 = uint8_t(temp);
                    ss >> temp; // -- читаем r2 -- //
                    command.c.c32.r2 = uint8_t(temp);
                    ss >> temp; // -- читаем address -- //
                    command.c.c32.address = temp;
                }
                cpu.ram.push(command, address);
                ++address;
                break;
            }
            case 'e':   // -- начинаем работу процессора -- //
            {
                word command;
                command.c.c32.t = 1;
                command.c.c32.cop = 0;
                cpu.ram.push(command,address);
                uint16_t IP;
                ss >> IP;
                cpu.PSW.IP = IP;
                break;
            }
        }
    }
    file.close();
    return true;
}

int main (int argc, char* argv[])
{
    CPU cpu;
    if (argc > 1)
    {
        cout << "File " << argv[1] << " is ";
        if (Uploader(cpu, argv[1]))
        {
            cout << "uploaded!" << endl;
            cpu.run();
        }
        else cout << "not uploaded" << endl;
    } else cout << "Program has been started without arguments." << endl;
    system("pause");

    return 0;
}

