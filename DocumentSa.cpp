#include <sdsl/suffix_arrays.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;
using namespace sdsl;


bool search(char* pat, char* txt, int_vector<> suffix_array, int n) {
    int m = strlen(pat);
    int l = 0, r = suffix_array.size() - 1;

    while (l <= r) {
        int mid = l + (r - l) / 2;
        int res = strncmp(pat, txt + suffix_array[mid], m);

        if (res == 0) return true;
        if (res < 0) r = mid - 1;
        else l = mid + 1;
    }
    return false;
}

class DocumentSuffixArray {
private:
    vector<int_vector<>> sas;
    vector<int_vector<>> texts;
    double total_mb;

public:
    DocumentSuffixArray(string T) {
        auto ss = istringstream(T);
        string infile;
        double total = 0.0;

        while (getline(ss, infile, '$')) {
            int_vector<> seq;
            load_vector_from_file(seq, infile, 1);

            int n = seq.size();
            seq.resize(n + 1);
            seq[n] = 0;

            int_vector<> sa(n + 1);
            algorithm::calculate_sa((const unsigned char*)seq.data(), n + 1, sa);

            texts.push_back(seq);
            sas.push_back(sa);
        }

        for (const auto& sa : sas)
            total += sdsl::size_in_bytes(sa);

        for (const auto& txt : texts)
            total += sdsl::size_in_bytes(txt);

        total_mb = total / (1024.0 * 1024.0);
    }

    double size_in_mb() const {
        return total_mb;
    }

    void doc_locate(string pattern) {
        for (size_t i = 0; i < sas.size(); i++) {
            search((char*)pattern.c_str(),
                   (char*)texts[i].data(),
                   sas[i],
                   texts[i].size());
        }
    }
};

