# Quiz-o-Nator

Quiz-o-Nator es una aplicación de quiz en línea de comandos escrita en C++ que permite cargar preguntas y respuestas desde un archivo de texto. El programa es interactivo, muestra menús y resultados con colores (usando ANSI escape codes) y ofrece opciones para repasar preguntas falladas, copiar errores al portapapeles y reiniciar el test (manteniendo o variando el conjunto de preguntas).

## Características

- **Carga de archivo de preguntas**: Abre un diálogo para seleccionar el archivo de texto con las preguntas.
- **Formato de archivo flexible**: Cada bloque de pregunta debe seguir la estructura:

Enunciado de la pregunta A) Opción 1 B) Opción 2 *C) Opción correcta D) Opción 4

- **Interactividad y colores**: Menú interactivo con opciones coloreadas para mejorar la experiencia de usuario.
- **Depuración**: Muestra información de depuración (líneas originales y limpias) para detectar errores de formato.
- **Randomización**: Las preguntas y opciones se mezclan aleatoriamente en cada ejecución.
- **Opciones post-test**:
- Mostrar repaso de preguntas incorrectas.
- Copiar preguntas incorrectas al portapapeles.
- Reiniciar el mismo test.
- Reiniciar el test con otro conjunto aleatorio.
- Abrir el archivo de preguntas en el explorador/edición para repaso.

## Requisitos

- **Compilador**: C++17 o superior.
- **CMake**: Versión 3.10 o superior.
- **Plataforma**: Windows (con soporte para la API de Windows para diálogos de archivo y clipboard), Linux o macOS.
- **Terminal con soporte ANSI**: Se recomienda usar Windows Terminal o habilitar ANSI en CMD/PowerShell (ver sección de Configuración).

## Compilación

1. **Clonar el repositorio** (o descarga los archivos fuente):

    git clone <URL_del_repositorio>
    cd Quiz-o-Nator

Crear un directorio de compilación:

    mkdir build
    cd build

Generar los archivos de CMake:

    cmake ..

Compilar el proyecto:

    cmake --build . --config Release

    Esto generará un ejecutable (por ejemplo, quiz.exe en Windows).

## Uso

Ejecuta el programa:

    ./quiz.exe   # En Windows: quiz.exe

Selecciona el archivo de preguntas:
Se abrirá un diálogo de selección de archivo; elige el archivo TXT con tus preguntas (debe seguir el formato indicado).

Configura el quiz:
Se te pedirá ingresar el número de preguntas a responder.

Realiza el test:
El programa mostrará cada pregunta (con opciones mezcladas y con colores) y te pedirá responder.

Menú post-test:
Al finalizar, podrás:

- Revisar las preguntas incorrectas.

- Copiar al portapapeles las preguntas incorrectas.

- Reiniciar el test (manteniendo las mismas preguntas).

- Reiniciar el test con otro conjunto aleatorio.

- el archivo de preguntas en el explorador para repaso.

- Salir al menú principal.

Configuración de la Terminal en Windows

Para que CMD o PowerShell muestren los colores y caracteres Unicode (tildes y emojis), asegúrate de:

Activar UTF-8:

Ejecuta en CMD:

    chcp 65001

Usar una terminal con soporte ANSI:

    Se recomienda usar Windows Terminal o configurar CMD/PowerShell para que admitan ANSI (ver activar ANSI en CMD).

Verificar la fuente:

    Utiliza fuentes como Cascadia Code o Consolas que soporten emojis y caracteres especiales.

## **Formato del Archivo de Preguntas**

**Cada bloque de pregunta debe seguir este formato:**

**Enunciado de la pregunta**
```
A) Opción 1
B) Opción 2
*C) Opción correcta
D) Opción 4
---
```

    El asterisco * indica la respuesta correcta.
    Cada bloque debe terminar en una línea que contenga únicamente ---.


**Licencia**

    Este proyecto se distribuye bajo la Licencia MIT.