// -*- coding: utf-8 -*-
// Copyright (C) 2019 Puttichai Lertkultanon
//
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU Lesser General Public License as published by the Free Software Foundation, either version 3
// of the License, or at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
// even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.
#ifndef PIECEWISE_POLY_TRAJECTORY_H
#define PIECEWISE_POLY_TRAJECTORY_H

#include <vector>
#include <openrave/openrave.h>
#include "polynomialcommon.h"

namespace OpenRAVE {

namespace PiecewisePolynomialsInternal {

class Coordinate {
public:
    Coordinate()
    {
    }
    Coordinate(dReal p, dReal v) : point(p), value(v)
    {
    }
    ~Coordinate()
    {
    }
    bool operator<(const Coordinate& rhs) const
    {
        return point < rhs.point;
    }
    dReal point;
    dReal value;
}; // end class Coordinate

class Polynomial {
public:
    /*
       \params vcoeffs vector of polynomial coefficients, starting from the weakest term. That is,
               this polynomial p is described by p(t) = c[0] + c[1]*t + c[2]*t**2 + ... + c[n]*t**n,
               where n is the degree of this polynomial
     */
    Polynomial()
    {
    };
    Polynomial(const std::vector<dReal>& c);
    ~Polynomial()
    {
    }

    //
    // Functions
    //
    /// \brief Initialize this polynomial with the given coefficients.
    void Initialize(const std::vector<dReal>& c);

    /// \brief Initialize this polynomial with the existing coefficients
    void Initialize();

    /// \brief Append zeros to the coefficient vectors.
    void PadCoefficients(size_t newdegree);

    /// \brief Update the weakest term coefficient.
    void UpdateInitialValue(dReal c0);

    /// \brief Evaluate this polynomial at time t.
    dReal Eval(dReal t) const;

    /// \brief Evaluate the first derivative of this polynomial at time t.
    dReal Evald1(dReal t) const;

    /// \brief Evaluate the second derivative of this polynomial at time t.
    dReal Evald2(dReal t) const;

    /// \brief Evaluate the third derivative of this polynomial at time t.
    dReal Evald3(dReal t) const;

    /// \brief Evaluate the n-th derivative of this polynomial at time t.
    dReal Evaldn(dReal t, size_t n) const;

    /// \brief Return the polynomial d/dt p(t)
    Polynomial Differentiate() const;

    // /// \brief Return the polynomial int p(t) dt
    // Polynomial Integrate();

    /// \brief Return
    inline const std::vector<Coordinate>& GetExtrema() const
    {
        return vcextrema;
    }

    /// \brief
    void Serialize(std::ostream& O) const;

    /// \brief
    void Deserialize(std::istream& I);

    /// \brief Find all local extrema of this polynomial. Keep the results in vcextrema in ascending
    /// order.
    void _FindAllLocalExtrema();

    /// \brief Find all local extrema of the ideriv-th derivative of this polynomial. Keep the
    /// results in vcoord in ascending order.
    void FindAllLocalExtrema(size_t ideriv, std::vector<Coordinate>& vcoord) const;

    //
    // Members
    //
    size_t degree; ///< the degree of this polynomial
    std::vector<dReal> vcoeffs; ///< vector of coefficients of this polynomial (weakest term first)
    std::vector<dReal> vcoeffsd; ///< vector of coefficients of the first derivative of this polynomial
    std::vector<dReal> vcoeffsdd; ///< vector of coefficients of the second derivative of this polynomial
    std::vector<dReal> vcoeffsddd; ///< vector of coefficients of the third derivative of this polynomial
    std::vector<Coordinate> vcextrema; ///< vector of pairs (t, p(t)) where each t is such that p(t) is a local extrema

    mutable std::vector<dReal> _vcurcoeffs;
}; // end class Polynomial

class Chunk {
public:
    /*
       Chunk is a vertical stack of polynomials. Each polynomial is parameterized by t \in [0, duration].

       \params duration the duration of this chunk
       \params vpolynomials vector of polynomials
     */
    Chunk() : duration(0), constraintChecked(false)
    {
    }
    Chunk(const dReal duration, const std::vector<Polynomial>& vpolynomials);
    ~Chunk()
    {
    }

