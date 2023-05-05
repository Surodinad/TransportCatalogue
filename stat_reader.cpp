#include "stat_reader.h"
#include "input_reader.h"

#include <iomanip>

namespace transport_catalogue {

using namespace std;
using namespace stat_reader;

Query StatReader::ReadQuery(const std::string& query) {
    string_view text = query;
    detail::TrimSpaces(text);
    string_view type = text.substr(0, text.find_first_of(' '));
    text.remove_prefix(type.size());
    detail::TrimSpaces(text);
    Query result;
    if (type == "Bus") {
        result.type = QueryType::Bus;
    } else {
        result.type = QueryType::Stop;
    }
    result.name = string(text);
    return result;
}

void StatReader::ProcessQuery(std::ostream& os, TransportCatalogue& catalogue_ref, const std::string& query_text) {
    Query query = ReadQuery(query_text);

    if (query.type == QueryType::Stop) {
        auto stats = catalogue_ref.GetStopInfo(query.name);
        if (stats.first == 0) {
            os <<  "Stop "s << query.name << ": not found"s << endl;
        } else if (stats.second.empty()) {
            os <<  "Stop "s << query.name << ": no buses"s << endl;
        } else {
            os <<  "Stop "s << query.name << ": buses"s;
            for (const auto& bus : stats.second) {
                os << ' ' << bus;
            }
            os << endl;
        }
    } else {
        auto stats = catalogue_ref.GetBusInfo(query.name);
        if (get<0>(stats) == 0) {
            os << "Bus "s << query.name << ": not found"s << endl;
        } else {
            os << "Bus "s << query.name << ": " << get<0>(stats)
               << " stops on route, "s << get<1>(stats) << " unique stops, "s
               << std::setprecision(6) << get<2>(stats) << std::setprecision(20)
               << " route length, "s << std::setprecision(6) << get<3>(stats) << std::setprecision(20)
               << " curvature"s << endl;
        }
    }
}

}//namespace transport_catalogue