# Lux

Intérprete de lenguaje de programación construido desde cero en C++.

## Compilar

```bash
g++ -std=c++17 -I src src/main.cpp src/lexer.cpp -o lux-language
```

> A medida que agreguemos archivos `.cpp` al proyecto, hay que añadirlos al comando.

## Ejecutar

```bash
./lux-language
```

En Windows (PowerShell):
```powershell
.\lux-language.exe
```