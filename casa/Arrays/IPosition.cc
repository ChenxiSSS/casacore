//# IPosition.cc: A vector of integers, used to index into arrays.
//# Copyright (C) 1994,1995,1996,1997,1998,1999,2000,2001,2002
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include "IPosition.h"
#include "ArrayError.h"
#include "Array.h"

#include <cassert>
#include <sstream>

namespace casacore { //# NAMESPACE CASACORE - BEGIN

IPosition::IPosition (size_t length)
: size_p (length),
  data_p (buffer_p)
{
    if (length > BufferLength) {
	allocateBuffer();
    }
}

IPosition::IPosition(std::initializer_list<ssize_t> list)
: size_p (list.size()),
  data_p (buffer_p)
{
  if (list.size() > BufferLength) {
    allocateBuffer();
  }
  std::initializer_list<ssize_t>::const_iterator list_iter = list.begin();
  size_t i = 0;
  while(list_iter != list.end())
  {
    data_p[i] = *list_iter;
    ++list_iter;
    ++i;
  }
}

IPosition::IPosition (const Array<int> &other)
  : size_p (0),
    data_p (buffer_p)
{
    if (other.size() > 0) {
        if (other.ndim() != 1) {
            throw(ArrayError("IPosition::IPosition(const Array<int> &other) - "
                            "other is not one-dimensional"));
        }
        fill (other.size(), other.begin());
    }
    assert(ok());
}

IPosition::IPosition (const Array<long long> &other)
  : size_p (0),
    data_p (0)
{
    if (other.size() > 0) {
        if (other.ndim() != 1) {
            throw(ArrayError("IPosition::IPosition(const Array<Int64> &other) - "
                            "other is not one-dimensional"));
        }
        fill (other.size(), other.begin());
    }
    assert(ok());
}

IPosition::~IPosition()
{
    if (data_p != &buffer_p[0]) {
        delete [] data_p;
    }
}

void IPosition::allocateBuffer()
{
    if (size_p <= BufferLength) {
        data_p = buffer_p;
    } else {
	data_p = new ssize_t[size_p];
    }
    assert(ok());
}

IPosition::IPosition (size_t length, ssize_t val)
: size_p (length),
  data_p (buffer_p)
{
    if (size_p > BufferLength) {
	allocateBuffer();
    }
    for (size_t i=0; i<size_p; i++) {
	data_p[i] = val;
    }
}

// <thrown>
//    <item> std::runtime_error
// </thrown>
IPosition::IPosition (size_t length, ssize_t val0, ssize_t val1, ssize_t val2,
                      ssize_t val3, ssize_t val4, ssize_t val5, ssize_t val6,
                      ssize_t val7, ssize_t val8, ssize_t val9)
: size_p (length),
  data_p (buffer_p)
{
    if (size_p > BufferLength) {
	allocateBuffer();
    }
    if (size_p > 10  ||  length < 1) {
	throw(std::runtime_error("IPosition::IPosition(size_t length, val0, ...) - "
			 "Can only initialize from 1 to 10 elements"));
    }
    switch (length) {
         case 10: data_p[9] = val9;    // Fall through
	 case 9:  data_p[8] = val8;    // Fall through
	 case 8:  data_p[7] = val7;    // Fall through
	 case 7:  data_p[6] = val6;    // Fall through
	 case 6:  data_p[5] = val5;    // Fall through
	 case 5:  data_p[4] = val4;    // Fall through
	 case 4:  data_p[3] = val3;    // Fall through
	 case 3:  data_p[2] = val2;    // Fall through
	 case 2:  data_p[1] = val1;    // Fall through
	 case 1:  data_p[0] = val0; break;
	 default:
             throw(std::runtime_error("IPosition::IPosition(size_t length, val0, ...) - "
			     "Can only initialize from 1 to 10 elements"));
    }
    for (size_t i=0; i<size_p; i++) {
	if (data_p[i] == MIN_INT) {
	    throw(std::runtime_error("IPosition::IPosition(size_t length, val0, ...) - "
		    "One or more valn == INT_MIN. Probably haven't defined "
		    "enough values. Otherwise specify after construction."));
	}
    }
    assert(ok());
}

IPosition::IPosition (const IPosition& other)
: size_p (other.size_p),
  data_p (buffer_p)
{
    if (size_p > BufferLength) {
	allocateBuffer();
    }
    for (size_t i=0; i<size_p; i++) {
	data_p[i] = other.data_p[i];
    }
    assert(ok());
}

IPosition::IPosition (IPosition&& source) noexcept
: size_p (source.size_p),
  data_p (size_p > BufferLength ? source.data_p : buffer_p)
{
  for(size_t i=0; i!=size_p; ++i)
    data_p[i] = source.data_p[i];
  
  source.size_p = 0;
  source.data_p = source.buffer_p;
}

Vector<int> IPosition::asVector() const
{
    assert(ok());
    Vector<int> retval(nelements());
    copy (retval.begin());
    return retval;
}

Vector<long long> IPosition::asVector64() const
{
    assert(ok());
    Vector<long long> retval(nelements());
    copy (retval.begin());
    return retval;
}

IPosition::IPosition (const std::vector<int> &other)
: size_p (0),
  data_p (buffer_p)
{
    fill (other.size(), other.begin());
    assert(ok());
}

IPosition::IPosition (const std::vector<long long> &other)
  : size_p (0),
    data_p (0)
{
    fill (other.size(), other.begin());
    assert(ok());
}

std::vector<int> IPosition::asStdVector() const
{
    assert(ok());
    std::vector<int> retval(nelements());
    copy (retval.begin());
    return retval;
}

std::vector<long long> IPosition::asStdVector64() const
{
    assert(ok());
    std::vector<long long> retval(nelements());
    copy (retval.begin());
    return retval;
}

IPosition IPosition::nonDegenerate (size_t startingAxis) const
{
    if (startingAxis >= size_p) {
        return *this;
    }
    IPosition ignoreAxes(startingAxis);
    for (size_t i=0; i<startingAxis; i++) {
	ignoreAxes(i) = i;
    }
    return nonDegenerate (ignoreAxes);
}
IPosition IPosition::nonDegenerate (const IPosition& ignoreAxes) const
{
    assert(ok());
    // First determine which axes have to be ignored, thus always be kept.
    // Do not count here, because in theory ignoreAxes can contain the
    // same axis more than once.
    // To remove degenerate axes use two passes - first find out how many axes
    // have to be kept.
    size_t i;
    IPosition keepAxes(size_p, 0);
    for (i=0; i<ignoreAxes.nelements(); i++) {
      if(ignoreAxes(i) >= ssize_t(size_p)) throw std::runtime_error("ignoreAxes(i) >= ssize_t(size_p)");
      keepAxes(ignoreAxes(i)) = 1;
    }
    // Now count all axes to keep.
    size_t count=0;
    for (i=0; i<size_p; i++) {
	if (keepAxes(i) == 1) {
	    count++;
	}else{
	    if (data_p[i] != 1) {
		keepAxes(i) = 1;
		count++;
	    }
	}
    }
    if (count == size_p) return *this;
    if (count == 0) count = 1;
    IPosition nondegenerateIP(count,1);
    count = 0;
    for (i=0; i<size_p; i++) {
	if (keepAxes(i)) {
	    nondegenerateIP(count++) = data_p[i];    
	}
    }
    return nondegenerateIP;
}

void IPosition::resize (size_t newSize, bool copy)
{
    assert(ok());
    // If the size is unchanged, just return (more efficient)
    if (newSize == size_p) {
	return;
    }
    ssize_t* oldData = data_p;
    size_t oldSize = size_p;
    size_p = newSize;
    allocateBuffer();
    if (oldData != data_p  &&  copy) {
	for (size_t i=0; i<std::min(size_p, oldSize); i++) {
	    data_p[i] = oldData[i];
	}
    }
    // Delete the old data, if any.
    if (oldData != &buffer_p[0]) {
	delete [] oldData;
    }
    assert(ok());
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
IPosition& IPosition::operator= (const IPosition& other)
{
  assert(ok());
  if (&other == this) {
    return *this;
  }
  if (size_p != other.size_p) {
    resize (other.nelements(), false);
  }
  for (size_t i=0; i<size_p; i++) {
    data_p[i] = other.data_p[i];
  }
  assert(ok());
  return *this;
}

IPosition& IPosition::operator=(IPosition&& source)
{
  size_p = source.size_p;
  data_p = size_p > BufferLength ? source.data_p : buffer_p;
  for(size_t i=0; i!=size_p; ++i)
    data_p[i] = source.data_p[i];
  
  source.size_p = 0;
  source.data_p = source.buffer_p;
  
  return *this;
}

IPosition& IPosition::operator= (ssize_t value)
{
    assert(ok());
    for (size_t i=0; i<size_p; i++) {
	data_p[i] = value;
    }
    return *this;
}

IPosition IPosition::operator() (const IPosition& axes) const
{
  IPosition ipos(axes.nelements());
  size_t i = 0;
  for (IPosition::const_iterator iter=axes.begin();
       iter!=axes.end(); ++iter, ++i) {
    if (*iter >= int(size_p)) {
      throw std::runtime_error("IPosition::operator()(const IPosition&): "
                      "Axis number must be less than size of current object");
    }
    ipos[i] = data_p[*iter];
  }
  return ipos;
}

void IPosition::append (const IPosition& other)
{
    size_t j = size_p;
    resize (size_p + other.size_p);
    for (size_t i=0; i<other.size_p; i++) {
	data_p[j++] = other.data_p[i];
    }
}

void IPosition::prepend (const IPosition& other)
{
    size_t i;
    size_t j = size_p;
    resize (size_p + other.size_p);
    for (i=size_p; j>0;) {
	data_p[--i] = data_p[--j];
    }
    for (i=0; i<other.size_p; i++) {
	data_p[i] = other.data_p[i];
    }
}

IPosition IPosition::concatenate (const IPosition& other) const
{
    IPosition tmp (*this);
    tmp.append (other);
    return tmp;
}

void IPosition::setFirst (const IPosition& other)
{
    if (size_p < other.size_p) {
	throw (std::runtime_error ("IPosition::setFirst(other); other is too long"));
    }
    for (size_t i=0; i<other.size_p; i++) {
	data_p[i] = other.data_p[i];
    }
}

void IPosition::setLast (const IPosition& other)
{
    if (size_p < other.size_p) {
	throw (std::runtime_error ("IPosition::setLast(other); other is too long"));
    }
    size_t j = size_p - other.size_p;
    for (size_t i=0; i<other.size_p; i++) {
	data_p[j++] = other.data_p[i];
    }
}

IPosition IPosition::getFirst (size_t n) const
{
    if (size_p < n) {
	throw (std::runtime_error ("IPosition::getFirst(n); n is too high"));
    }
    IPosition tmp(n);
    for (size_t i=0; i<n; i++) {
	tmp.data_p[i] = data_p[i];
    }
    return tmp;
}

IPosition IPosition::getLast (size_t n) const
{
    if (size_p < n) {
	throw (std::runtime_error ("IPosition::getLast(n); n is too high"));
    }
    IPosition tmp(n);
    size_t j = size_p - n;
    for (size_t i=0; i<n; i++) {
	tmp.data_p[i] = data_p[j++];
    }
    return tmp;
}

IPosition IPosition::removeAxes (const IPosition& axes) const
{
  // Get the axes to keep.
  // It also checks if axes are specified correctly.
  IPosition resAxes = IPosition::otherAxes (size_p, axes);
  size_t ndimRes = resAxes.nelements();
  // Create the result shape.
  IPosition resShape(ndimRes);
  if (ndimRes == 0) {
    resShape.resize(1);
    resShape[0] = 1;
  } else {
    for (size_t i=0; i<ndimRes; ++i) {
      resShape[i] = data_p[resAxes[i]];
    }
  }
  return resShape;
}

IPosition IPosition::keepAxes (const IPosition& axes) const
{
  return removeAxes (otherAxes(size_p, axes));
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
void IPosition::operator += (const IPosition& other)
{
    assert(ok());
    if (! conform(other)) {
	throw(ArrayConformanceError("IPosition::operator += "
				    "(const IPosition&) - "
				    "this and other differ in length"));
    }
    for (size_t i=0; i<size_p; i++) {
	data_p[i] += other.data_p[i];
    }
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
void IPosition::operator -= (const IPosition& other)
{
    assert(ok());
    if (! conform(other)) {
	throw(ArrayConformanceError("IPosition::operator -= "
				    "(const IPosition&) - "
				    "this and other differ in length"));
    }
    for (size_t i=0; i<size_p; i++) {
	data_p[i] -= other.data_p[i];
    }
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
void IPosition::operator *= (const IPosition& other)
{
    assert(ok());
    if (! conform(other)) {
	throw(ArrayConformanceError("IPosition::operator *= "
				    "(const IPosition&) - "
				    "this and other differ in length"));
    }
    for (size_t i=0; i<size_p; i++) {
	data_p[i] *= other.data_p[i];
    }
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
void IPosition::operator /= (const IPosition& other)
{
    assert(ok());
    if (! conform(other)) {
	throw(ArrayConformanceError("IPosition::operator /= "
				    "(const IPosition&) - "
				    "this and other differ in length"));
    }
    for (size_t i=0; i<size_p; i++) {
	data_p[i] /= other.data_p[i];
    }
}

void IPosition::operator += (ssize_t val)
{
    assert(ok());
    for (size_t i=0; i<size_p; i++) {
	data_p[i] += val;
    }
}

void IPosition::operator -= (ssize_t val)
{
    assert(ok());
    for (size_t i=0; i<size_p; i++) {
	data_p[i] -= val;
    }
}

void IPosition::operator *= (ssize_t val)
{
    assert(ok());
    for (size_t i=0; i<size_p; i++) {
	data_p[i] *= val;
    }
}

void IPosition::operator /= (ssize_t val)
{
    assert(ok());
    for (size_t i=0; i<size_p; i++) {
	data_p[i] /= val;
    }
}

bool IPosition::isEqual (const IPosition& other) const
{
    return isEqual (other, nelements());
}

bool IPosition::isEqual (const IPosition& other,
			 bool skipDegeneratedAxes) const
{
    if (!skipDegeneratedAxes) {
	return isEqual (other, nelements());
    }
    size_t nrthis = nelements();
    size_t nrthat = other.nelements();
    size_t i;
    size_t j=0;
    for (i=0; i<nrthis; i++) {
	if (data_p[i] != 1) {
	    while (j < nrthat  &&  other(j) == 1) {
		j++;
	    }
	    if (j >= nrthat) {
		break;
	    }
	    if (data_p[i] != other(j)) {
		return false;
	    }
	    j++;
	}
    }
    for (; i<nrthis; i++) {
	if (data_p[i] != 1) {
	    return false;
	}
    }
    for (; j<nrthat; j++) {
	if (other(j) != 1) {
	    return false;
	}
    }
    return true;
}

bool IPosition::isEqual (const IPosition& other, size_t nrCompare) const
{
    if (! conform (other)) {
	return false;
    }
    if (nrCompare > nelements()) {
	nrCompare = nelements();
    }
    for (size_t i=0; i<nrCompare; i++) {
	if (data_p[i] != other(i)) {
	    return false;
	}
    }
    return true;
}


bool IPosition::isSubSet (const IPosition& other) const
{
    size_t nrthis = nelements();
    size_t nrthat = other.nelements();
    size_t j=0;
    for (size_t i=0; i<nrthis; i++) {
      if (j < nrthat) {
	if (other(j) == data_p[i]  ||  other(j) == 1) {
	  j++;
	}
      }
    }
    return (j == nrthat);
}


// <thrown>
//    <item> ArrayConformanceError
// </thrown>
IPosition operator + (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator + "
			"(const IPosition&, const IPosition&) - "
			"left and right operand do not conform "));
    }
    IPosition result(left);
    result += right;
    return result;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
IPosition operator - (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator - "
			"(const IPosition&, const IPosition&) - "
			"left and right operand do not conform "));
    }
    IPosition result(left);
    result -= right;
    return result;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
IPosition operator * (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator * "
			"(const IPosition&, const IPosition&) - "
			"left and right operand do not conform "));
    }
    IPosition result(left);
    result *= right;
    return result;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
