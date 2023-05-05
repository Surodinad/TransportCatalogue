#pragma once

#include <iostream>
#include <string_view>
#include <deque>

#include "transport_catalogue.h"

namespace transport_catalogue {

namespace input_reader {

enum class QueryType {
    Stop,
    Bus
};

struct Query {
    QueryType type;
    std::string text;
};

class InputReader {
public:
    void ReadQuery(const std::string& query);
    void ParseQueries(TransportCatalogue& catalogue_ref) const;

private:
    std::deque<Query> stop_queries_;
    std::deque<Query> bus_queries_;
};

}//namespace input_reader

namespace detail {

void TrimSpaces(std::string_view& text); //Обрезает пробелы по краям строки

std::vector<std::string_view> Split(std::string_view query, char divider);

}//namespace detail

}//namespace transport_catalogue