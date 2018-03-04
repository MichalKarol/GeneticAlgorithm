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
#include "greedysearch.h"

std::vector<uint> GreedySearch::search(
    std::function<std::vector<uint>(void)> initializationFunction,
    std::function<uint(const std::vector<uint>&)> evaluationFunction) {
    std::vector<uint> basePermutation = initializationFunction();

    std::vector<uint> bestPermuatation;
    uint bestFitness = 0;

    std::sort(std::begin(basePermutation), std::end(basePermutation));
    do {
        uint fitness = evaluationFunction(basePermutation);

        if (fitness > bestFitness) {
            bestPermuatation.clear();
            std::copy(std::begin(basePermutation), std::end(basePermutation),
                std::back_inserter(bestPermuatation));
            std::cout << "From: " << bestFitness << " to " << fitness << "\n";
            bestFitness = fitness;
        }
    } while (std::next_permutation(
        std::begin(basePermutation), std::end(basePermutation)));

    std::cout << "Greedy alg: " << bestFitness << "\n";
    return bestPermuatation;
}
