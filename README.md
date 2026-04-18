# Lux_Language

Intérprete de lenguaje de programación construido desde cero en C++.

## Compilar

```bash
g++ -std=c++17 -I src src/main.cpp src/lexer.cpp src/parser.cpp src/evaluator.cpp -o lux-language
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

## Arquitectura

### Módulo 1-2 — Lexer y Tokens
`src/token.h` — enum `TokenType`, struct `Token`, función `tokenTypeToString()`  
`src/lexer.h` / `src/lexer.cpp` — clase `Lexer`, método `tokenize()`

Convierte el código fuente en una lista plana de tokens.

### Módulo 3 — AST
`src/ast.h` — clase base `Node` y 12 nodos concretos

Define la estructura jerárquica que representa el código.
Los nodos usan `unique_ptr` para manejo automático de memoria.

| Nodo | Descripción |
|---|---|
| `Program` | Raíz del árbol — lista de sentencias |
| `LetStatement` | `let nombre = valor` |
| `ReturnStatement` | `return valor` |
| `ExpressionStatement` | Expresión usada como sentencia |
| `BlockStatement` | `{ sentencias... }` |
| `NumberLiteral` | Entero literal: `42` |
| `BooleanLiteral` | `true` o `false` |
| `Identifier` | Nombre de variable: `x` |
| `BinaryExpression` | `a + b`, `x == 5` |
| `PrefixExpression` | `-x`, `!true` |
| `IfExpression` | `if (cond) { } else { }` |
| `FunctionLiteral` | `fn(a, b) { ... }` |
| `CallExpression` | `suma(3, 4)` |

## Módulo 4 — Parser Parte 1

| Archivo | Contenido |
|---|---|
| `src/parser.h` | Declaración de la clase `Parser`, enum `Precedence` |
| `src/parser.cpp` | Implementación del Pratt Parser |

Convierte la lista de tokens en un AST navegable.
Implementa el algoritmo Pratt Parser (Top-Down Operator Precedence).

### Precedencias

| Nivel | Nombre | Operadores |
|---|---|---|
| 0 | LOWEST | inicio de expresión |
| 1 | EQUALS | `==` `!=` |
| 2 | LESSGREATER | `<` `>` `<=` `>=` |
| 3 | SUM | `+` `-` |
| 4 | PRODUCT | `*` `/` |
| 5 | PREFIX | `-x` `!x` |
| 6 | CALL | `f(x)` |

### Métodos principales

| Método | Rol Pratt |
|---|---|
| `parseExpression(minima)` | Algoritmo central |
| `parseIdentifier()` / `parseNumberLiteral()` / etc. | nud |
| `parseInfixExpression(left)` | led |

---

## Módulo 5 — Parser Parte 2
 
| Archivo | Cambios |
|---|---|
| `src/parser.h` | +`parseBlockStatement()`, +`parseIfExpression()`, +`parseFunctionLiteral()`, +`parseFunctionParameters()`, +`parseCallExpression()` |
| `src/parser.cpp` | Implementación de todos los métodos anteriores + `FN` e `IF` registrados como nud + `LPAREN` despacha a `parseCallExpression` |
| `src/main.cpp` | Pretty-printer extendido + pruebas de cada nuevo nodo + prueba integradora |
 
Completa la gramática del lenguaje Lux. El Parser ahora puede construir el AST completo de cualquier programa válido.
 
### Nuevos métodos del Parser
 
| Método | Tipo | Descripción |
|---|---|---|
| `parseBlockStatement()` | sentencia | Parsea `{ sentencias... }` — bucle hasta encontrar `}` |
| `parseIfExpression()` | nud | Parsea `if (cond) { } else { }` — `else` opcional |
| `parseFunctionLiteral()` | nud | Parsea `fn(params) { cuerpo }` |
| `parseFunctionParameters()` | helper | Lee lista de nombres separados por `,` entre `()` |
| `parseCallExpression(left)` | led | Parsea `expresión(args)` — el `(` como operador infijo |
 
### Decisiones de diseño del Parser
 
| Decisión | Razón |
|---|---|
| `if` es expresión (nud), no sentencia | Permite `let x = if (cond) { a } else { b }` |
| `fn` es expresión (nud), no declaración | Permite funciones anónimas y funciones como valores |
| `parseBlockStatement` termina con `currentToken = }` | El llamador necesita inspeccionar qué viene después del bloque |
| `parseFunctionParameters` devuelve `vector<string>` | Los parámetros son solo nombres — no hace falta un nodo completo |
| `parseCallExpression` es led via `LPAREN` | Permite `f(x)`, `fn(x){x}(5)` y `f()(x)` sin casos especiales |
 
### Programas Lux completos que el Parser maneja correctamente
 
```
let absoluto = fn(x) {
    if (x < 0) { return -x; } else { return x; }
};
 
