#pragma once

#include "transport_catalogue.h"

namespace transport_catalogue {

namespace stat_reader {

enum class QueryType {
    Stop,
    Bus
};

struct Query {
    QueryType type;
    std::string name;
};


class StatReader {
public:
    void ProcessQuery(std::ostream& os, TransportCatalogue& catalogue_ref, const std::string& query_text);

private:
    stat_reader::Query ReadQuery(const std::string& query);
};

}//namespace stat_reader

}//namespace transport_catalogue