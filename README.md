# Proyecto-Compi

# Gramatica:
Program ::= VarDecList  FunDecList  Body

VarDecList ::= (VarDec)*

FunDecList ::= (FunDec)*

FunDec ::= 'proc' id '(' [ParamDecList] ')' [':' Type] '=' NEWLINE INDENT Body DEDENT 

Body ::= VarDecList StmtList

ParamDecList ::= Param (',' Param)*

Param ::= id ':' Type	

VarDec ::= 'v	ar' VarList ':' Type

VarList ::= id (',' id)*

StmtList ::= Stmt (NEWLINE Stmt )*

Stmt ::= id '=' CExp

  | 'echo' '(' CExp ')'

  | 'if' CExp ':' NEWLINE INDENT Body ['else' ':' NEWLINE INDENT Body DEDENT] DEDENT

  | 'while' CExp ':' NEWLINE INDENT Body DEDENT

  | 'return' [CExp]

CExp ::= IfExp (('<' | '<=' | '==' | '>=' | '>') IfExp)?

IfExp ::= 'if' CExp ':' Exp 'else' ':' Exp | Exp

Exp ::= Term (('+' | '-') Term)*

Term ::= Factor (('*' | '/') Factor)*

Factor ::= CastExpr | id | Num | Bool | '(' Exp ')' | id '(' [ArgList] ')' 

CastExpr ::= Type '(' CExp ')'

ArgList ::= CExp (',' CExp)*

Bool ::= 'true' | 'false'

Type ::= id

# 📋 Casos de Prueba - Compilador Nim

Este documento contiene una suite completa de pruebas para validar la implementación de un compilador Nim, organizada en tres categorías principales.

---

## 🔧 Tipos Numéricos Soportados

| Tipo | Descripción | Bits |
|------|-------------|------|
| `int` | Entero con signo | 64 |
| `int32` | Entero con signo | 32 |
| `uint` | Entero sin signo | 64 |
| `uint32` | Entero sin signo | 32 |
| `float` | Punto flotante (double) | 64 |
| `float32` | Punto flotante | 32 |

### Conversiones Explícitas
- **Sintaxis:** `valor.tipo`
- **Ejemplos:** `x.float`, `y.int`, `z.uint32`

### Expresiones Condicionales (Ternarias)
- **Sintaxis:** `if condicion: valor_true else: valor_false`
- Puede usarse como expresión que retorna un valor
- Soporta anidamiento

---

## 🎯 Categoría 1: Funciones (3 casos)

### Test 1: Función con tipos mixtos (int, uint, float)

```nim
proc calcularPromedio(a: int, b: uint, c: float): float =
    var suma: float
    suma = a.float + b.float + c
    return suma / 3.float

var resultado: float
resultado = calcularPromedio(-10, 20, 15.5)
echo(resultado)
```

**Salida esperada:** `8.500000` o similar

---

### Test 2: Función recursiva con diferentes tipos numéricos

```nim
proc factorialUint(n: uint): uint =
    if n <= 1.uint:
        return 1.uint
    else:
        return n * factorialUint(n - 1.uint)

proc potencia(base: float, exp: int): float =
    if exp == 0:
        return 1.float
    else:
        return base * potencia(base, exp - 1)

var fact: uint
var pot: float

fact = factorialUint(5.uint)
echo(fact)

pot = potencia(2.5, 3)
echo(pot)
```

**Salida esperada:**
```
120
15.625000
```

---

### Test 3: Función con conversiones implícitas en parámetros

```nim
proc operacionCompleja(x: int32, y: float32, z: uint32): float =
    var temp1: float
    var temp2: float
    
    temp1 = x.float + y.float
    temp2 = z.float * 2.float
    
    return temp1 + temp2

var a: int32
var b: float32
var c: uint32
var res: float

a = 10
b = 3.5.float32
c = 25.uint32

res = operacionCompleja(a, b, c)
echo(res)
```

**Salida esperada:** `63.500000`

---

## ⚙️ Categoría 2: Implementación Base (5 casos)

### Test 4: Operaciones con tipos float (32 y 64 bits)

```nim
var x: float
var y: float32
var z: float

x = 10.5
y = 3.25.float32
z = x + y.float

echo(x)
echo(y)
echo(z)

z = x * 2.float
echo(z)

z = x / 2.5
echo(z)
```

**Salida esperada:**
```
10.500000
3.250000
13.750000
21.000000
4.200000
```

---

### Test 5: Operaciones con unsigned int (32 y 64 bits)

```nim
var a: uint
var b: uint32
var c: uint

a = 100.uint
b = 50.uint32
c = a + b.uint

echo(a)
echo(b)
echo(c)

c = a * 2.uint
echo(c)

a = 1000.uint
b = 300.uint32
c = a - b.uint
echo(c)
```

**Salida esperada:**
```
100
50
150
200
700
```

---