let maximo = fn(a, b) {
    if (a > b) { return a; } else { return b; }
};
let resultado = maximo(10, 3);
 
let fibonacci = fn(n) {
    if (n < 2) { return n; }
    return fibonacci(n - 1) + fibonacci(n - 2);
};
```

---

## Módulo 6 — Entorno (Scope)

| Archivo | Contenido |
|---|---|
| `src/object.h` | Clase base `Object` — versión mínima, se expandirá en Módulo 7 |
| `src/environment.h` | Clase `Environment` completa con cadena de scopes |

Implementa el sistema de variables del intérprete: dónde se guardan y cómo se buscan.

### Diseño de `Environment`

```
Environment
├── store_  : std::map<string, shared_ptr<Object>>   variables locales
└── outer_  : shared_ptr<Environment>                scope padre (nullptr si global)
```

### Métodos

| Método | Descripción |
|---|---|
| `get(name)` | Busca en el mapa local; si no está, sube al padre recursivamente. Devuelve `nullptr` si no existe en ningún scope. |
| `set(name, value)` | Guarda en el mapa local. Nunca escribe en el padre. |
| `createEnclosed(outer)` | Función estática. Crea un scope hijo con `outer` como padre. |

### Comportamiento de la cadena de scopes

```
Scope S2 (hijo de S1)   →   Scope S1 (hijo de S0)   →   Scope S0 (global)
    b = 3                       a = 5                       x = 10

