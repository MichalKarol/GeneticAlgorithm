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
#ifndef FACTORYPROBLEM_H
#define FACTORYPROBLEM_H
#include "geneticalgorithm.h"
#include "matrix.h"
#include "randomservice.h"
#include <algorithm>
#include <array>
#include <functional>
#include <tuple>
#include <vector>

namespace FactoryProblem {
struct FactoryFenotype {
    FactoryFenotype(size_t numberOfLocations)
        : numberOfLocations(numberOfLocations) {
    }

    std::vector<uint> locations;
    size_t numberOfLocations;
};

using FactoryChromosome = Chromosome<FactoryFenotype, uint>;

// Initialization
std::function<FactoryChromosome(void)> getFactoryRandomInitializationFunction(size_t numberOfLocations);

// Evaluation
std::function<uint(FactoryChromosome&)> getFactoryEvaluationFunction(const Matrix& distanceMatrix, const Matrix& flowMatrix);
uint factoryFitnessToResult(uint fitness);

// Crossings
std::tuple<FactoryChromosome, FactoryChromosome> factoryOXCrossingFunction(const FactoryChromosome& parent1, const FactoryChromosome& parent2);
std::tuple<FactoryChromosome, FactoryChromosome> factorySymetricOXCrossingFunction(const FactoryChromosome& parent1, const FactoryChromosome& parent2);

// Muatation
void factorySwapMuatationFunction(FactoryChromosome& object);
} // namespace FactoryProblem

#endif // FACTORYPROBLEM_H
