// Test_FM_index.cpp
// Implementación de búsqueda de documentos usando FM-index
//
// Prerrequisitos: Tener la biblioteca SDSL instalada
//
// Compilación: g++ -O3 -o test_fmi Test_FM_index.cpp -lsdsl -ldivsufsort -ldivsufsort64

#include <sdsl/suffix_arrays.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace sdsl;
using namespace std;

#include <fstream>

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "Uso: " << argv[0] << " <archivo entrada> [archivo nombres]" << endl;
    return 1;
  }

  // Cargar nombres de archivos si se proporciona el argumento
  vector<string> filenames;
  if (argc >= 3) {
      ifstream namefile(argv[2]);
      if (namefile.is_open()) {
          string line;
          while (getline(namefile, line)) {
              filenames.push_back(line);
          }
          namefile.close();
      } else {
          cerr << "Advertencia: No se pudo abrir el archivo de nombres." << endl;
      }
  }

  cout << "Construyendo el FM-index compactado (Huffman + RRR)..." << endl;
  // Usamos wt_huff con rrr_vector para máxima compresión (Hipótesis H1 del reporte)
  csa_wt<wt_huff<rrr_vector<127>>> fm_index;
  construct(fm_index, argv[1], 1);
  
  cout << "Tamaño del FM-index " << size_in_mega_bytes(fm_index) << " MB." << endl;

  // Construir mapa de documentos usando sd_vector (Sección 5.1 del reporte)
  cout << "Construyendo mapa de documentos (sd_vector)..." << endl;
  string sep_str = "\x02";
  
  // Primero recolectamos las posiciones de los separadores
  vector<size_t> boundary_positions;
  boundary_positions.push_back(0); // El primer documento empieza en 0
  
  size_t occs_sep = sdsl::count(fm_index, sep_str.begin(), sep_str.end());
  if (occs_sep > 0) {
      auto sep_locs = sdsl::locate(fm_index, sep_str.begin(), sep_str.end());
      sort(sep_locs.begin(), sep_locs.end());
      for (auto loc : sep_locs) {
          boundary_positions.push_back(loc + 1); // Inicio del siguiente doc
      }
  }
  
  bit_vector b(fm_index.size(), 0);
  for (size_t pos : boundary_positions) {
      if (pos < b.size()) {
          b[pos] = 1;
      }
  }
  
  sd_vector<> doc_borders(b);
  sd_vector<>::rank_1_type doc_rank(&doc_borders);
  
  cout << "Documentos detectados: " << (boundary_positions.size()) << endl; 

  string patron;
  cout << "Ingrese un patrón a buscar: ";
  cin >> patron;

  // Buscando las ocurrencias del patrón
  size_t occs = sdsl::count(fm_index, patron.begin(), patron.end());

  cout << "# de ocurrencias: " << occs << endl;
  if (occs > 0) {
    cout << "Las ocurrencias se encuentran en los siguientes documentos: " << endl;
    auto posiciones = sdsl::locate(fm_index, patron.begin(), patron.end());
    sort(posiciones.begin(), posiciones.end());
    
    vector<size_t> docs_found;
    for (size_t i = 0; i < occs; ++i) {
        size_t pos = posiciones[i];
        size_t doc_id = doc_rank(pos + 1); 
        docs_found.push_back(doc_id);
    }
    
    // Eliminar duplicados (Estrategia B del reporte)
    auto last = unique(docs_found.begin(), docs_found.end());
    docs_found.erase(last, docs_found.end());
    
    for (auto doc_id : docs_found) {
        if (!filenames.empty() && doc_id <= filenames.size() && doc_id > 0) {
            cout << filenames[doc_id - 1] << endl;
        } else {
            cout << "Documento " << doc_id << endl;
        }
    }
  }
  return 0;
}