IPosition operator / (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator / "
			"(const IPosition&, const IPosition&) - "
			"left and right operand do not conform "));
    }
    IPosition result(left);
    result /= right;
    return result;
}

IPosition operator + (const IPosition& left, ssize_t val)
{
    IPosition result(left);
    result += val;
    return result;
}

IPosition operator - (const IPosition& left, ssize_t val)
{
    IPosition result(left);
    result -= val;
    return result;
}

IPosition operator * (const IPosition& left, ssize_t val)
{
    IPosition result(left);
    result *= val;
    return result;
}

IPosition operator / (const IPosition& left, ssize_t val)
{
    IPosition result(left);
    result /= val;
    return result;
}

IPosition operator + (ssize_t val, const IPosition& right)
{
    IPosition result(right.nelements());
    result = val;
    result += right;
    return result;
}

IPosition operator - (ssize_t val, const IPosition& right)
{
    IPosition result(right.nelements());
    result = val;
    result -= right;
    return result;
}

IPosition operator * (ssize_t val, const IPosition& right)
{
    IPosition result(right.nelements());
    result = val;
    result *= right;
    return result;
}

IPosition operator / (ssize_t val, const IPosition& right)
{
    IPosition result(right.nelements());
    result = val;
    result /= right;
    return result;
}

