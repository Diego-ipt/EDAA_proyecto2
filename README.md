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