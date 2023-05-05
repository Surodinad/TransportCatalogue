#include "input_reader.h"

#include <algorithm>

namespace transport_catalogue {

using namespace std;
using namespace input_reader;

void InputReader::ReadQuery(const std::string& query) {
    string_view text = query;
    detail::TrimSpaces(text);
    string_view type = text.substr(0, text.find_first_of(' '));
    text.remove_prefix(type.size());
    detail::TrimSpaces(text);
    if (type == "Stop"s) {
        Query result;
        result.type = QueryType::Stop;
        result.text = string(text);
        stop_queries_.push_back(result);
    } else {
        Query result;
        result.type = QueryType::Bus;
        result.text = string(text);
        bus_queries_.push_back(result);
    }
}

void detail::TrimSpaces(string_view& text) {
    size_t pos = text.find_first_not_of(' ');
    text.remove_prefix(pos);
    pos = text.find_last_not_of(' ');
    size_t sz = text.size();
    text.remove_suffix(sz - pos - 1);
}

vector<string_view> detail::Split(string_view query, char divider) {
    vector<string_view> result;
    string_view tmp = query;
    size_t rpos = 0;

    while (rpos != -1) {
        rpos = tmp.find_first_of(divider);
        string_view ires;

        if (rpos != -1) {
            ires = tmp.substr(0, rpos);
        } else {
            ires = tmp.substr(0, query.size());
        }

        detail::TrimSpaces(ires);
        result.push_back(ires);
        tmp.remove_prefix(rpos + 1);
    }

    return result;
}

void InputReader::ParseQueries(TransportCatalogue& catalogue_ref) const {
    for (const auto& query : stop_queries_) {
        vector<string_view> name_to_coords = detail::Split(query.text, ':');
        vector<string_view> coords_and_dists = detail::Split(name_to_coords[1], ',');
        structures::Stop tmp = {string(name_to_coords[0]), {stod(string(coords_and_dists[0])), stod(string(coords_and_dists[1]))}};
        catalogue_ref.AddStop(tmp);
    }

    for (const auto& query : bus_queries_) {
        vector<string_view> name_to_stops = detail::Split(query.text, ':');
        if (count(name_to_stops[1].begin(), name_to_stops[1].end(), '-')) {
            vector<string_view> stop_names = detail::Split(name_to_stops[1], '-');
            vector<string_view> rev_stop_names = stop_names;
            rev_stop_names.pop_back();
            std::reverse(rev_stop_names.begin(), rev_stop_names.end());
            structures::Bus tmp;
            tmp.name = name_to_stops[0];
            tmp.stops.reserve(stop_names.size()*2);
            for (string_view stop_name : stop_names) {
                tmp.stops.push_back(catalogue_ref.FindStop(stop_name));
            }
            for (string_view stop_name : rev_stop_names) {
                tmp.stops.push_back(catalogue_ref.FindStop(stop_name));
            }
            catalogue_ref.AddBus(tmp);
        } else {
            vector<string_view> stop_names = detail::Split(name_to_stops[1], '>');
            structures::Bus tmp;
            tmp.name = name_to_stops[0];
            tmp.stops.reserve(stop_names.size());
            for (string_view stop_name : stop_names) {
                tmp.stops.push_back(catalogue_ref.FindStop(stop_name));
            }
            catalogue_ref.AddBus(tmp);
        }
    }

    for (const auto& query : stop_queries_) {
        vector<string_view> name_to_coords = detail::Split(query.text, ':');
        vector<string_view> coords_and_dists = detail::Split(name_to_coords[1], ',');

        if (coords_and_dists.size() > 2) {
            for (int i = 2; i < coords_and_dists.size(); ++i) {
                size_t pos = coords_and_dists[i].find_first_of('m');
                double dist = stod(string(coords_and_dists[i].substr(0, pos)));
                coords_and_dists[i].remove_prefix(pos + 1);
                pos = coords_and_dists[i].find_first_not_of(' ');
                coords_and_dists[i].remove_prefix(pos + 2);
                detail::TrimSpaces(coords_and_dists[i]);
                catalogue_ref.SetRealDistance(catalogue_ref.FindStop(name_to_coords[0]), catalogue_ref.FindStop(coords_and_dists[i]), dist);
            }
        }
    }
}

}//namespace transport_catalogue