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
#include "matrix.h"

Matrix::Matrix(size_t cols, size_t rows)
    : cols(cols), rows(rows) {
    matrix.resize(rows);
    for (auto& row : matrix) {
        row.resize(cols);
    }
}

vector<uint>& Matrix::operator[](size_t index) {
    return matrix[index];
}
const vector<uint>& Matrix::operator[](size_t index) const {
    return matrix[index];
}

istream& operator>>(istream& ios, Matrix& matrix) {
    for (size_t column = 0; column < matrix.cols; column++) {
        for (size_t row = 0; row < matrix.rows; row++) {
            ios >> matrix[column][row];
        }
    }

    return ios;
}