    //
    // Functions
    //
    /// \brief
    void UpdateInitialValues(std::vector<dReal>& vinitialvalues);

    /// \brief Initialize this chunk with the given duration and polynomials vector
    void Initialize(const dReal duration, const std::vector<Polynomial>& vpolynomials);

    /// \brief Initialize this chunk with the existing duration and polynomials vector
    void Initialize();

    /// \brief Cut this chunk into two halves. The left half (from t = 0 to t = t) is stored in
    /// this. The right half is returned via remChunk.
    void Cut(dReal t, Chunk& remChunk);

    /// \brief Evaluate all polynomials at time t.
    void Eval(dReal t, std::vector<dReal>& res) const;

    /// \brief Evaluate the first derivatives of all polynomials at time t.
    void Evald1(dReal t, std::vector<dReal>& res) const;

    /// \brief Evaluate the second derivatives of all polynomials at time t.
    void Evald2(dReal t, std::vector<dReal>& res) const;

    /// \brief Evaluate the third derivatives of all polynomials at time t.
    void Evald3(dReal t, std::vector<dReal>& res) const;

    /// \brief Evaluate the n-th derivatives of all polynomials at time t.
    void Evaldn(dReal t, size_t n, std::vector<dReal>& res) const;

    /// \brief
    void Serialize(std::ostream& O) const;

    /// \brief
    void Deserialize(std::istream& I);

    /// \brief Initialize this chunk with constant polynomials.
    void SetConstant(const std::vector<dReal>& x0Vect, const dReal duration, const size_t degree);

    //
    // Members
    //
    size_t degree;
    size_t dof;
    dReal duration;
    std::vector<Polynomial> vpolynomials;

    mutable bool constraintChecked; ///< TODO: write a description for this parameter (similar to that of RampND)

}; // end class Chunk

class PiecewisePolynomialTrajectory {
public:
    /*
       PiecewisePolynomialTrajectory is a horizontal stack of chunks.

       \params vchunks vector of chunks
     */
    PiecewisePolynomialTrajectory()
    {
    }
    PiecewisePolynomialTrajectory(const std::vector<Chunk>& vchunks);
    ~PiecewisePolynomialTrajectory()
    {
    }

    //
    // Functions
    //
    /// \brief Evaluate this trajectory at time t.
    void Eval(dReal t, std::vector<dReal>& res) const;

    /// \brief Evaluate the first derivative of this trajectory at time t.
    void Evald1(dReal t, std::vector<dReal>& res) const;

    /// \brief Evaluate the second derivative of this trajectory at time t.
    void Evald2(dReal t, std::vector<dReal>& res) const;

    /// \brief Evaluate the third derivative of this trajectory at time t.
    void Evald3(dReal t, std::vector<dReal>& res) const;

    /// \brief Evaluate the n-th derivative of this trajectory at time t.
    void Evaldn(dReal t, size_t n, std::vector<dReal>& res) const;

    /// \brief Find the index of the chunk in which the given time t falls into. Also compute the remainder of that chunk.
    void FindChunkIndex(dReal t, size_t& index, dReal& remainder) const;

    /// \brief Initialize this trajectory with the given chunks
    void Initialize(const std::vector<Chunk>& vchunks);

    /// \brief Initializee this trajectory with the existing chunks
    void Initialize();

    /// \brief
    void _UpdateChunksVector();

    /// \brief
    void Serialize(std::ostream& O) const;

    /// \brief
    void Deserialize(std::istream& I);

    /// \brief
    void ReplaceSegment(dReal t0, dReal t1, const std::vector<Chunk>& vchunks);

    /// \brief
    inline void Reset()
    {
        degree = 0;
        dof = 0;
        duration = 0;
        vswitchtimes.resize(0);
        vchunks.resize(0);
    }

    //
    // Members
    //
    size_t degree;
    size_t dof;
    dReal duration;
    std::vector<dReal> vswitchtimes; ///< vector of time instants where at least one dof changes its control value. The control value is p^(degree)(t)/degree!. For example, in case degree = 2, the control value is p''(t)/2, which is the acceleration.
    std::vector<Chunk> vchunks;

}; // end class PiecewisePolynomialTrajectory

} // end namespace PiecewisePolynomialsInternal

} // end namespace OpenRAVE
#endif
