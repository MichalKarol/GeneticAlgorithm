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
#ifndef GENERICS_H
#define GENERICS_H
#include "geneticalgorithm.h"
#include "randomservice.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

template <class Fenotype, class Eval>
struct GenericRandomInitializationFunction
    : public InitializationFunction<Fenotype, Eval> {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    GenericRandomInitializationFunction(
        size_t populationSize, size_t randomRange,
        std::function<Chromosome<Fenotype, Eval>(void)> initFunction)
        : populationSize(populationSize), randomRange(randomRange), initFunction(initFunction) {
    }

    const size_t populationSize;
    const size_t randomRange;
    std::function<Chromosome<Fenotype, Eval>(void)> initFunction;

    Population operator()() const override {
        // Init and generate genes
        Population init;
        std::generate_n(std::back_inserter(init), populationSize, initFunction);

        return init;
    }
};

template <class Fenotype, class Eval>
struct GenericEvaluationFunction : public EvaluationFunction<Fenotype, Eval> {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    GenericEvaluationFunction(
        std::function<Eval(Chromosome<Fenotype, Eval>&)> evalFunction)
        : evalFunction(evalFunction) {
    }
    const std::function<Eval(Chromosome<Fenotype, Eval>&)> evalFunction;

    Eval operator()(Population& population) const override {
        std::vector<Eval> scores;
        std::transform(std::begin(population), std::end(population),
            std::back_inserter(scores), evalFunction);
        return std::accumulate(std::cbegin(scores), std::cend(scores), Eval());
    }
};

template <class Fenotype, class Eval>
struct GenericIterationCountStopCondition
    : public StopCondition<Fenotype, Eval> {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;

    GenericIterationCountStopCondition(size_t maxIterations)
        : maxIterations(maxIterations) {
    }

    size_t currentIteration = 0;
    const size_t maxIterations;

    bool operator()(const Population&, const Eval&) override {
        return currentIteration++ >= maxIterations;
    }
};

template <class Fenotype, class Eval>
struct GenericConsoleLoggingFunction : public LoggingFunction<Fenotype, Eval> {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;
    size_t currentIteration = 0;

    GenericConsoleLoggingFunction(std::function<long(Eval)> fitnessToResult)
        : fitnessToResult(fitnessToResult) {
    }

    std::function<long(Eval)> fitnessToResult;

    void operator()(const Population& population) override {
        std::cout << "-------------------------------------------\n";
        std::cout << "Iteration: " << currentIteration << "\n";
        std::cout << "Population size: " << population.size() << "\n";
        std::cout << "Max value: "
                  << fitnessToResult((*std::max_element(std::cbegin(population),
                                          std::cend(population)))
                                         .lastEvaluation)
                  << "\n";
        std::cout << "Mean value: "
                  << fitnessToResult(std::accumulate(
                         std::cbegin(population), std::cend(population), Eval(),
                         [](Eval accumulator,
                             const Chromosome<Fenotype, Eval> chromosome) {
                             return accumulator += chromosome.lastEvaluation;
                         }))
                / static_cast<Eval>(population.size())
                  << "\n";
        std::cout << "Min value: "
                  << fitnessToResult((*std::min_element(std::cbegin(population),
                                          std::cend(population)))
                                         .lastEvaluation)
                  << std::endl;
        currentIteration++;
    }
    void show() override {}
};

template <class Fenotype, class Eval>
struct GenericJSLoggingFunction : public LoggingFunction<Fenotype, Eval> {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;
    std::vector<Eval> min;
    std::vector<Eval> avg;
    std::vector<Eval> max;

    GenericJSLoggingFunction(std::function<long(Eval)> fitnessToResult)
        : fitnessToResult(fitnessToResult) {
    }

    std::function<long(Eval)> fitnessToResult;

    void operator()(const Population& population) override {
        max.push_back(
            (*std::max_element(std::cbegin(population), std::cend(population)))
                .lastEvaluation);
        avg.push_back(
            std::accumulate(
                std::cbegin(population), std::cend(population), Eval(),
                [](Eval accumulator, const Chromosome<Fenotype, Eval> chromosome) {
                    return accumulator += chromosome.lastEvaluation;
                })
            / static_cast<Eval>(population.size()));
        min.push_back(
            (*std::min_element(std::cbegin(population), std::cend(population)))
                .lastEvaluation);
    }

