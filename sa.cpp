// Construcción del suffix array y la BWT de un texto
//
// Prerrequisitos: Tener la biblioteca SDSL instalada
//
// Compilación: g++ -O3 -o sa sa.cpp -lsdsl -ldivsufsort -ldivsufsort64

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sdsl/suffix_arrays.hpp>
#include <sstream>
#include <string>
#include <vector>

using namespace sdsl;
using namespace std;

bool search(char* pat, char* txt, int_vector<> suffix_array, int n) {
  int m = strlen(pat);

  int l = 0, r = suffix_array.size() - 1;
  while (l <= r) {
    int mid = l + (r - l) / 2;
    int res = strncmp(pat, txt + suffix_array[mid], m);

    if (res == 0)
      return true;

    if (res < 0) r = mid -1;
    else l = mid + 1;
  }

  return false;
}

vector<string> doc_locate(string T, char* p) {
  auto result = vector<string>();
  auto ss = istringstream(T);
  string infile;
  int n;

  while (std::getline(ss, infile, '$')) {
    int_vector<> seq;

    load_vector_from_file(seq, infile, 1);
    n = seq.size();

    seq.resize(n + 1);
    n = seq.size();
    seq[n - 1] = 0; // Representa el final de texto. Suele representarse por el
                    // símbolo $

    int_vector<> sa(1, 0, bits::hi(n) + 1);
    sa.resize(n);
    algorithm::calculate_sa((const unsigned char *)seq.data(), n, sa);

    int_vector<> bwt(1, 0, 8);
    bwt.resize(n);

    int32_t to_add[2] = {(int32_t)-1, n - 1};
    for (int32_t i = 0; i < n; ++i)
      bwt[i] = seq[sa[i] + to_add[sa[i] == 0]];

    if (search(p, (char*) seq.data(), sa, n))
      result.push_back(infile);
  }

  return result;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    cout << "Uso: " << argv[0] << " <archivo entrada>" << endl;
    return 1;
  }

  char* archivos = argv[1];
  char* patrones = argv[2];

  auto nombres = doc_locate(archivos, patrones);

  for (auto nombre: nombres)
    cout << nombre << endl;

  return 0;
}
