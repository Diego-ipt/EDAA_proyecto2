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

class DocumentFMIndex {
private:
    csa_wt<wt_huff<rrr_vector<127>>> fm_index;
    sd_vector<> doc_borders;
    sd_vector<>::rank_1_type doc_rank;
    size_t num_docs;

public:
    DocumentFMIndex(string filename) {
        cout << "Construyendo el FM-index compactado (Huffman + RRR)..." << endl;
        construct(fm_index, filename, 1);
        cout << "Tamaño del FM-index " << size_in_mega_bytes(fm_index) << " MB." << endl;

        cout << "Construyendo mapa de documentos (sd_vector)..." << endl;
        string sep_str = "\x02";
        
        vector<size_t> boundary_positions;
        boundary_positions.push_back(0); 
        
        size_t occs_sep = sdsl::count(fm_index, sep_str.begin(), sep_str.end());
        if (occs_sep > 0) {
            auto sep_locs = sdsl::locate(fm_index, sep_str.begin(), sep_str.end());
            sort(sep_locs.begin(), sep_locs.end());
            for (auto loc : sep_locs) {
                boundary_positions.push_back(loc + 1);
            }
        }
        
        bit_vector b(fm_index.size(), 0);
        for (size_t pos : boundary_positions) {
            if (pos < b.size()) {
                b[pos] = 1;
            }
        }
        
        doc_borders = sd_vector<>(b);
        doc_rank = sd_vector<>::rank_1_type(&doc_borders);
        num_docs = boundary_positions.size();
        cout << "Documentos detectados: " << num_docs << endl;
    }

    // Funcionalidad explícita doc_locate
    vector<size_t> doc_locate(const string& patron) {
        size_t occs = sdsl::count(fm_index, patron.begin(), patron.end());
        cout << "# de ocurrencias: " << occs << endl;
        
        vector<size_t> docs_found;
        if (occs > 0) {
            auto posiciones = sdsl::locate(fm_index, patron.begin(), patron.end());
            sort(posiciones.begin(), posiciones.end());
            
            for (size_t i = 0; i < occs; ++i) {
                size_t pos = posiciones[i];
                size_t doc_id = doc_rank(pos + 1); 
                docs_found.push_back(doc_id);
            }
            
            auto last = unique(docs_found.begin(), docs_found.end());
            docs_found.erase(last, docs_found.end());
        }
        return docs_found;
    }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "Uso: " << argv[0] << " <archivo entrada> [archivo nombres]" << endl;
    return 1;
  }

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

  // Instanciar la clase que provee la funcionalidad
  DocumentFMIndex doc_index(argv[1]);

  string patron;
  cout << "Ingrese un patrón a buscar: ";
  cin >> patron;

  // Llamada explícita a doc_locate
  vector<size_t> docs = doc_index.doc_locate(patron);

  if (!docs.empty()) {
    cout << "Las ocurrencias se encuentran en los siguientes documentos: " << endl;
    for (auto doc_id : docs) {
        if (!filenames.empty() && doc_id <= filenames.size() && doc_id > 0) {
            cout << filenames[doc_id - 1] << endl;
        } else {
            cout << "Documento " << doc_id << endl;
        }
    }
  }
  return 0;
}