### Test 6: Comparaciones con tipos numéricos mixtos

```nim
var x: int
var y: uint
var z: float
var resultado: bool

x = -10
y = 20.uint
z = 15.5

resultado = x < y.int
echo(resultado)

resultado = y.float > z
echo(resultado)

resultado = x.float == -10.float
echo(resultado)

resultado = z >= 15.5
echo(resultado)
```

**Salida esperada:**
```
1
1
1
1
```

---

### Test 7: Tipos de 32 bits (int32, uint32, float32)

```nim
var a: int32
var b: uint32
var c: float32
var d: int32

a = 1000
b = 2000.uint32
c = 3.14159.float32

d = a + b.int32
echo(d)

a = b.int32 - 500
echo(a)

c = c * 2.float32
echo(c)
```

**Salida esperada:**
```
3000
1500
6.283180
```

---

### Test 8: Bucles y condicionales con diferentes tipos

```nim
var i: uint
var suma: int
var promedio: float

suma = 0
i = 1.uint

while i <= 10.uint:
    suma = suma + i.int
    i = i + 1.uint

echo(suma)

promedio = suma.float / 10.float
echo(promedio)

if promedio > 5.float:
    echo(1)
else:
    echo(0)
```

**Salida esperada:**
```
55
5.500000
1
```

---

## 🚀 Categoría 3: Extensiones (6 casos)

### Test 9: Conversión explícita entre todos los tipos numéricos

```nim
var a: int
var b: uint
var c: float
var d: int32
var e: uint32
var f: float32

a = 100
b = a.uint
c = b.float
d = c.int32
e = d.uint32
f = e.float32

echo(a)
echo(b)
echo(c)
echo(d)
echo(e)
echo(f)
```

**Salida esperada:**
```
100
100
100.000000
100
100
100.000000
```

---

### Test 10: Expresión condicional ternaria básica

```nim
var x: int
var y: int
var max: int

x = 10
y = 20

max = if x > y: x else: y
echo(max)

max = if x < y: x else: y
echo(max)
```

**Salida esperada:**
```
20
10
```

---

### Test 11: Expresiones ternarias con tipos numéricos mixtos

```nim
var a: int
var b: float
var resultado: float

a = 5
b = 10.5

resultado = if a.float > b: a.float * 2.float else: b / 2.float
echo(resultado)

resultado = if a < 10: b + a.float else: b - a.float
echo(resultado)
```

**Salida esperada:**
```
5.250000
15.500000
```

---

### Test 12: Promoción de tipos en operaciones aritméticas

```nim
var x: int32
var y: int
var z: int

x = 100
y = 200

z = x.int + y
echo(z)

var a: float32
var b: float
var c: float

a = 3.5.float32
b = 7.25

c = a.float + b
echo(c)
```

**Salida esperada:**
```
300
10.750000
```

---

### Test 13: Expresiones ternarias anidadas con conversiones

```nim
var x: int
var y: int
var z: int
var resultado: float

x = 10
y = 20
z = 15

resultado = if x > y: 
    x.float 
else: 
    if y > z: y.float else: z.float

echo(resultado)

resultado = if x.float < 15.float:
    if y > 25: y.float * 2.float else: y.float
else:
    z.float

echo(resultado)
```

**Salida esperada:**
```
20.000000
20.000000
```

---

### Test 14: Conversiones en contexto de funciones con ternarias

```nim
proc calcular(a: int, b: int): float =
    var temp: float
    temp = if a > b: a.float else: b.float
    return temp * 1.5

proc seleccionar(x: uint, y: uint): uint =
    return if x > y: x else: y

var res1: float
var res2: uint

res1 = calcular(10, 25)
echo(res1)

res2 = seleccionar(100.uint, 75.uint)
echo(res2)

var final: float
final = if res2.float > 50.float: res1 + res2.float else: res1
echo(final)
```

**Salida esperada:**
```
37.500000
100
137.500000
```

---

## ⚠️ Verificaciones Importantes

| Aspecto | Consideración |
|---------|---------------|
| **Precisión de floats** | Los valores pueden variar ligeramente según la implementación |
| **Overflow** | Cuidado con valores que excedan los límites de tipos de 32 bits |
| **Conversiones unsigned** | Asegurar que los valores negativos se manejen correctamente |
| **Promoción implícita** | Verificar que las operaciones mixtas se resuelvan correctamente |

---

## 📊 Resumen de Cobertura

- ✅ **3 casos** de funciones con tipos mixtos y recursión
- ✅ **5 casos** de implementación base con operaciones fundamentales
- ✅ **6 casos** de extensiones con conversiones y expresiones ternarias
- 📈 **Total: 14 casos de prueba**

---

> **Nota:** Este conjunto de pruebas cubre los aspectos fundamentales y avanzados del compilador Nim, incluyendo manejo de tipos, conversiones, funciones y expresiones condicionales.
