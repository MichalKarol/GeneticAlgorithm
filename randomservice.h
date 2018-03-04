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
#ifndef RANDOMSERVICE_H
#define RANDOMSERVICE_H
#include <functional>
#include <random>

class RandomService {
public:
    static RandomService& getService() {
        static RandomService service;
        return service;
    }
    RandomService(const RandomService&) = delete;
    void operator=(const RandomService&) = delete;
    RandomService(const RandomService&&) = delete;
    void operator=(const RandomService&&) = delete;

    template <class T>
    std::function<T(void)> getRangeFunction(T from, T to) {
        std::uniform_int_distribution<T> range(from, to - 1);
        return std::bind(range, std::ref(engine));
    }

    std::function<bool(void)> getBoolFunction(double probability) {
        std::bernoulli_distribution dist(probability);
        return std::bind(dist, std::ref(engine));
    }

    std::ranlux48& getEngine() { return engine; }

private:
    RandomService() = default;
    std::random_device device;
    std::ranlux48 engine{ device() };
};

#endif // RANDOMSERVICE_H
