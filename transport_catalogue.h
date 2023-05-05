#pragma once

#include <iostream>
#include <string_view>
#include <vector>
#include <deque>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "geo.h"

namespace transport_catalogue {

namespace structures {

struct Stop {
    std::string name;
    Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
};

bool operator==(const Stop& lhs, const Stop& rhs);

bool operator<(const Bus& lhs, const Bus& rhs);

using Distance = double;
using StopPtrPair = std::pair<const Stop*, const Stop*>;

namespace hashes {

class StopHasher {
public:
    size_t operator()(const Stop& stop) const;
private:
    std::hash<std::string> hasher_;
};

class StopPtrPairHasher {
public:
    size_t operator()(const StopPtrPair& s_p_pair) const;
private:
    std::hash<const void*> ptr_hasher_;
};

} //namespace hashes

} //namespace structures

class TransportCatalogue {
public:
    void AddStop(structures::Stop& stop);
    void AddBus(structures::Bus& bus);
    structures::Stop* FindStop(std::string_view name) const;
    structures::Bus* FindBus(std::string_view name) const;
    std::tuple<size_t, size_t, structures::Distance, structures::Distance> GetBusInfo(const std::string& bus_name);
    std::pair<int, std::vector<std::string_view>> GetStopInfo(const std::string& stop_name);
    void SetRealDistance(const structures::Stop* const first_stop, const structures::Stop* const second_stop, structures::Distance distance);

private:
    std::deque<structures::Stop> stops_;
    std::deque<structures::Bus> buses_;
    std::unordered_map<std::string_view, structures::Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, structures::Bus*> busname_to_bus_;
    std::unordered_map<const structures::Stop*, std::deque<structures::Bus*>> stop_to_buses_;
    std::unordered_map<structures::StopPtrPair, structures::Distance, structures::hashes::StopPtrPairHasher> stop_pairs_to_distance_;

    structures::Distance GetDistance(const structures::Stop* const first_stop, const structures::Stop* const second_stop);
    structures::Distance GetGeoDist(const structures::Stop* const first_stop, const structures::Stop* const second_stop) const;
};

} //namespace transport_catalogue