IPosition max (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
        throw(ArrayConformanceError("::max "
                        "(const IPosition&, const IPosition&) - "
                        "left and right operand do not conform "));
    }
    IPosition result(left);
    const size_t ndim = result.nelements();
    ssize_t max;
    for (size_t i = 0; i < ndim; i++) {
      if (result(i) < (max = right(i))) {
        result(i) = max;
      }
    }
    return result;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
IPosition min (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
        throw(ArrayConformanceError("::min "
                        "(const IPosition&, const IPosition&) - "
                        "left and right operand do not conform "));
    }
    IPosition result(left);
    const size_t ndim = result.nelements();
    ssize_t min;
    for (size_t i = 0; i < ndim; i++) {
      if (result(i) > (min = right(i))) {
        result(i) = min;
      }
    }
    return result;
}

long long IPosition::product() const
{
    if (nelements() ==  0) {
	return 0;
    }
    long long total = 1;
    for (size_t i=0; i<nelements(); i++) {
	total *= data_p[i];
    }
    return total;
}

bool IPosition::allOne() const
{
    for (size_t i=0; i<nelements(); ++i) {
        if (data_p[i] != 1) {
            return false;
        }
    }
    return true;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
bool operator == (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator== "
				    "(const IPosition&, const IPosition&) - "
				    "left and right operand do not conform "));
    }
    size_t n=left.nelements();
    bool result = true;
    for (size_t i=0; i<n; i++) {
	if (left(i) == right(i)) {
	    // Nothing - written to make cut and paste easier
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
bool operator != (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator!= "
				    "(const IPosition&, const IPosition&) - "
				    "left and right operand do not conform (left.shape()=" +
            to_string(left) + ", right.shape()=" + to_string(right) + ")"));
    }
    size_t n=left.nelements();
    bool result = false;
    for (size_t i=0; i<n; i++) {
	if (left(i) != right(i)) {
	    result = true;
	    break;
	} else {
	    // Nothing - written to make cut and paste easier
	}
    }
    return result;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
