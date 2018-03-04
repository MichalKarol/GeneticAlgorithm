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
#include "error.cpp"
#include "factoryproblem.h"
#include "generics.h"
#include "geneticalgorithm.h"
#include "greedysearch.h"
#include "matrix.h"
#include "randomsearch.h"
#include <fstream>
#include <iostream>
#include <numeric>

typedef unsigned int uint;

using Eval = uint;

const std::string PATH = "had20.dat";

const size_t POPULATION_SIZE = 100;
const size_t MAX_ITERATION_COUNT = 50;
const uint TOURNAMENT_SIZE = 100;
const double CROSSING_PROBABILITY = 0.70;
const double MUTATING_PROBABILITY = 0.20;

int main() {
    // Input
    std::ifstream file;
    file.open(PATH);

    if (file) {
        // Reading matrix size
        size_t matrixSize;
        file >> matrixSize;

        // Skipping one line
        file.ignore(1);

        // Reading matrix
        Matrix distanceMatrix(matrixSize, matrixSize);
        Matrix flowMatrix(matrixSize, matrixSize);

        file >> flowMatrix >> distanceMatrix;

        auto serachInitializationFunction = [&]() -> std::vector<uint> {
            std::vector<uint> init;
            std::generate_n(std::back_inserter(init), matrixSize, [i = uint(0)]() mutable {
                return i++;
            });
            return init;
        };

        auto searchEvaluationFunction = [&](const std::vector<uint>& permuatation) -> uint {
            uint base = 10000U;
            for (size_t i = 0; i < permuatation.size(); i++) {
                const std::vector<uint> flowRow = flowMatrix[permuatation[i]];
                const std::vector<uint> distanceRow = distanceMatrix[i];

                for (size_t j = (i + 1); j < permuatation.size(); j++) {
                    base -= 2 * (flowRow[permuatation[j]] * distanceRow[j]);
                }
            }
            return base;
        };

        // Other types of algorithms
        //        RandomSearch::search(serachInitializationFunction, searchEvaluationFunction, 1000);
        //        GreedySearch::search(serachInitializationFunction, searchEvaluationFunction);

        // Genetic algorithm
        using Fenotype = FactoryProblem::FactoryFenotype;
        using Eval = uint;

        GenericRandomInitializationFunction<Fenotype, Eval> initializationFunction(POPULATION_SIZE, matrixSize, FactoryProblem::getFactoryRandomInitializationFunction(matrixSize));
        GenericEvaluationFunction<Fenotype, Eval> evaluationFunction(FactoryProblem::getFactoryEvaluationFunction(distanceMatrix, flowMatrix));
        GenericIterationCountStopCondition<Fenotype, Eval> stopCondition(MAX_ITERATION_COUNT);
        GenericJSLoggingFunction<Fenotype, Eval> loggingFunction(FactoryProblem::factoryFitnessToResult);

        GenericTournamentSelectionFunction<Fenotype, Eval> selectionFunction(TOURNAMENT_SIZE, POPULATION_SIZE);
        GenericCrossoverFunction<Fenotype, Eval> crossoverFunction(CROSSING_PROBABILITY, FactoryProblem::factorySymetricOXCrossingFunction);
        GenericMutationFunction<Fenotype, Eval> mutationFunction(MUTATING_PROBABILITY, FactoryProblem::factorySwapMuatationFunction);

        const Chromosome<Fenotype, Eval> found = GeneticAlgorithm<Fenotype, Eval>::optimize(
            std::ref(initializationFunction),
            std::ref(evaluationFunction),
            std::ref(stopCondition),
            std::ref(loggingFunction),

            std::ref(selectionFunction),
            std::ref(crossoverFunction),
            std::ref(mutationFunction));

        loggingFunction.show();

        std::cout << FactoryProblem::factoryFitnessToResult(found.lastEvaluation) << "\n";
        return static_cast<int>(Error::NO_ERROR);
    }

    return static_cast<int>(Error::NO_ERROR);
}
