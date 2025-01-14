#pragma once

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

struct State
{
    double spreadRate;
    double spreadDirection;
    double timeToIgnite;
    bool willIgnite;
    bool ignited;

    State()
        : spreadRate(0.0f)
        , spreadDirection(0.0f)
        , timeToIgnite(INFINITY)
        , ignited(false)
        , willIgnite(false) {}
};

inline bool operator!=(const State& x, const State& y)
{
    return x.ignited != y.ignited || x.willIgnite != y.willIgnite;
}

inline std::ostream& operator<<(std::ostream& os, const State& x)
{
    return os << x.ignited;
}

inline void from_json(const nlohmann::json& j, State& s)
{
    s.willIgnite = j.at("ignited");
}