S2->get("b") → encontrado en S2 → devuelve 3
S2->get("a") → no en S2 → sube → encontrado en S1 → devuelve 5
S2->get("x") → no en S2 → sube → no en S1 → sube → encontrado en S0 → devuelve 10
S2->get("z") → no en S2 → sube → no en S1 → sube → no en S0 → nullptr (no existe)
```

### Reglas de diseño

| Regla | Comportamiento |
|---|---|
| `set` escribe solo en el scope actual | Una variable local nunca toca al padre |
| Shadowing | Una variable local con el mismo nombre que una del padre la oculta sin modificarla |
| Scopes hermanos | Dos hijos del mismo padre no se ven entre sí — la cadena solo va hacia arriba |
| Scope léxico | Una función ve el scope donde fue **definida**, no donde fue **llamada** |

### Por qué `shared_ptr` y no `unique_ptr`

Los scopes pueden ser compartidos por los closures. Cuando una función captura
su entorno de definición, ese scope tiene dos dueños: el intérprete que lo creó
y la función que lo capturó. `shared_ptr` mantiene el scope vivo mientras al
menos uno de los dos lo referencie, y lo destruye automáticamente cuando ambos
desaparecen.

### Concepto C++ nuevo: `shared_ptr`

`shared_ptr<T>` es el puntero inteligente para ownership compartido.
Mantiene un contador de referencias interno. El objeto se destruye
automáticamente cuando el contador llega a cero (ningún `shared_ptr`
lo referencia). A diferencia de `unique_ptr`, puede copiarse libremente.

### Estructura de archivos actualizada

```
lux/
├── src/
│   ├── token.h
│   ├── lexer.h
│   ├── lexer.cpp
│   ├── ast.h
│   ├── parser.h
│   ├── parser.cpp
│   ├── object.h          ← nuevo en Módulo 6
│   ├── environment.h     ← nuevo en Módulo 6
│   └── main.cpp
├── .gitignore
└── README.md
```

---

## Módulo 7 — Evaluador Parte 1

| Archivo | Contenido |
|---|---|
| `src/object.h` | Sistema de tipos completo: `ObjectType`, `Object`, `Integer`, `Boolean`, `Null`, instancias globales, `nativeBoolToObject()` |
| `src/evaluator.h` | Declaración de la clase `Evaluator` |
| `src/evaluator.cpp` | Implementación del evaluador: literales, prefijos, expresiones binarias |
| `src/main.cpp` | Pipeline completo + pruebas del sistema de tipos y del evaluador |

El Evaluador recorre el AST nodo por nodo y produce valores del sistema de tipos.
Implementa el patrón **tree-walking interpreter**: evaluación recursiva descendente.

### Sistema de tipos de Lux

```
Object                    ← clase base abstracta (object.h)
├── Integer               ← guarda int value
├── Boolean               ← guarda bool value
├── Null                  ← ausencia de valor, sin campos
├── ReturnValue           ← wrapper de return (Módulo 8)
├── Function              ← función como valor (Módulo 9)
└── Error                 ← error en tiempo de ejecución (Módulo 8)
```

Cada subclase implementa dos métodos virtuales obligatorios:

| Método | Descripción |
|---|---|
| `type()` | Devuelve `ObjectType` — etiqueta rápida del tipo, sin cast |
| `inspect()` | Devuelve representación en texto — lo que ve el usuario |

### Instancias globales compartidas

`true`, `false` y `null` son valores únicos. En lugar de crear un objeto nuevo
cada vez, el evaluador reutiliza instancias globales:

```cpp
inline std::shared_ptr<Boolean> LUX_TRUE  = std::make_shared<Boolean>(true);
inline std::shared_ptr<Boolean> LUX_FALSE = std::make_shared<Boolean>(false);
inline std::shared_ptr<Null>    LUX_NULL  = std::make_shared<Null>();
```

`inline` en variables (C++17): una sola instancia aunque múltiples `.cpp`
incluyan `object.h`. Sin `inline`, el linker reportaría símbolo duplicado.

### Diseño del Evaluator

```cpp
// Único método público — puerta de entrada
std::shared_ptr<Object> eval(Node* node, std::shared_ptr<Environment> env);
```

`Node*` crudo porque el evaluador solo observa el AST, no lo posee.
`shared_ptr<Environment>` porque el entorno es compartido y debe sobrevivir
mientras se evalúa.

#### Dispatch por tipo
 
`eval()` hace `dynamic_cast` a cada tipo posible de nodo y delega al
método especializado correspondiente:

```
eval(node):
    Program            → evalProgram()
    ExpressionStatement→ eval(inner)        ← transparente
    NumberLiteral      → evalNumberLiteral()
    BooleanLiteral     → evalBooleanLiteral()
    PrefixExpression   → evalPrefixExpression()
    BinaryExpression   → evalBinaryExpression()
    (más casos en M8)
