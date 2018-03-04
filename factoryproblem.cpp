//    Copyright (C) 2018 Michał Karol <michal.p.karol@gmail.com>

//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#include "factoryproblem.h"
#include <cassert>

//  Init function
std::function<FactoryProblem::FactoryChromosome(void)>
FactoryProblem::getFactoryRandomInitializationFunction(
    size_t numberOfLocations) {
    return [numberOfLocations]() {
        FactoryFenotype fenotype(numberOfLocations);

        //  Random init
        std::generate_n(std::back_inserter(fenotype.locations), numberOfLocations,
            [i = 0]() mutable { return i++; });
        std::shuffle(std::begin(fenotype.locations), std::end(fenotype.locations),
            RandomService::getService().getEngine());

        return FactoryChromosome(fenotype, 0U);
    };
}

//  Evaluation function
std::function<uint(FactoryProblem::FactoryChromosome&)>
FactoryProblem::getFactoryEvaluationFunction(
    const Matrix& distanceMatrix, const Matrix& flowMatrix) {
    return [&](FactoryChromosome& chromosome) {
        const vector<uint> locations = chromosome.fenotype.locations;

        chromosome.lastEvaluation = 10000U;
        for (size_t i = 0; i < locations.size(); i++) {
            const std::vector<uint> flowRow = flowMatrix[locations[i]];
            const std::vector<uint> distanceRow = distanceMatrix[i];

            for (size_t j = (i + 1); j < locations.size(); j++) {
                chromosome.lastEvaluation
                    -= 2 * (flowRow[locations[j]] * distanceRow[j]);
            }
        }
        return chromosome.lastEvaluation;
    };
}

uint FactoryProblem::factoryFitnessToResult(uint fitness) {
    return 10000U - fitness;
}

//  Crossing
std::tuple<FactoryProblem::FactoryChromosome, FactoryProblem::FactoryChromosome>
FactoryProblem::factoryOXCrossingFunction(
    const FactoryChromosome& parent1, const FactoryChromosome& parent2) {
    size_t numberOfLocations = parent1.fenotype.numberOfLocations;

    RandomService& service = RandomService::getService();
    auto indexGen
        = service.getRangeFunction<uint>(0, static_cast<uint>(numberOfLocations));

    auto generateChild = [&]() -> FactoryChromosome {
        uint indexL = indexGen();
        uint indexR = indexGen();

        if (indexL > indexR)
            std::swap(indexL, indexR);

        FactoryFenotype childFenotype(numberOfLocations);
        childFenotype.locations.resize(numberOfLocations);

        std::copy(std::cbegin(parent1.fenotype.locations) + indexL,
            std::cbegin(parent1.fenotype.locations) + indexR,
            std::begin(childFenotype.locations) + indexL);

        vector<uint> unusedLocations;
        std::copy_if(std::cbegin(parent2.fenotype.locations),
            std::cend(parent2.fenotype.locations),
            std::back_inserter(unusedLocations), [&](const uint location) {
                return std::find(std::cbegin(parent1.fenotype.locations) + indexL,
                           std::cbegin(parent1.fenotype.locations) + indexR, location)
                    == std::cbegin(parent1.fenotype.locations) + indexR;
            });

        uint unusedLocationsIndex = 0L;
        for (uint i = 0; i < indexL; i++, unusedLocationsIndex++) {
            childFenotype.locations[i] = unusedLocations[unusedLocationsIndex];
        }

        for (uint i = indexR; i < numberOfLocations; i++, unusedLocationsIndex++) {
            childFenotype.locations[i] = unusedLocations[unusedLocationsIndex];
        }

        return FactoryChromosome(childFenotype, 0U);
    };

    return std::make_tuple(generateChild(), generateChild());
}

std::tuple<FactoryProblem::FactoryChromosome, FactoryProblem::FactoryChromosome>
FactoryProblem::factorySymetricOXCrossingFunction(
    const FactoryChromosome& parent1, const FactoryChromosome& parent2) {
    size_t numberOfLocations = parent1.fenotype.numberOfLocations;

    RandomService& service = RandomService::getService();
    auto indexGen = service.getRangeFunction<uint>(0, static_cast<uint>(numberOfLocations));

    uint indexL = indexGen();
    uint indexR = indexGen();

    if (indexL > indexR)
        std::swap(indexL, indexR);

    FactoryFenotype child1Fenotype(numberOfLocations);
    child1Fenotype.locations.resize(numberOfLocations);

    FactoryFenotype child2Fenotype(numberOfLocations);
    child2Fenotype.locations.resize(numberOfLocations);

    auto generateFenotype = [&](const FactoryChromosome& parentA, const FactoryChromosome& parentB, FactoryFenotype& fenotype) {
        std::copy(std::cbegin(parentA.fenotype.locations) + indexL,
            std::cbegin(parentA.fenotype.locations) + indexR,
            std::begin(fenotype.locations) + indexL);

        vector<uint> unusedLocations;
        std::copy_if(std::cbegin(parentB.fenotype.locations),
            std::cend(parentB.fenotype.locations),
            std::back_inserter(unusedLocations), [&](const uint location) {
                return std::find(std::cbegin(parentA.fenotype.locations) + indexL,
                           std::cbegin(parentA.fenotype.locations) + indexR, location)
                    == std::cbegin(parentA.fenotype.locations) + indexR;
            });

        uint unusedLocationsIndex = 0L;
        for (uint i = 0; i < indexL; i++, unusedLocationsIndex++) {
            fenotype.locations[i] = unusedLocations[unusedLocationsIndex];
        }

        for (uint i = indexR; i < numberOfLocations; i++, unusedLocationsIndex++) {
            fenotype.locations[i] = unusedLocations[unusedLocationsIndex];
        }
    };

    generateFenotype(parent1, parent2, child1Fenotype);
    generateFenotype(parent2, parent1, child2Fenotype);

    return std::make_tuple(FactoryChromosome(child1Fenotype, 0U), FactoryChromosome(child2Fenotype, 0));
}

//  Mutating
void FactoryProblem::factorySwapMuatationFunction(FactoryChromosome& object) {
    RandomService& service = RandomService::getService();
    auto indexGen
        = service.getRangeFunction<size_t>(0, object.fenotype.numberOfLocations);

    uint indexA = static_cast<uint>(indexGen());
    uint indexB = static_cast<uint>(indexGen());

    std::iter_swap(std::begin(object.fenotype.locations) + indexA,
        std::begin(object.fenotype.locations) + indexB);
}
