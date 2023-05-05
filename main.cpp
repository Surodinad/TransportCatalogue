#include <iostream>
#include <fstream>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;
using namespace transport_catalogue;

string ReadLine(std::istream& is) {
    string s;
    getline(is, s);
    return s;
}

int ReadLineWithNumber(std::istream& is) {
    int result = 0;
    is >> result;
    ReadLine(is);
    return result;
}

int main() {
    TransportCatalogue catalogue;
    input_reader::InputReader reader;
    stat_reader::StatReader outer;

    int n = ReadLineWithNumber(cin);

    for (int i = 0; i < n; ++i) {
        string query = ReadLine(cin);
        reader.ReadQuery(query);
    }

    reader.ParseQueries(catalogue);

    n = ReadLineWithNumber(cin);

    for (int i = 0; i < n; ++i) {
        string query = ReadLine(cin);
        outer.ProcessQuery(cout, catalogue, query);
    }

    return 0;
}
