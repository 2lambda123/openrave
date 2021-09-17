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
#ifndef PIECEWISE_POLY_POLY_CHECKER_H
#define PIECEWISE_POLY_POLY_CHECKER_H

#include "polynomialtrajectory.h"

#define QUINTIC_SMOOTHER_POLY_CHECKING_DEBUG

namespace OpenRAVE {

namespace PiecewisePolynomialsInternal {

enum PolynomialCheckReturn {
    PCR_Normal = 0,
    PCR_PositionLimitsViolation = 1,     ///< local extrema of polynomial in the given range violate position limits
    PCR_VelocityLimitsViolation = 2,     ///< local extrema of polynomial in the given range violate velocity limits
    PCR_AccelerationLimitsViolation = 3, ///< local extrema of polynomial in the given range violate acceleration limits
    PCR_JerkLimitsViolation = 4,         ///< local extrema of polynomial in the given range violate jerk limits
    PCR_NegativeDuration = 5,            ///< the maximum of the range in which polynomial is defined is negative
    PCR_PositionDiscrepancy = 6,         ///< a terminal position (initial/final) is different from the given value
    PCR_VelocityDiscrepancy = 7,         ///< a terminal velocity (initial/final) is different from the given value
    PCR_AccelerationDiscrepancy = 8,     ///< a terminal acceleration (initial/final) is different from the given value
    PCR_DurationDiscrepancy = 9,         ///< polynomial is defined in a range different from the given range
};

class PolynomialChecker {
public:
    PolynomialChecker()
    {
    }
    PolynomialChecker(size_t ndof, int envid);
    ~PolynomialChecker()
    {
    }

    void Initialize(size_t ndof, int envid);

    /// \brief
    PolynomialCheckReturn CheckPolynomial(const Polynomial& p, const dReal T, const dReal xmin, const dReal xmax, const dReal vm=0, const dReal am=0, const dReal jm=0);

    /// \brief
    PolynomialCheckReturn CheckChunk(const Chunk& c, const std::vector<dReal>& xminVect, const std::vector<dReal>& xmaxVect, const std::vector<dReal>& vmVect=std::vector<dReal>(), const std::vector<dReal>& amVect=std::vector<dReal>(), const std::vector<dReal>& jmVect=std::vector<dReal>());

    /// \brief
    PolynomialCheckReturn CheckPiecewisePolynomialTrajectory(const PiecewisePolynomialTrajectory& traj, const std::vector<dReal>& xminVect, const std::vector<dReal>& xmaxVect, const std::vector<dReal>& vmVect=std::vector<dReal>(), const std::vector<dReal>& amVect=std::vector<dReal>(), const std::vector<dReal>& jmVect=std::vector<dReal>());

    //
    // Members
    //
    size_t ndof;
    int envid;

    std::vector<Coordinate> _cacheCoordsVect;

#ifdef QUINTIC_SMOOTHER_POLY_CHECKING_DEBUG
    dReal _failedPoint;
    dReal _failedValue;
#endif

}; // end class PolynomialChecker

} // end namespace PiecewisePolynomialsInternal

} // end namespace OpenRAVE
#endif
