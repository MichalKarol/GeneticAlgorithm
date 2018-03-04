﻿//    Copyright (C) 2018 Michał Karol <michal.p.karol@gmail.com>

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
#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <iostream>
#include <vector>

using std::istream;
using std::vector;
typedef unsigned int uint;

class Matrix {
public:
    Matrix(size_t cols, size_t rows);

    vector<uint>& operator[](size_t index);
    const vector<uint>& operator[](size_t index) const;

    const size_t cols = 0;
    const size_t rows = 0;

private:
    vector<vector<uint>> matrix;
};

istream& operator>>(istream& is, Matrix& matrix);

#endif // MATRIX_H
