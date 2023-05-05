#include "transport_catalogue.h"

#include <algorithm>

namespace transport_catalogue {

using namespace std;
using namespace structures;
using namespace structures::hashes;

size_t StopHasher::operator()(const Stop& stop) const {
    return hasher_(stop.name);
}

size_t StopPtrPairHasher::operator()(const StopPtrPair& s_p_pair) const {
    return ptr_hasher_(s_p_pair.first) + ptr_hasher_(s_p_pair.second) * 19;
}

bool operator==(const Stop& lhs, const Stop& rhs) {
    return lhs.name == rhs.name && lhs.coordinates == rhs.coordinates;
}

bool operator<(const Bus& lhs, const Bus& rhs) {
    return lhs.name < rhs.name;
}

Distance TransportCatalogue::GetDistance(const Stop* const first_stop, const Stop* const second_stop) {
    StopPtrPair stop_pair = StopPtrPair(first_stop, second_stop);
    if (stop_pairs_to_distance_.count(stop_pair)) {
        return stop_pairs_to_distance_.at(stop_pair);
    } else {
        Distance res = ComputeDistance(first_stop->coordinates, second_stop->coordinates);
        stop_pairs_to_distance_[stop_pair] = res;
        return res;
    }
}

Distance TransportCatalogue::GetGeoDist(const Stop* const first_stop, const Stop* const second_stop) const {
    return ComputeDistance(first_stop->coordinates, second_stop->coordinates);
}

void TransportCatalogue::SetRealDistance(const Stop* const first_stop, const Stop* const second_stop, Distance distance) {
    StopPtrPair stop_pair_1 = StopPtrPair(first_stop, second_stop);
    StopPtrPair stop_pair_2 = StopPtrPair(second_stop, first_stop);
    stop_pairs_to_distance_[stop_pair_1] = distance;
    if (!stop_pairs_to_distance_.count(stop_pair_2)) {
        stop_pairs_to_distance_[stop_pair_2] = distance;
    }
}

void TransportCatalogue::AddStop(Stop& stop) {
    stops_.push_back(stop);
    stopname_to_stop_.emplace(stops_.back().name, &stops_.back());
}

void TransportCatalogue::AddBus(Bus& bus) {
    buses_.push_back(bus);
    busname_to_bus_.emplace(buses_.back().name, &buses_.back());
    for (const auto& stop : bus.stops) {
        stop_to_buses_[stop].push_back(&buses_.back());
    }
}

structures::Stop* TransportCatalogue::FindStop(std::string_view name) const {
    if (stopname_to_stop_.count(name)) {
        return stopname_to_stop_.at(name);
    } else {
        return nullptr;
    }
}

structures::Bus* TransportCatalogue::FindBus(std::string_view name) const {
    if (busname_to_bus_.count(name)) {
        return busname_to_bus_.at(name);
    } else {
        return nullptr;
    }
}

tuple<size_t, size_t, Distance, Distance> TransportCatalogue::GetBusInfo(const string& bus_name) {
    size_t stops_on_route, unique_stops;
    Distance route_length = 0.0, curvature = 0.0;

    const Bus* bus = FindBus(bus_name);

    if (bus) {
        stops_on_route = bus->stops.size();

        unordered_set<string_view> unique_names;
        for (const auto& stop : bus->stops) {
            unique_names.insert(stop->name);
        }
        unique_stops = unique_names.size();

        double geo_dist = 0.0;
        auto fit = bus->stops.begin();
        auto sit = fit;
        ++sit;
        while (sit != bus->stops.end()) {
            route_length += GetDistance(*fit, *sit);
            geo_dist += GetGeoDist(*fit, *sit);
            ++fit;
            ++sit;
        }

        curvature = route_length / geo_dist;

        return {stops_on_route, unique_stops, route_length, curvature};

    } else {
        return {0, 0, 0.0, 0.0};
    }
}

pair<int, vector<string_view>> TransportCatalogue::GetStopInfo(const string& stop_name) {
    pair<int, vector<string_view>> result;

    Stop* stop = FindStop(stop_name);

    if (stopname_to_stop_.count(stop_name)) {
        result.first = 1;
        if (stop_to_buses_.count(stop)) {
            result.second.reserve(stop_to_buses_.at(stop).size());
            for (const auto& bus : stop_to_buses_.at(stop)) {
                result.second.push_back(bus->name);
            }
            sort(result.second.begin(), result.second.end());
            result.second.erase(unique(result.second.begin(), result.second.end()), result.second.end());
        }
    }

    return result;
}

}//namespace transport_catalogue