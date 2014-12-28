// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

/*
This class lets you easily create resizable 2D arrays.
It uses a single vector, which is more efficient than having a vector of vectors, because memory is only allocated once.
    Resizing is still not very efficient, so don't use this if you need to do that a lot.
The size can be changed dynamically on runtime in a non-destructive way.
    Note: All of the elements are copied into a new matrix, so try not to do this often.
*/
template <class Type>
class Matrix
{
    public:
        Matrix()
        {
            clear();
        }

        Matrix(unsigned width, unsigned height)
        {
            resize(width, height);
        }

        void resize(unsigned width, unsigned height, bool preserve = true)
        {
            // Only resize if the new size is different
            if (matrixWidth != width || matrixHeight != height)
            {
                unsigned newMatrixSize = width * height;
                if (!elements.empty() && preserve)
                {
                    // Preserve the original data by copying it
                    std::vector<Type> tempElements(newMatrixSize);
                    copyMatrix(elements, tempElements, matrixWidth, width, matrixHeight, height);
                    elements.swap(tempElements);
                }
                else
                {
                    elements.clear();
                    elements.resize(newMatrixSize);
                }
                matrixWidth = width;
                matrixHeight = height;
                matrixSize = newMatrixSize;
            }
        }

        // Removes all of the elements, so the size is 0x0
        void clear()
        {
            matrixWidth = 0;
            matrixHeight = 0;
            matrixSize = 0;
            elements.clear();
            elements.shrink_to_fit();
        }

        // You can get/set elements with these functions
        // Note that they are (x, y), which is (column, row)
        Type& operator()(unsigned x, unsigned y)
        {
            return elements[(y * matrixWidth) + x];
        }

        unsigned width() const
        {
            return matrixWidth;
        }

        unsigned height() const
        {
            return matrixHeight;
        }

        unsigned size() const
        {
            return matrixSize;
        }

        using MatrixType = std::vector<Type>;

        typename MatrixType::iterator begin()
        {
            return elements.begin();
        }

        typename MatrixType::iterator end()
        {
            return elements.end();
        }

    private:
        void copyMatrix(const std::vector<Type>& source, std::vector<Type>& dest, unsigned sourceWidth, unsigned destWidth, unsigned sourceHeight, unsigned destHeight) const
        {
            unsigned height = std::min(sourceHeight, destHeight);
            unsigned width = std::min(sourceWidth, destWidth);
            // Fill data
            dest.clear();
            dest.resize(destWidth * destHeight);
            // Copy data
            for (unsigned y = 0; y < height; ++y)
                for (unsigned x = 0; x < width; ++x)
                    dest[(y * destWidth) + x] = source[(y * sourceWidth) + x];
        }

        MatrixType elements;
        unsigned matrixWidth;
        unsigned matrixHeight;
        unsigned matrixSize;
};

#endif