```

#### Métodos privados del Evaluator

| Método | Descripción |
|---|---|
| `evalProgram` | Ejecuta todas las sentencias en orden, devuelve el valor de la última |
| `evalNumberLiteral` | `NumberLiteral{42}` → `Integer{42}` |
| `evalBooleanLiteral` | `BooleanLiteral{true}` → `LUX_TRUE` |
| `evalPrefixExpression` | Evalúa operando, aplica `-` o `!` |
| `evalBinaryExpression` | Evalúa ambos lados, delega a helper según tipos |
| `evalIntegerBinaryExpression` | `+`, `-`, `*`, `/`, `==`, `!=`, `<`, `>`, `<=`, `>=` entre enteros |
| `evalBooleanBinaryExpression` | `==`, `!=` entre booleanos — compara punteros (instancias únicas) |

### Por qué `Node*` y no `shared_ptr<Node>`

El AST usa `unique_ptr` para ownership. El evaluador no posee los nodos,
solo los lee. En C++ moderno, un observador sin ownership usa puntero crudo `T*`.

### Conceptos C++ nuevos en este módulo

| Concepto | Descripción |
|---|---|
| `virtual` | Activa dispatch dinámico — usa el tipo real del objeto, no el del puntero |
| `override` | Verifica en compilación que realmente sobreescribe un método virtual de la base |
| Método virtual puro `= 0` | Obliga a las subclases a implementarlo; hace la clase abstracta |
| Clase abstracta | No se puede instanciar — existe solo para ser heredada |
| vtable / vptr | Mecanismo interno de C++ para resolver llamadas virtuales en runtime |
| Destructor virtual | Obligatorio cuando hay métodos virtuales — garantiza destrucción correcta |
| `dynamic_cast<T*>` | Downcast seguro con verificación en runtime — devuelve `nullptr` si falla |
| `.get()` en `shared_ptr` | Extrae el puntero crudo sin transferir ownership |
| `inline` variable (C++17) | Una sola instancia global aunque múltiples `.cpp` incluyan el header |
| Lambda `[&](params){ }` | Función anónima con captura por referencia |
| `auto` para lambdas | `auto` deduce el tipo único de cada lambda |

### Pipeline completo funcionando

```
"1 + 2 * 3;"
    ↓ Lexer      → [NUMBER:1, PLUS, NUMBER:2, ASTERISK, NUMBER:3, SEMICOLON, EOF]
    ↓ Parser     → BinaryExpression(+, NumberLiteral(1), BinaryExpression(*, 2, 3))
    ↓ Evaluator  → eval(Binary+) → eval(1)=Integer{1}, eval(Binary*)→eval(2)*eval(3)=Integer{6}
    ↓ resultado  → Integer{7}  →  inspect() → "7"
```

### Resultados verificados

```
42          → INTEGER  : 42
1 + 2 * 3   → INTEGER  : 7      (precedencia correcta)
(1+2) * 3   → INTEGER  : 9      (paréntesis respetados)
!true       → BOOLEAN  : false
!!false     → BOOLEAN  : false  (doble negación recursiva)
-(3 + 4)    → INTEGER  : -7
3 < 5       → BOOLEAN  : true
5 == 5      → BOOLEAN  : true
1 + 2 == 3  → BOOLEAN  : true   (aritmética + comparación)
true == true→ BOOLEAN  : true   (comparación de punteros)
```

---

## Estructura de archivos

```
lux/
├── src/
│   ├── token.h           Módulo 1-2 — TokenType, Token
│   ├── lexer.h           Módulo 1-2 — declaración Lexer
│   ├── lexer.cpp         Módulo 1-2 — implementación Lexer
│   ├── ast.h             Módulo 3   — Node + 12 nodos concretos
│   ├── parser.h          Módulo 4-5 — declaración Parser
│   ├── parser.cpp        Módulo 4-5 — Pratt Parser completo
│   ├── object.h          Módulo 7   — sistema de tipos: Integer, Boolean, Null
│   ├── environment.h     Módulo 6   — Environment con cadena de scopes
│   ├── evaluator.h       Módulo 7   — declaración Evaluator
│   ├── evaluator.cpp     Módulo 7   — implementación Evaluator (expresiones)
│   └── main.cpp          pruebas acumuladas de todos los módulos
├── .gitignore
└── README.md
```

---

## Próximo módulo: 8 — Evaluador Parte 2

Evaluar variables (`Identifier`), declaraciones (`LetStatement`),
condicionales (`IfStatement`), bloques (`BlockStatement`) y retornos (`ReturnStatement`).
Al terminar, Lux podrá ejecutar programas reales con variables y lógica condicional.