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
#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H
#include <functional>
#include <random>
#include <vector>

// Interfaces
template <class Fenotype, class Eval>
struct Chromosome {
    using TypedChromosome = Chromosome<Fenotype, Eval>;
    Chromosome(Fenotype fenotype, Eval eval)
        : fenotype(fenotype), lastEvaluation(eval) {
    }

    Fenotype fenotype;
    Eval lastEvaluation;

    bool operator<(const Chromosome& other) const {
        return lastEvaluation < other.lastEvaluation;
    }
};

template <class Fenotype, class Eval>
struct InitializationFunction {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    virtual ~InitializationFunction() = default;
    virtual Population operator()() const = 0;
};

template <class Fenotype, class Eval>
struct EvaluationFunction {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    virtual ~EvaluationFunction() = default;
    virtual Eval operator()(Population&) const = 0;
};

template <class Fenotype, class Eval>
struct StopCondition {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    virtual ~StopCondition() = default;
    virtual bool operator()(const Population&, const Eval&) = 0;
};

template <class Fenotype, class Eval>
struct LoggingFunction {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    virtual ~LoggingFunction() = default;
    virtual void operator()(const Population&) = 0;
    virtual void show() const = 0;
};

template <class Fenotype, class Eval>
struct SelectionFunction {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    virtual ~SelectionFunction() = default;
    virtual Population operator()(const Population&) const = 0;
};

template <class Fenotype, class Eval>
struct CrossoverFunction {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    virtual ~CrossoverFunction() = default;
    virtual void operator()(Population&) const = 0;
};

template <class Fenotype, class Eval>
struct MutationFunction {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    virtual ~MutationFunction() = default;
    virtual void operator()(Population&) const = 0;
};

// Algorithm
template <class Fenotype, class Eval>
struct GeneticAlgorithm {
    using Subject = Chromosome<Fenotype, Eval>;
    using Population = std::vector<Subject>;

    static Subject
    optimize(const InitializationFunction<Fenotype, Eval>& initializationFunction,
        const EvaluationFunction<Fenotype, Eval>& evaluationFunction,
        StopCondition<Fenotype, Eval>& stopCondition,
        LoggingFunction<Fenotype, Eval>& loggingFunction,

        const SelectionFunction<Fenotype, Eval>& selectionFunction,
        const CrossoverFunction<Fenotype, Eval>& crossoverFunction,
        const MutationFunction<Fenotype, Eval>& mutationFunction) {

        // Initialization and first evaluation
        Population population = initializationFunction();
        Eval evaluation = evaluationFunction(population);
        loggingFunction(population);

        // Main algorith loop
        while (!stopCondition(population, evaluation)) {
            population = selectionFunction(population);
            crossoverFunction(population);
            mutationFunction(population);
            evaluation = evaluationFunction(population);
            loggingFunction(population);
        }

        // Returning best subject form population
        return *std::max_element(std::cbegin(population), std::cend(population));
    }
};

#endif // GENETICALGORITHM_H
