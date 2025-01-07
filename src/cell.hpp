#pragma once

#include <cmath>
#include <nlohmann/json.hpp>
#include <cadmium/celldevs/grid/cell.hpp>
#include <cadmium/celldevs/grid/config.hpp>
#include <behave/surface.h>
#include <behave/fuelModels.h>
#include "state.hpp"

class GridCell : public cadmium::celldevs::GridCell<State, double>
{
    double slope;
    double aspect;
    int fuelModelNumber;
    double windDirection;
    double windSpeed;

public:
    GridCell(const std::vector<int>& id, const std::shared_ptr<
            const cadmium::celldevs::GridCellConfig<State, double>>& config)
        : cadmium::celldevs::GridCell<State, double>(id, config)
    {
        slope = config->rawCellConfig.at("slope");
        aspect = config->rawCellConfig.at("aspect");
        fuelModelNumber = config->rawCellConfig.at("fuelModelNumber");
        windDirection = config->rawCellConfig.at("windDirection");
        windSpeed = config->rawCellConfig.at("windSpeed");
    }

    [[nodiscard]] State localComputation(State state, const std::unordered_map<
        std::vector<int>, cadmium::celldevs::NeighborData<State, double>>& neighborhood) const override
    {
        if (state.ignited)
        {
            return state;
        }
        if (state.willIgnite)
        {
            FuelModels fuelModels;
            Surface surface(fuelModels);
            surface.setSlope(slope, SlopeUnits::Percent);
            surface.setAspect(aspect);
            surface.setFuelModelNumber(fuelModelNumber);
            surface.setWindDirection(windDirection);
            surface.setWindSpeed(windSpeed, SpeedUnits::MetersPerSecond, WindHeightInputMode::DirectMidflame);
            surface.doSurfaceRunInDirectionOfMaxSpread();
            state.ignited = true;
            state.spreadDirection = surface.getDirectionOfMaxSpread() * M_PI / 180.0f;
            state.spreadRate = surface.getSpreadRate(SpeedUnits::MetersPerSecond);
            return state;
        }
        for (const auto& [neighborId, neighborData]: neighborhood)
        {
            if (!neighborData.state->ignited)
            {
                continue;
            }
            cadmium::celldevs::coordinates vectorFromNeighbor = distanceVectorFrom(neighborId);
            double directionFromNeighbor = atan2(vectorFromNeighbor.at(0), vectorFromNeighbor.at(1));
            if (directionFromNeighbor < 0.0001f)
            {
                continue;
            }
            if (abs(neighborData.state->spreadDirection - directionFromNeighbor) < M_PI / 4.0f)
            {
                double timeToIgnite = neighborData.vicinity / neighborData.state->spreadRate;
                state.timeToIgnite = fmin(state.timeToIgnite, timeToIgnite);
                state.willIgnite = true;
            }
        }
        return state;
    }

    [[nodiscard]] double outputDelay(const State& state) const override
    {
        if (state.ignited)
        {
            return 1.0f;
        }
        else if (state.willIgnite)
        {
            return state.timeToIgnite;
        }
        else
        {
            return 1.0f;
        }
    }
};