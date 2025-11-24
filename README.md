# Proyecto 2 - Detección de Patrones en Múltiples Documentos

Este proyecto implementa una solución basada en **FM-index** para buscar patrones en una colección de múltiples documentos, devolviendo los nombres de los archivos que contienen el patrón.

## Requisitos

*   Compilador C++ (g++)
*   Biblioteca SDSL (Succinct Data Structure Library)
*   Python 3 (para preparar el corpus)

## Instrucciones de Ejecución

### 1. Preparación del Corpus

El sistema funciona concatenando todos los documentos en un único archivo grande (`corpus.txt`). Para evitar conflictos con el contenido de los archivos (que pueden contener el símbolo `$`), utilizamos el carácter ASCII 2 (`STX` - Start of Text) como separador.

Ejecuta el script de Python para generar el corpus y la lista de nombres de archivos:

```bash
python prepare_corpus.py
```

Esto generará dos archivos:
*   `corpus.txt`: Todos los documentos concatenados, separados por el byte `\x02`.
*   `filenames.txt`: Lista ordenada de los nombres de los archivos originales.

### 2. Compilación

Compila el código C++ (`Test_FM_index.cpp`) asegurándote de enlazar las bibliotecas de SDSL:

```bash
g++ -O3 -o test_fmi Test_FM_index.cpp -lsdsl -ldivsufsort -ldivsufsort64
```

### 3. Ejecución del Test

Ejecuta el programa pasando el corpus y el archivo de nombres como argumentos:

```bash
./test_fmi corpus.txt filenames.txt
```

El programa construirá el índice y te pedirá que ingreses un patrón.

**Ejemplo:**
```
Ingrese un patrón a buscar: data
# de ocurrencias: 40382
Las ocurrencias se encuentran en los siguientes documentos:
dblp5MB_1.xml
sources5MB_7.xml
...
```

## Detalles de Implementación

### Separador de Documentos
Originalmente se planeó usar `$` como separador. Sin embargo, durante las pruebas se detectó que este carácter aparecía frecuentemente dentro de los archivos XML y de código fuente, lo que causaba que el sistema detectara miles de "documentos" falsos.

**Solución:** Se cambió el separador por el carácter de control **ASCII 2 (`\x02`)**. Este carácter es no imprimible y extremadamente raro en archivos de texto estándar, garantizando una delimitación correcta de los documentos.

### Estructuras de Datos (SDSL)
Siguiendo las recomendaciones de optimización:
*   **FM-Index:** Se utiliza `csa_wt<wt_huff<rrr_vector<127>>>`. Esta combinación (Wavelet Tree de Huffman + RRR bit vector) ofrece una compresión superior para texto natural.
*   **Mapeo de Documentos:** Se utiliza `sd_vector` (Sparse-Dense vector) para almacenar los límites de los documentos. Esto permite determinar a qué documento pertenece una posición en tiempo $O(1)$ con un consumo de memoria mínimo.
## Experimentación
Cada experimento fue repetido 32 veces haciendo uso de código obtenido desde el repositorio uhr por leonardlover (https://github.com/leonardlover/uhr/).
El archivo get_patterns.py busca los 20 patrones más frecuentes y menos frecuentes (aunque con al menos una aparición), usando una limitación de 6 caracteres por patrón, guardándolos en patterns_common.txt y patterns_rare.txt, respectivamente.

Se incluye el archivo DocumentFMIndex.cpp el cual es una versión editada para experimentación de Test_FM_index.cpp, de manera que las funciones puedan ser llamadas directamente desde el archivo que se encarga de realizar los experimentos.
Para compilar uhr.cpp, se debe realizar:
```bash
g++ -std=c++17 -O2 uhr.cpp ../DocumentFMIndex.cpp -o uhr -lsdsl -ldivsufsort -ldivsufsort64
```
Para ejecutarlo:
```bash
./uhr resultados.csv 32 1 1 1
```
El patrón de búsqueda utilizado en cada experimento se especifica directamente en el código fuente, específicamente en uhr.cpp, en la llamada:
```c++
index.doc_locate("<patrón>");
```
Para la hipótesis 1, se hace uso del patrón de alta frecuencia (según lo previamente obtenido con get_patterns.py) "author" y el patrón de baja frecuencia "<?xml ". 
Para la hipótesis 2, se hace uso del patrón inexistente en el dataset: "¿?--¿?". 
Para la hipótesis 3, puesto que la memoria ocupada por cada estructura es determinista para un texto dado, la construcción repetida del índice no produce variaciones en el uso de memoria. Por esta razón, el valor reportado corresponde a una única medición representativa por archivo.