bool operator < (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator< "
			"(const IPosition&, const IPosition&) - "
			"left and right operand do not conform "));
    }
    size_t n=left.nelements();
    bool result = true;
    for (size_t i=0; i<n; i++) {
	if (left(i) < right(i)) {
	    // Nothing - written to make cut and paste easier
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
bool operator <= (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator<= "
				    "(const IPosition&, const IPosition&) - "
				    "left and right operand do not conform "));
    }
    size_t n=left.nelements();
    bool result = true;
    for (size_t i=0; i<n; i++) {
	if (left(i) <= right(i)) {
	    // Nothing - written to make cut and paste easier
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
bool operator > (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator> "
				    "(const IPosition&, const IPosition&) - "
				    "left and right operand do not conform "));
    }
    size_t n=left.nelements();
    bool result = true;
    for (size_t i=0; i<n; i++) {
	if (left(i) > right(i)) {
	    // Nothing - written to make cut and paste easier
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

// <thrown>
//    <item> ArrayConformanceError
// </thrown>
bool operator >= (const IPosition& left, const IPosition& right)
{
    if (! left.conform(right)) {
	throw(ArrayConformanceError("::operator>= "
				    "(const IPosition&, const IPosition&) - "
				    "left and right operand do not conform "));
    }
    size_t n=left.nelements();
    bool result = true;
    for (size_t i=0; i<n; i++) {
	if (left(i) >= right(i)) {
	    // Nothing - written to make cut and paste easier
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator == (const IPosition& left, ssize_t val)
{
    bool result = true;
    size_t n = left.nelements();
    for (size_t i=0; i<n; i++) {
	if (left(i) == val) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator != (const IPosition& left, ssize_t val)
{
    bool result = false;
    size_t n = left.nelements();
    for (size_t i=0; i<n; i++) {
	if (left(i) != val) {
	    result = true;
	    break;
	} else {
	    // Nothing
	}
    }
    return result;
}

bool operator < (const IPosition& left, ssize_t val)
{
    bool result = true;
    size_t n = left.nelements();
    for (size_t i=0; i<n; i++) {
	if (left(i) < val) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator <= (const IPosition& left, ssize_t val)
{
    bool result = true;
    size_t n = left.nelements();
    for (size_t i=0; i<n; i++) {
	if (left(i) <= val) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator > (const IPosition& left, ssize_t val)
{
    bool result = true;
    size_t n = left.nelements();
    for (size_t i=0; i<n; i++) {
	if (left(i) > val) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator >= (const IPosition& left, ssize_t val)
{
    bool result = true;
    size_t n = left.nelements();
    for (size_t i=0; i<n; i++) {
	if (left(i) >= val) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator == (ssize_t val, const IPosition& right)
{
    bool result = true;
    size_t n = right.nelements();
    for (size_t i=0; i<n; i++) {
	if (val == right(i)) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator != (ssize_t val, const IPosition& right)
{
    bool result = false;
    size_t n = right.nelements();
    for (size_t i=0; i<n; i++) {
	if (val != right(i)) {
	    result = true;
	    break;
	} else {
	    // Nothing
	}
    }
    return result;
}

bool operator < (ssize_t val, const IPosition& right)
{
    bool result = true;
    size_t n = right.nelements();
    for (size_t i=0; i<n; i++) {
	if (val < right(i)) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator <= (ssize_t val, const IPosition& right)
{
    bool result = true;
    size_t n = right.nelements();
    for (size_t i=0; i<n; i++) {
	if (val <= right(i)) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator > (ssize_t val, const IPosition& right)
{
    bool result = true;
    size_t n = right.nelements();
    for (size_t i=0; i<n; i++) {
	if (val > right(i)) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

bool operator >= (ssize_t val, const IPosition& right)
{
    bool result = true;
    size_t n = right.nelements();
    for (size_t i=0; i<n; i++) {
	if (val >= right(i)) {
	    // Nothing
	} else {
	    result = false;
	    break;
	}
    }
    return result;
}

std::string to_string(const IPosition& ip)
{
  std::ostringstream oss;
  oss << ip;
  return oss.str();
}

std::string IPosition::toString() const
{
  return to_string(*this);
}

std::ostream& operator<< (std::ostream& os, const IPosition& ip)
{
    os << "[";
    for (size_t i=0; i<ip.nelements(); i++) {
	if (i > 0) {
	    os << ", ";
	}
	os << ip(i);
    }
    os << "]";
    return os;
}

bool IPosition::ok() const
{
  assert(size_p > BufferLength || data_p == &buffer_p[0]);
  assert(data_p != nullptr);
  bool retval = true;
  if (size_p <= BufferLength && data_p != &buffer_p[0]) { retval = false; }
  if (data_p == nullptr) { retval = false; }
  return retval;
}


IPosition toIPositionInArray (long long offset, const IPosition& shape)
{
    if (! isInsideArray (offset, shape) ) {
	throw (ArrayIndexError(
            "IPosition ::toIPositionInArray (long long offset,"
            " const IPosition& shape)"
             " - Invalid offset."));
    }

    IPosition iposition (shape.nelements());
    long long divisor = 1;

    size_t ndim = shape.nelements();
    for (size_t idim = 0; idim < ndim; idim++) {
        iposition(idim) = ((offset / divisor) % shape(idim));
        divisor *= shape(idim);
    }

    return iposition;

}

long long toOffsetInArray (const IPosition& iposition, const IPosition& shape)
{
    if (! (iposition.conform(shape)) ) {
	throw (ArrayConformanceError(
            "long long ::toOffsetInArray (const IPosition& iposition,"
            " const IPosition& shape)"
             " - IPositions do not conform"));
    }

    if (! isInsideArray (iposition, shape) ) {
	throw (ArrayIndexError(
            "long long ::toOffsetInArray (const IPosition& iposition,"
            " const IPosition& shape)"
             " - Invalid iposition."));
    }

    long long offset = 0;
    long long multiplier = 1;

    size_t ndim = shape.nelements();
    for (size_t idim = 0; idim < ndim; idim++) {
        offset += (iposition(idim) * multiplier);
        multiplier *= shape(idim);
    }

    return offset;
}


bool isInsideArray (long long offset, const IPosition& shape)
{
    return (offset < shape.product()) ? true : false;
}


bool isInsideArray (const IPosition& iposition, const IPosition& shape)
{
    if (! (iposition.conform(shape)) ) {
	throw (ArrayConformanceError(
            "bool ::isInsideArray (const IPosition& iposition,"
            " const IPosition& shape)"
             " - IPositions do not conform"));
    }

    bool result = true;
    ssize_t ioff;

    size_t ndim = shape.nelements();
    for (size_t idim = 0; idim < ndim; idim++) {
        ioff = iposition(idim);
        if ( (ioff < 0) || (ioff >= shape(idim)) ) {
            result = false;
            break;
        }
    }

    return result;

}


IPosition IPosition::makeAxisPath (size_t nrdim, const IPosition& partialPath)
{
    // Check if the specified traversal axes are correct and unique.
    if (partialPath.nelements() > nrdim)  std::runtime_error("partialPath.nelements() > nrdim");
    IPosition path(nrdim);
    IPosition done(nrdim, 0);
    size_t i,j;
    for (i=0; i<partialPath.nelements(); i++) {
        path(i) = partialPath(i);
        if (path(i) >= int(nrdim)  ||  done(path(i)) != 0) {
            throw (std::runtime_error ("IPosition::makeAxisPath: invalid defined axes"));
        }
        done(path(i)) = 1;
    }
    // Fill unspecified axes with the natural order.
    for (j=0; j<nrdim; j++) {
        if (done(j) == 0) {
            path(i++) = j;
        }
    }
    return path;
}

IPosition IPosition::otherAxes (size_t nrdim, const IPosition& axes)
{
   if (nrdim<axes.nelements()) throw std::runtime_error("nrdim<axes.nelements()");
   return makeAxisPath(nrdim, axes).getLast(nrdim-axes.nelements());
}

void IPosition::throwIndexError() const
{
    // This should be an IndexError<size_t> - but that causes multiply
    // defined symbols with the current objectcenter.
    throw(std::runtime_error("IPosition::operator() - index error"));
}

bool IPositionComparator::operator ()(const IPosition& lhs, const IPosition& rhs) const {
    size_t lhsSize = lhs.size_p;
    size_t rhsSize = rhs.size_p;
    if (lhsSize == rhsSize) {
        ssize_t *lp = lhs.data_p;
        ssize_t *rp = rhs.data_p;
        for (size_t i=0; i<lhsSize; ++i, ++lp, ++rp) {
            if (*lp != *rp) {
                return *lp < *rp;
            }
        }
    }
    else {
        return lhsSize < rhsSize;
    }
    // same size and all elements equal, return false
    return false;
}

} //# NAMESPACE CASACORE - END

