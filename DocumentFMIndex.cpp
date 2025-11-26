#include "DocumentFMIndex.hpp"
#include <sdsl/suffix_arrays.hpp>
#include <algorithm>
#include <iostream>

using namespace sdsl;
using namespace std;

DocumentFMIndex::DocumentFMIndex(const string& filename) {

    cout << "Construyendo FM-index...\n";
    construct(fm_index, filename, 1);

    string sep_str = "\x02";

    vector<size_t> boundaries;
    boundaries.push_back(0);

    size_t occ = sdsl::count(fm_index, sep_str.begin(), sep_str.end());
    if (occ > 0) {
        auto locs = sdsl::locate(fm_index, sep_str.begin(), sep_str.end());
        sort(locs.begin(), locs.end());
        for (auto p : locs) boundaries.push_back(p + 1);
    }

    bit_vector bv(fm_index.size(), 0);
    for (auto p : boundaries)
        if (p < bv.size()) bv[p] = 1;

    doc_borders = sd_vector<>(bv);
    doc_rank = sd_vector<>::rank_1_type(&doc_borders);

    num_docs = boundaries.size();
}

vector<size_t> DocumentFMIndex::doc_locate(const string& pattern) {
    vector<size_t> result;

    size_t occ = sdsl::count(fm_index, pattern.begin(), pattern.end());
    if (occ == 0) return result;

    auto positions = sdsl::locate(fm_index, pattern.begin(), pattern.end());
    sort(positions.begin(), positions.end());

    for (size_t pos : positions) {
        size_t doc_id = doc_rank(pos + 1);
        result.push_back(doc_id);
    }

    result.erase(unique(result.begin(), result.end()), result.end());
    return result;
}