    void show() const override {
        std::ofstream htmlFile("outFile.html");
        if (htmlFile) {
            htmlFile << R"(<!DOCTYPE html>
                            <html>
                            <head>
                            <title>Results</title>
                            <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.2/Chart.bundle.min.js"></script>
                            </head>
                            <body>)";
            htmlFile << R"(<canvas id="myChart" ></canvas>)";
            htmlFile << R"(<script>
                            let ctx = document.getElementById("myChart").getContext('2d');
                            let myChart = new Chart(ctx, {
                            type: 'line',
                            data: {
                            labels: [)";
            std::for_each(
                std::cbegin(min), std::cend(min),
                [&, i = 0U](const Eval) mutable { htmlFile << i++ << ","; });

            htmlFile << R"(],
                            datasets: [{
                            label: 'Min',
                            data: [)";
            std::for_each(std::cbegin(min), std::cend(min),
                [&](const Eval value) mutable {
                    htmlFile << fitnessToResult(value) << ",";
                });
            htmlFile << R"(],
                            borderColor: "blue",
                            fill: false,
                            },

                            {
                            label: 'Avg',
                            data: [)";
            std::for_each(std::cbegin(avg), std::cend(avg),
                [&](const Eval value) mutable {
                    htmlFile << fitnessToResult(value) << ",";
                });
            htmlFile << R"(],
                            borderColor: "grey",
                            fill: false,
                            },

                            {
                            label: 'Max',
                            data: [)";
            std::for_each(std::cbegin(max), std::cend(max),
                [&](const Eval value) mutable {
                    htmlFile << fitnessToResult(value) << ",";
                });
            htmlFile << R"(],
                            borderColor: "red",
                            fill: false,
                            }]
                            },
                            options: {
                            scales: {
                            yAxes: [{
                            ticks: {
                            beginAtZero:true
                            }
                            }]
                            }
                            }
                            });
                            </script>)";
            htmlFile << R"(</body></html>)";
            htmlFile.close();
            system("vivaldi-snapshot outFile.html");
        }
    }
};

// Selection functions
template <class Fenotype, class Eval>
struct GenericTournamentSelectionFunction
    : public SelectionFunction<Fenotype, Eval> {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;
    GenericTournamentSelectionFunction(size_t tournamentSize,
        size_t populationSize)
        : tournamentSize(tournamentSize), populationSize(populationSize) {
    }

    size_t tournamentSize;
    size_t populationSize;

    // Random generator
    RandomService& service = RandomService::getService();
    std::function<size_t(void)> pickIndex = service.getRangeFunction<size_t>(0, populationSize);

    Population operator()(const Population& population) const override {
        Population newPopulation;

        // Generate
        std::generate_n(std::back_inserter(newPopulation), populationSize, [&]() {
            std::vector<const Chromosome<Fenotype, Eval>*> tournament;

            // Generate new tournanemt
            std::generate_n(std::back_inserter(tournament), tournamentSize,
                [&]() { return &population[pickIndex()]; });

            // And find best
            return **std::max_element(std::cbegin(tournament), std::cend(tournament),
                [](const Chromosome<Fenotype, Eval>* left,
                    const Chromosome<Fenotype, Eval>* right) {
                    return *left < *right;
                });
        });

        // Move new population in place of old one
        return newPopulation;
    }
};

template <class Fenotype, class Eval>
struct GenericCrossoverFunction : public CrossoverFunction<Fenotype, Eval> {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;
    using TypedChromosome = Chromosome<Fenotype, Eval>;

    GenericCrossoverFunction(
        double crossoverProbability,
        std::function<std::tuple<TypedChromosome, TypedChromosome>(
            TypedChromosome, TypedChromosome)>
            crossingFunction)
        : crossoverProbability(crossoverProbability), crossingFunction(crossingFunction) {
    }

    double crossoverProbability;
    std::function<std::tuple<TypedChromosome, TypedChromosome>(TypedChromosome,
        TypedChromosome)>
        crossingFunction;

    // Random generator
    RandomService& service = RandomService::getService();
    std::function<bool(void)> isCrossing = service.getBoolFunction(crossoverProbability);

    void operator()(Population& population) const override {
        // Shuffle
        std::shuffle(std::begin(population), std::end(population),
            service.getEngine());

        // For every pair
        for (size_t i = 0; i < population.size() / 2; i++) {
            // Check if it is crossing
            if (isCrossing()) {
                // Then cross it. Using move because Chromosome objects will no longer
                // be needed
                std::tie(population[i], population[i + 1]) = crossingFunction(
                    std::move(population[i]), std::move(population[i + 1]));
            }
        }
    }
};

template <class Fenotype, class Eval>
struct GenericMutationFunction : public MutationFunction<Fenotype, Eval> {
    using Population = std::vector<Chromosome<Fenotype, Eval>>;
    using TypedChromosome = Chromosome<Fenotype, Eval>;

    GenericMutationFunction(
        double mutationProbability,
        std::function<void(TypedChromosome&)> mutationFunction)
        : mutationProbability(mutationProbability), mutationFunction(mutationFunction) {
    }

    double mutationProbability;
    std::function<void(TypedChromosome&)> mutationFunction;

    // Random generator
    RandomService& service = RandomService::getService();
    std::function<bool(void)> isMutating = service.getBoolFunction(mutationProbability);

    void operator()(Population& population) const override {
        // Find mutating chromosomes
        std::for_each(std::begin(population), std::end(population),
            [&](TypedChromosome& chromosome) {
                if (isMutating()) {
                    mutationFunction(chromosome);
                }
            });
    }
};

#endif // GENERICS_H
