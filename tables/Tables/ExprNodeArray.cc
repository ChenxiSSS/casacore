//# ExprNodeArray.cc: Classes representing an array in table select expression
//# Copyright (C) 1997,1999,2000,2001
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
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <tables/Tables/ExprNode.h>
#include <tables/Tables/ExprNodeArray.h>
#include <tables/Tables/ExprNodeSet.h>
#include <tables/Tables/ExprDerNode.h>
#include <tables/Tables/TableError.h>
#include <casa/Arrays/MArrayMath.h>
#include <casa/Arrays/MArrayLogical.h>
#include <casa/Arrays/Slicer.h>
#include <casa/Utilities/Assert.h>
#include <casa/Exceptions/Error.h>


namespace casa { //# NAMESPACE CASA - BEGIN

TableExprNodeArray::TableExprNodeArray (NodeDataType dtype, OperType otype)
: TableExprNodeBinary (dtype, VTArray, otype, Table())
{
    ndim_p = -1;
}
TableExprNodeArray::TableExprNodeArray (const TableExprNodeRep& node,
					NodeDataType dtype, OperType otype)
: TableExprNodeBinary (dtype, node, otype)
{}
TableExprNodeArray::TableExprNodeArray (NodeDataType dtype, OperType otype,
					const IPosition& shape)
: TableExprNodeBinary (dtype, VTArray, otype, Table())
{
    shape_p = shape;
    ndim_p  = shape.nelements();
    if (ndim_p == 0) {
	ndim_p = -1;
    }
}

TableExprNodeArray::~TableExprNodeArray()
{}

const IPosition& TableExprNodeArray::getShape (const TableExprId& id)
{
    varShape_p.resize (0);
    switch (dataType()) {
    case NTBool:
	varShape_p = getArrayBool(id).shape();
	break;
    case NTInt:
	varShape_p = getArrayInt(id).shape();
	break;
    case NTDouble:
	varShape_p = getArrayDouble(id).shape();
	break;
    case NTComplex:
	varShape_p = getArrayDComplex(id).shape();
	break;
    case NTString:
	varShape_p = getArrayString(id).shape();
	break;
    case NTDate:
	varShape_p = getArrayDate(id).shape();
	break;
    default:
	TableExprNode::throwInvDT ("TableExprNodeArray::getShape");
    }
    return varShape_p;
}

MArray<DComplex> TableExprNodeArray::getArrayDComplex (const TableExprId& id)
{
    MArray<Double> arr = getArrayDouble (id);
    Array<DComplex> result (arr.shape());
    convertArray (result, arr.array());
    return MArray<DComplex> (result, arr.mask());
}

Bool TableExprNodeArray::hasBool     (const TableExprId& id, Bool value)
{
    return anyEQ (value, getArrayBool (id));
}
Bool TableExprNodeArray::hasInt      (const TableExprId& id, Int64 value)
{
    return anyEQ (value, getArrayInt (id));
}
Bool TableExprNodeArray::hasDouble   (const TableExprId& id, Double value)
{
    return anyEQ (value, getArrayDouble (id));
}
Bool TableExprNodeArray::hasDComplex (const TableExprId& id,
				      const DComplex& value)
{
    return anyEQ (value, getArrayDComplex (id));
}
Bool TableExprNodeArray::hasString   (const TableExprId& id,
				      const String& value)
{
    return anyEQ (value, getArrayString (id));
}
Bool TableExprNodeArray::hasDate     (const TableExprId& id,
				      const MVTime& value)
{
    return anyEQ (value, getArrayDate (id));
}

MArray<Bool> TableExprNodeArray::hasArrayBool (const TableExprId& id,
                                               const MArray<Bool>& value)
{
    MArray<Bool> set = getArrayBool (id);
    Array<Bool> result(value.shape());
    Bool deleteIn, deleteOut;
    const Bool* in = value.array().getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    for (uInt i=0; i<nval; i++) {
	out[i] = anyEQ (in[i], set);
    }
    value.array().freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return MArray<Bool> (result, value.mask());
}
MArray<Bool> TableExprNodeArray::hasArrayInt (const TableExprId& id,
                                              const MArray<Int64>& value)
{
    MArray<Int64> set = getArrayInt (id);
    Array<Bool> result(value.shape());
    Bool deleteIn, deleteOut;
    const Int64* in = value.array().getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    for (uInt i=0; i<nval; i++) {
	out[i] = anyEQ (in[i], set);
    }
    value.array().freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return MArray<Bool> (result, value.mask());
}
MArray<Bool> TableExprNodeArray::hasArrayDouble (const TableExprId& id,
                                                 const MArray<Double>& value)
{
    MArray<Double> set = getArrayDouble (id);
    Array<Bool> result(value.shape());
    Bool deleteIn, deleteOut;
    const Double* in = value.array().getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    for (uInt i=0; i<nval; i++) {
	out[i] = anyEQ (in[i], set);
    }
    value.array().freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return MArray<Bool> (result, value.mask());
}
MArray<Bool> TableExprNodeArray::hasArrayDComplex (const TableExprId& id,
                                                   const MArray<DComplex>& value)
{
    MArray<DComplex> set = getArrayDComplex (id);
    Array<Bool> result(value.shape());
    Bool deleteIn, deleteOut;
    const DComplex* in = value.array().getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    for (uInt i=0; i<nval; i++) {
	out[i] = anyEQ (in[i], set);
    }
    value.array().freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return MArray<Bool> (result, value.mask());
}
MArray<Bool> TableExprNodeArray::hasArrayString (const TableExprId& id,
                                                 const MArray<String>& value)
{
    MArray<String> set = getArrayString (id);
    Array<Bool> result(value.shape());
    Bool deleteIn, deleteOut;
    const String* in = value.array().getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    for (uInt i=0; i<nval; i++) {
	out[i] = anyEQ (in[i], set);
    }
    value.array().freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return MArray<Bool> (result, value.mask());
}
MArray<Bool> TableExprNodeArray::hasArrayDate (const TableExprId& id,
                                               const MArray<MVTime>& value)
{
    MArray<MVTime> set = getArrayDate (id);
    Array<Bool> result(value.shape());
    Bool deleteIn, deleteOut;
    const MVTime* in = value.array().getStorage (deleteIn);
    Bool* out = result.getStorage (deleteOut);
    uInt nval = value.nelements();
    for (uInt i=0; i<nval; i++) {
	out[i] = anyEQ (in[i], set);
    }
    value.array().freeStorage (in, deleteIn);
    result.putStorage (out, deleteOut);
    return MArray<Bool> (result, value.mask());
}

Bool TableExprNodeArray::getElemBool (const TableExprId& id,
				      const Slicer& slicer)
{
    MArray<Bool> arr = getArrayBool (id);
    arr.array().validateIndex (slicer.start());
    return arr.array()(slicer.start());
}
Int64 TableExprNodeArray::getElemInt (const TableExprId& id,
                                      const Slicer& slicer)
{
    MArray<Int64> arr = getArrayInt (id);
    arr.array().validateIndex (slicer.start());
    return arr.array()(slicer.start());
}
Double TableExprNodeArray::getElemDouble (const TableExprId& id,
					  const Slicer& slicer)
{
    MArray<Double> arr = getArrayDouble (id);
    arr.array().validateIndex (slicer.start());
    return arr.array()(slicer.start());
}
DComplex TableExprNodeArray::getElemDComplex (const TableExprId& id,
					      const Slicer& slicer)
{
    MArray<DComplex> arr = getArrayDComplex (id);
    arr.array().validateIndex (slicer.start());
    return arr.array()(slicer.start());
}
String TableExprNodeArray::getElemString (const TableExprId& id,
					  const Slicer& slicer)
{
    MArray<String> arr = getArrayString (id);
    arr.array().validateIndex (slicer.start());
    return arr.array()(slicer.start());
}
MVTime TableExprNodeArray::getElemDate (const TableExprId& id,
					const Slicer& slicer)
{
    MArray<MVTime> arr = getArrayDate (id);
    arr.array().validateIndex (slicer.start());
    return arr.array()(slicer.start());
}

MArray<Bool> TableExprNodeArray::getSliceBool (const TableExprId& id,
                                               const Slicer& slicer)
{
    MArray<Bool> arr = getArrayBool (id);
    IPosition start, end, incr;
    slicer.inferShapeFromSource (arr.array().shape(), start, end, incr);
    return arr(start, end, incr);
}
MArray<Int64> TableExprNodeArray::getSliceInt (const TableExprId& id,
                                               const Slicer& slicer)
{
    MArray<Int64> arr = getArrayInt (id);
    IPosition start, end, incr;
    slicer.inferShapeFromSource (arr.shape(), start, end, incr);
    return arr(start, end, incr);
}
MArray<Double> TableExprNodeArray::getSliceDouble (const TableExprId& id,
                                                   const Slicer& slicer)
{
    MArray<Double> arr = getArrayDouble (id);
    IPosition start, end, incr;
    slicer.inferShapeFromSource (arr.array().shape(), start, end, incr);
    return arr(start, end, incr);
}
MArray<DComplex> TableExprNodeArray::getSliceDComplex (const TableExprId& id,
                                                       const Slicer& slicer)
{
    MArray<DComplex> arr = getArrayDComplex (id);
    IPosition start, end, incr;
    slicer.inferShapeFromSource (arr.array().shape(), start, end, incr);
    return arr(start, end, incr);
}
MArray<String> TableExprNodeArray::getSliceString (const TableExprId& id,
                                                   const Slicer& slicer)
{
    MArray<String> arr = getArrayString (id);
    IPosition start, end, incr;
    slicer.inferShapeFromSource (arr.array().shape(), start, end, incr);
    return arr(start, end, incr);
}
MArray<MVTime> TableExprNodeArray::getSliceDate (const TableExprId& id,
                                                 const Slicer& slicer)
{
    MArray<MVTime> arr = getArrayDate (id);
    IPosition start, end, incr;
    slicer.inferShapeFromSource (arr.array().shape(), start, end, incr);
    return arr(start, end, incr);
}

Array<Bool>     TableExprNodeArray::getElemColumnBool (const Vector<uInt>&,
                                                       const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnBool(Slicer) not implemented)");
    return Array<Bool>();
}
Array<uChar>    TableExprNodeArray::getElemColumnuChar (const Vector<uInt>&,
                                                        const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnuChar(Slicer) not implemented)");
    return Array<uChar>();
}
Array<Short>    TableExprNodeArray::getElemColumnShort (const Vector<uInt>&,
                                                        const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnShort(Slicer) not implemented)");
    return Array<Short>();
}
Array<uShort>   TableExprNodeArray::getElemColumnuShort (const Vector<uInt>&,
                                                         const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnuShort(Slicer) not implemented)");
    return Array<uShort>();
}
Array<Int>      TableExprNodeArray::getElemColumnInt (const Vector<uInt>&,
                                                      const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnInt(Slicer) not implemented)");
    return Array<Int>();
}
Array<uInt>     TableExprNodeArray::getElemColumnuInt (const Vector<uInt>&,
                                                       const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnuInt(Slicer) not implemented)");
    return Array<uInt>();
}
Array<Float>    TableExprNodeArray::getElemColumnFloat (const Vector<uInt>&,
                                                        const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnFloat(Slicer) not implemented)");
    return Array<Float>();
}
Array<Double>   TableExprNodeArray::getElemColumnDouble (const Vector<uInt>&,
                                                         const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnDouble(Slicer) not implemented)");
    return Array<Double>();
}
Array<Complex>  TableExprNodeArray::getElemColumnComplex (const Vector<uInt>&,
                                                          const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnComplex(Slicer) not implemented)");
    return Array<Complex>();
}
Array<DComplex> TableExprNodeArray::getElemColumnDComplex (const Vector<uInt>&,
                                                           const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnDComplex(Slicer) not implemented)");
    return Array<DComplex>();
}
Array<String>   TableExprNodeArray::getElemColumnString (const Vector<uInt>&,
                                                         const Slicer&)
{
    TableExprNode::throwInvDT
                       ("(getElemColumnString(Slicer) not implemented)");
    return Array<String>();
}

MArray<Int64> TableExprNodeArray::makeArray (const IPosition& shape,
                                             Int64 value)
{
    Array<Int64> arr(shape);
    arr.set (value);
    return MArray<Int64>(arr);
}
MArray<Double> TableExprNodeArray::makeArray (const IPosition& shape,
					     Double value)
{
    Array<Double> arr(shape);
    arr.set (value);
    return MArray<Double>(arr);
}
MArray<DComplex> TableExprNodeArray::makeArray (const IPosition& shape,
					       const DComplex& value)
{
    Array<DComplex> arr(shape);
    arr.set (value);
    return MArray<DComplex>(arr);
}



// ----------------------------------
// TableExprNodeArrayColumn functions
// ----------------------------------

TableExprNodeArrayColumn::TableExprNodeArrayColumn
                                           (const ROTableColumn& tablecol,
					    const Table& table)
: TableExprNodeArray (NTNumeric, OtColumn),
  tabCol_p           (tablecol)
{
    //# Fill in the real data type and the base table pointer.
    switch (tabCol_p.columnDesc().dataType()) {
    case TpBool:
	dtype_p = NTBool;
	break;
    case TpString:
	dtype_p = NTString;
	break;
    case TpComplex:
    case TpDComplex:
	dtype_p = NTComplex;
	break;
    case TpChar:
    case TpUChar:
    case TpShort:
    case TpUShort:
    case TpInt:
    case TpUInt:
	dtype_p = NTInt;
        break;
    case TpFloat:
    case TpDouble:
	dtype_p = NTDouble;
	break;
    default:
	throw (TableInvExpr (tabCol_p.columnDesc().name(),
			     "unknown data type"));
    }
    table_p = table;
    exprtype_p = Variable;
    // Set the fixed shape and dimensionality (if known).
    ndim_p = tabCol_p.ndimColumn();
    if (ndim_p == 0) {
	ndim_p = -1;                         // unknown dimensionality
    }
    shape_p = tabCol_p.shapeColumn();
    setUnit (TableExprNodeColumn::getColumnUnit(tabCol_p));
}

TableExprNodeArrayColumn::~TableExprNodeArrayColumn()
{}

const IPosition& TableExprNodeArrayColumn::getShape (const TableExprId& id)
{
    varShape_p.resize (0);
    varShape_p = tabCol_p.shape (id.rownr());
    return varShape_p;
}
Bool TableExprNodeArrayColumn::isDefined (const TableExprId& id)
{
    return tabCol_p.isDefined (id.rownr());
}

Bool TableExprNodeArrayColumn::getColumnDataType (DataType& dt) const
{
    dt = tabCol_p.columnDesc().dataType();
    return True;
}



TableExprNodeArrayColumnBool::TableExprNodeArrayColumnBool
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnBool::~TableExprNodeArrayColumnBool()
{}

Bool TableExprNodeArrayColumnBool::getElemBool (const TableExprId& id,
						const Slicer& index)
{
    Array<Bool> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const Bool* f = arr.getStorage (deleteIt);
    Bool val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<Bool> TableExprNodeArrayColumnBool::getArrayBool (const TableExprId& id)
{
    return MArray<Bool> (col_p(id.rownr()));
}
MArray<Bool> TableExprNodeArrayColumnBool::getSliceBool (const TableExprId& id,
                                                         const Slicer& index)
{
    return MArray<Bool> (col_p.getSlice (id.rownr(), index));
}
Array<Bool> TableExprNodeArrayColumnBool::getElemColumnBool
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnuChar::TableExprNodeArrayColumnuChar
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnuChar::~TableExprNodeArrayColumnuChar()
{}

Int64 TableExprNodeArrayColumnuChar::getElemInt (const TableExprId& id,
                                                 const Slicer& index)
{
    Array<uChar> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const uChar* f = arr.getStorage (deleteIt);
    uChar val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<Int64> TableExprNodeArrayColumnuChar::getArrayInt
                                                    (const TableExprId& id)
{
    Array<uChar> arr = col_p (id.rownr());
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
MArray<Int64> TableExprNodeArrayColumnuChar::getSliceInt
                                                    (const TableExprId& id,
						     const Slicer& index)
{
    Array<uChar> arr = col_p.getSlice (id.rownr(), index);
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
Array<uChar> TableExprNodeArrayColumnuChar::getElemColumnuChar
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnShort::TableExprNodeArrayColumnShort
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnShort::~TableExprNodeArrayColumnShort()
{}

Int64 TableExprNodeArrayColumnShort::getElemInt (const TableExprId& id,
                                                 const Slicer& index)
{
    Array<Short> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const Short* f = arr.getStorage (deleteIt);
    Short val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<Int64> TableExprNodeArrayColumnShort::getArrayInt
                                                    (const TableExprId& id)
{
    Array<Short> arr = col_p (id.rownr());
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
MArray<Int64> TableExprNodeArrayColumnShort::getSliceInt
                                                    (const TableExprId& id,
						     const Slicer& index)
{
    Array<Short> arr = col_p.getSlice (id.rownr(), index);
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
Array<Short> TableExprNodeArrayColumnShort::getElemColumnShort
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnuShort::TableExprNodeArrayColumnuShort
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnuShort::~TableExprNodeArrayColumnuShort()
{}

Int64 TableExprNodeArrayColumnuShort::getElemInt (const TableExprId& id,
                                                  const Slicer& index)
{
    Array<uShort> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const uShort* f = arr.getStorage (deleteIt);
    uShort val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<Int64> TableExprNodeArrayColumnuShort::getArrayInt
                                                     (const TableExprId& id)
{
    Array<uShort> arr = col_p (id.rownr());
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
MArray<Int64> TableExprNodeArrayColumnuShort::getSliceInt
                                                     (const TableExprId& id,
						      const Slicer& index)
{
    Array<uShort> arr = col_p.getSlice (id.rownr(), index);
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
Array<uShort> TableExprNodeArrayColumnuShort::getElemColumnuShort
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnInt::TableExprNodeArrayColumnInt
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnInt::~TableExprNodeArrayColumnInt()
{}

Int64 TableExprNodeArrayColumnInt::getElemInt (const TableExprId& id,
                                               const Slicer& index)
{
    Array<Int> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const Int* f = arr.getStorage (deleteIt);
    Int val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<Int64> TableExprNodeArrayColumnInt::getArrayInt
                                                  (const TableExprId& id)
{
    Array<Int> arr = col_p (id.rownr());
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
MArray<Int64> TableExprNodeArrayColumnInt::getSliceInt
                                                  (const TableExprId& id,
						   const Slicer& index)
{
    Array<Int> arr = col_p.getSlice (id.rownr(), index);
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
Array<Int> TableExprNodeArrayColumnInt::getElemColumnInt
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnuInt::TableExprNodeArrayColumnuInt
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnuInt::~TableExprNodeArrayColumnuInt()
{}

Int64 TableExprNodeArrayColumnuInt::getElemInt (const TableExprId& id,
                                                const Slicer& index)
{
    Array<uInt> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const uInt* f = arr.getStorage (deleteIt);
    uInt val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<Int64> TableExprNodeArrayColumnuInt::getArrayInt
                                                   (const TableExprId& id)
{
    Array<uInt> arr = col_p (id.rownr());
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
MArray<Int64> TableExprNodeArrayColumnuInt::getSliceInt
                                                   (const TableExprId& id,
						    const Slicer& index)
{
    Array<uInt> arr = col_p.getSlice (id.rownr(), index);
    Array<Int64> out (arr.shape());
    convertArray (out, arr);
    return MArray<Int64> (out);
}
Array<uInt> TableExprNodeArrayColumnuInt::getElemColumnuInt
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnFloat::TableExprNodeArrayColumnFloat
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnFloat::~TableExprNodeArrayColumnFloat()
{}

Double TableExprNodeArrayColumnFloat::getElemDouble (const TableExprId& id,
						     const Slicer& index)
{
    Array<Float> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const Float* f = arr.getStorage (deleteIt);
    Float val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<Double> TableExprNodeArrayColumnFloat::getArrayDouble
                                                    (const TableExprId& id)
{
    Array<Float> arr = col_p (id.rownr());
    Array<Double> out (arr.shape());
    convertArray (out, arr);
    return MArray<Double>(out);
}
MArray<Double> TableExprNodeArrayColumnFloat::getSliceDouble
                                                    (const TableExprId& id,
						     const Slicer& index)
{
    Array<Float> arr = col_p.getSlice (id.rownr(), index);
    Array<Double> out (arr.shape());
    convertArray (out, arr);
    return MArray<Double>(out);
}
Array<Float> TableExprNodeArrayColumnFloat::getElemColumnFloat
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnDouble::TableExprNodeArrayColumnDouble
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnDouble::~TableExprNodeArrayColumnDouble()
{}

Double TableExprNodeArrayColumnDouble::getElemDouble (const TableExprId& id,
						      const Slicer& index)
{
    Array<Double> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const Double* f = arr.getStorage (deleteIt);
    Double val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<Double> TableExprNodeArrayColumnDouble::getArrayDouble
                                                     (const TableExprId& id)
{
    return MArray<Double> (col_p (id.rownr()));
}
MArray<Double> TableExprNodeArrayColumnDouble::getSliceDouble
                                                     (const TableExprId& id,
						      const Slicer& index)
{
    return MArray<Double> (col_p.getSlice (id.rownr(), index));
}
Array<Double> TableExprNodeArrayColumnDouble::getElemColumnDouble
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnComplex::TableExprNodeArrayColumnComplex
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnComplex::~TableExprNodeArrayColumnComplex()
{}

DComplex TableExprNodeArrayColumnComplex::getElemDComplex
                                                     (const TableExprId& id,
						      const Slicer& index)
{
    Array<Complex> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const Complex* f = arr.getStorage (deleteIt);
    DComplex val;
    val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<DComplex> TableExprNodeArrayColumnComplex::getArrayDComplex
                                                     (const TableExprId& id)
{
    Array<Complex> arr = col_p (id.rownr());
    Array<DComplex> out (arr.shape());
    convertArray (out, arr);
    return MArray<DComplex> (out);
}
MArray<DComplex> TableExprNodeArrayColumnComplex::getSliceDComplex
                                                     (const TableExprId& id,
						      const Slicer& index)
{
    Array<Complex> arr = col_p.getSlice (id.rownr(), index);
    Array<DComplex> out (arr.shape());
    convertArray (out, arr);
    return MArray<DComplex> (out);
}
Array<Complex> TableExprNodeArrayColumnComplex::getElemColumnComplex
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnDComplex::TableExprNodeArrayColumnDComplex
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnDComplex::~TableExprNodeArrayColumnDComplex()
{}

DComplex TableExprNodeArrayColumnDComplex::getElemDComplex
                                                     (const TableExprId& id,
						      const Slicer& index)
{
    Array<DComplex> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const DComplex* f = arr.getStorage (deleteIt);
    DComplex val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<DComplex> TableExprNodeArrayColumnDComplex::getArrayDComplex
                                                     (const TableExprId& id)
{
    return MArray<DComplex> (col_p (id.rownr()));
}
MArray<DComplex> TableExprNodeArrayColumnDComplex::getSliceDComplex
                                                     (const TableExprId& id,
						      const Slicer& index)
{
    return MArray<DComplex> (col_p.getSlice (id.rownr(), index));
}
Array<DComplex> TableExprNodeArrayColumnDComplex::getElemColumnDComplex
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}

TableExprNodeArrayColumnString::TableExprNodeArrayColumnString
                                           (const ROTableColumn& col,
					    const Table& table)
: TableExprNodeArrayColumn (col, table),
  col_p                    (col)
{}
TableExprNodeArrayColumnString::~TableExprNodeArrayColumnString()
{}

String TableExprNodeArrayColumnString::getElemString (const TableExprId& id,
						      const Slicer& index)
{
    Array<String> arr = col_p.getSlice (id.rownr(), index);
    Bool deleteIt;
    const String* f = arr.getStorage (deleteIt);
    String val = *f;
    arr.freeStorage (f, deleteIt);
    return val;
}
MArray<String> TableExprNodeArrayColumnString::getArrayString
                                                     (const TableExprId& id)
{
    return MArray<String> (col_p (id.rownr()));
}
MArray<String> TableExprNodeArrayColumnString::getSliceString
                                                     (const TableExprId& id,
						      const Slicer& index)
{
    return MArray<String> (col_p.getSlice (id.rownr(), index));
}
Array<String> TableExprNodeArrayColumnString::getElemColumnString
(const Vector<uInt>& rownrs, const Slicer& index)
{
    return col_p.getColumnCells (rownrs, index);
}




// ----------------------------
// TableExprNodeIndex functions
// ----------------------------

TableExprNodeIndex::TableExprNodeIndex (const TableExprNodeSet& indices,
					const TaQLStyle& style)
: TableExprNodeMulti (NTInt, VTIndex, OtColumn, indices),
  origin_p           (style.origin()),
  endMinus_p         (style.origin()),
  isCOrder_p         (style.isCOrder()),
  isSingle_p         (True)
{
    if (style.isEndExcl()) ++endMinus_p;
    fillIndex (indices);
}

TableExprNodeIndex::~TableExprNodeIndex()
{}

void TableExprNodeIndex::checkIndexValues (const TableExprNodeRep* arrayNode)
{
    uInt i;
    Int ndim = arrayNode->ndim();
    uInt n = start_p.nelements();
    // Check against dimensionality (if fixed).
    if (ndim >= 0  &&  ndim != Int(n)) {
	throw (TableInvExpr ("#indices mismatches array dimensionality"));
    }
    // Check start and increment values.
    for (i=0; i<n; i++) {
	if (!varIndex_p[3*i]) {
	    if (start_p(i) < 0) {
		throw (TableInvExpr ("index value before array origin"));
	    }
	}
	if (!varIndex_p[3*i + 2]) {
	    if (incr_p(i) < 0) {
		throw (TableInvExpr ("index increment value is negative"));
	    }
	}
    }
    // Check against array shape (if fixed).
    IPosition shape = arrayNode->shape();
    if (shape.nelements() > 0) {
	for (i=0; i<n; i++) {
	    if (!varIndex_p[3*i]) {
		if (start_p(i) >= shape(i)) {
		    throw (TableInvExpr("index value exceeds array shape"));
		}
	    }
	    if (!varIndex_p[3*i + 1]) {
		if (end_p(i) >= shape(i)) {
		    throw (TableInvExpr("index end value exceeds array shape"));
		}
	    }
	}
    }
}

void TableExprNodeIndex::fillSlicer (const TableExprId& id)
{
    uInt n = varIndex_p.nelements();
    uInt i = 0;
    uInt j = 0;
    while (j < n) {
	if (varIndex_p[j]) {
	    start_p(i) = operands_p[j]->getInt(id) - origin_p;
	}
	j++;
	if (varIndex_p[j]) {
	    if (operands_p[j] == 0) {
		end_p(i) = start_p(i);
	    }else{
		Int64 val = operands_p[j]->getInt (id);
		if (val < 0) {
		    end_p = Slicer::MimicSource;
		}else{
		    end_p(i) = val - endMinus_p;
		}
	    }
	}
	j++;
	if (varIndex_p[j]) {
	    incr_p(i) = operands_p[j]->getInt(id);
	}
	j++;
	i++;
    }
    slicer_p = Slicer (start_p, end_p, incr_p, Slicer::endIsLast);
}

// Fill the children pointers of a node.
// Also reduce the tree if possible by combining constants.
void TableExprNodeIndex::fillIndex (const TableExprNodeSet& indices)
{
    // Check that the set elements have equal data types.
    indices.checkEqualDataTypes();
    // Check that the set contains discrete values.
    if (! indices.isDiscrete()) {
	throw (TableInvExpr ("Index values must be discrete (with possible :"));
    }
    TableExprNodeRep* rep;
    // Copy block of start, end, and increment.
    // Determine if single element subscripting is done.
    // That is true if all starts are given and no end and increment values.
    // Check if all indices have data type Int and are scalars.
    uInt n = indices.nelements();
    operands_p.resize (3 * n);
    operands_p.set (static_cast<TableExprNodeRep*>(0));
    uInt j = 0;
    for (uInt i=0; i<n; i++) {
        uInt inx = (isCOrder_p  ?  n-i-1 : i);
	rep = const_cast<TableExprNodeRep*>(indices[inx].start());
	if (rep != 0) {
	    operands_p[j] = rep->link();
	}else{
	    isSingle_p = False;
	}
	j++;
	rep = const_cast<TableExprNodeRep*>(indices[inx].end());
	if (rep != 0) {
	    operands_p[j] = rep->link();
	    isSingle_p = False;
	}
	j++;
	rep = const_cast<TableExprNodeRep*>(indices[inx].increment());
	if (rep != 0) {
	    operands_p[j] = rep->link();
	    isSingle_p = False;
	}
	j++;
    }
    // Check if all indices have data type Int and are scalars.
    for (uInt i=0; i<j; i++) {
	if (operands_p[i] != 0) {
	    if (operands_p[i]->dataType()  != NTInt
	    ||  operands_p[i]->valueType() != VTScalar) {
		throw (TableInvExpr ("Index value must an integer scalar"));
	    }
	}
    }
    convertConstIndex();
    if (isConstant()) {
	slicer_p = Slicer (start_p, end_p, incr_p, Slicer::endIsLast);
    }
}

void TableExprNodeIndex::convertConstIndex()
{
    TableExprNodeRep* rep;
    uInt n = operands_p.nelements() / 3;
    start_p.resize (n);
    end_p.resize (n);
    incr_p.resize (n);
    varIndex_p.resize (3*n);
    varIndex_p.set (False);
    uInt j = 0;
    for (uInt i=0; i<n; i++) {
	// If no start value is given, it is 0.
	rep = operands_p[j];
	start_p(i) = 0;
	if (rep != 0) {
	    if (rep->isConstant()) {
		start_p(i) = rep->getInt(0) - origin_p;
	    }else{
		varIndex_p[j] = True;
	    }
	}
	j++;
	// If no end value is given, it is initially set to the end.
	// If a start is given, it is set to start.
	// A negative end means till the end.
	rep = operands_p[j];
	end_p(i) = Slicer::MimicSource;
	if (rep != 0) {
	    if (rep->isConstant()) {
		Int64 val = rep->getInt(0);
		if (val < 0) {
		    end_p = Slicer::MimicSource;
		}else{
		    end_p(i) = val - endMinus_p;
		}
	    }else{
		varIndex_p[j] = True;
	    }
	}else{
	    if (operands_p[j-1] != 0) {
		end_p(i) = start_p(i);
		varIndex_p[j] = varIndex_p[j-1];
	    }
	}
	    
	j++;
	// If no increment value is given, it is 1.
	rep = operands_p[j];
	incr_p(i) = 1;
	if (rep != 0) {
	    if (rep->isConstant()) {
              incr_p(i) = rep->getInt(0);
	    }else{
		varIndex_p[j] = True;
	    }
	}
	j++;
    }
}




// ----------------------
// TableExprNodeArrayPart
// ----------------------
TableExprNodeArrayPart::TableExprNodeArrayPart (TableExprNodeRep* arrayNode,
						TableExprNodeIndex* indexNode)
: TableExprNodeArray (arrayNode->dataType(), OtSlice),
  indexNode_p        (indexNode),
  colNode_p          (0)
{
    checkTablePtr (indexNode);
    checkTablePtr (arrayNode);
    fillExprType  (indexNode);
    fillExprType  (arrayNode);
    arrNode_p = dynamic_cast<TableExprNodeArray*>(arrayNode);
    AlwaysAssert (arrNode_p, AipsError);
    // If indexing a single element, the result is a scalar.
    if (indexNode->isSingle()) {
	vtype_p = VTScalar;
	ndim_p  = 0;
    } else if (indexNode->isConstant()) {
	// Otherwise if the index node is constant, it may be possible
	// to determine the resulting shape.
	const Slicer& slicer = indexNode->getSlicer(0);
	// If all slicer lengths are defined, that is the resulting shape.
	if (slicer.isFixed()) {
	    shape_p = slicer.length();
	    ndim_p  = shape_p.nelements();
	}else{
	    // If some are depending on array shape, the resulting
	    // shape can be determined if the array shape is fixed.
	    IPosition arrshp = arrayNode->shape();
	    if (arrshp.nelements() > 0) {
		IPosition blc,trc,inc;
		shape_p = slicer.inferShapeFromSource (arrshp, blc, trc, inc);
		ndim_p  = shape_p.nelements();
	    }
	}
    }
    if (indexNode->isConstant()) {
	// If the constant child is an ArrayColumn, things can be
	// improved in getColumnXXX.
	colNode_p = dynamic_cast<TableExprNodeArrayColumn*>(arrayNode);
    }
}

TableExprNodeArrayPart::~TableExprNodeArrayPart()
{}


void TableExprNodeArrayPart::show (ostream& os, uInt indent) const
{
    TableExprNodeRep::show (os, indent);
    os << "array: ";
    lnode_p->show (os, indent+2);
    os << "index: ";
    indexNode_p->show (os, indent+2);
}

Bool TableExprNodeArrayPart::getColumnDataType (DataType& dt) const
{
    //# Return data type of column if constant index.
    if (indexNode_p->isConstant()) {
	return lnode_p->getColumnDataType (dt);
    }
    return False;
}

//# Note that all following casts are perfectly safe.
Bool TableExprNodeArrayPart::getBool (const TableExprId& id)
{
    DebugAssert (valueType() == VTScalar, AipsError);
    return arrNode_p->getElemBool (id, indexNode_p->getSlicer(id));
}
Int64 TableExprNodeArrayPart::getInt (const TableExprId& id)
{
    DebugAssert (valueType() == VTScalar, AipsError);
    return arrNode_p->getElemInt (id, indexNode_p->getSlicer(id));
}
Double TableExprNodeArrayPart::getDouble (const TableExprId& id)
{
    DebugAssert (valueType() == VTScalar, AipsError);
    return arrNode_p->getElemDouble (id, indexNode_p->getSlicer(id));
}
DComplex TableExprNodeArrayPart::getDComplex (const TableExprId& id)
{
    DebugAssert (valueType() == VTScalar, AipsError);
    return arrNode_p->getElemDComplex (id, indexNode_p->getSlicer(id));
}
String TableExprNodeArrayPart::getString (const TableExprId& id)
{
    DebugAssert (valueType() == VTScalar, AipsError);
    return arrNode_p->getElemString (id, indexNode_p->getSlicer(id));
}
MVTime TableExprNodeArrayPart::getDate (const TableExprId& id)
{
    DebugAssert (valueType() == VTScalar, AipsError);
    return arrNode_p->getElemDate (id, indexNode_p->getSlicer(id));
}

MArray<Bool> TableExprNodeArrayPart::getArrayBool (const TableExprId& id)
{
    DebugAssert (valueType() == VTArray, AipsError);
    return arrNode_p->getSliceBool (id, indexNode_p->getSlicer(id));
}
MArray<Int64> TableExprNodeArrayPart::getArrayInt (const TableExprId& id)
{
    DebugAssert (valueType() == VTArray, AipsError);
    return arrNode_p->getSliceInt (id, indexNode_p->getSlicer(id));
}
MArray<Double> TableExprNodeArrayPart::getArrayDouble (const TableExprId& id)
{
    DebugAssert (valueType() == VTArray, AipsError);
    return arrNode_p->getSliceDouble (id, indexNode_p->getSlicer(id));
}
MArray<DComplex> TableExprNodeArrayPart::getArrayDComplex
                                                     (const TableExprId& id)
{
    DebugAssert (valueType() == VTArray, AipsError);
    return arrNode_p->getSliceDComplex (id, indexNode_p->getSlicer(id));
}
MArray<String> TableExprNodeArrayPart::getArrayString (const TableExprId& id)
{
    DebugAssert (valueType() == VTArray, AipsError);
    return arrNode_p->getSliceString (id, indexNode_p->getSlicer(id));
}
MArray<MVTime> TableExprNodeArrayPart::getArrayDate (const TableExprId& id)
{
    DebugAssert (valueType() == VTArray, AipsError);
    return arrNode_p->getSliceDate (id, indexNode_p->getSlicer(id));
}

Array<Bool> TableExprNodeArrayPart::getColumnBool (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnBool (rownrs);
    }
    return colNode_p->getElemColumnBool (rownrs, indexNode_p->getSlicer(0));
}
Array<uChar>    TableExprNodeArrayPart::getColumnuChar (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnuChar (rownrs);
    }
    return colNode_p->getElemColumnuChar (rownrs, indexNode_p->getSlicer(0));
}
Array<Short>    TableExprNodeArrayPart::getColumnShort (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnShort (rownrs);
    }
    return colNode_p->getElemColumnShort (rownrs, indexNode_p->getSlicer(0));
}
Array<uShort>   TableExprNodeArrayPart::getColumnuShort (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnuShort (rownrs);
    }
    return colNode_p->getElemColumnuShort (rownrs, indexNode_p->getSlicer(0));
}
Array<Int>      TableExprNodeArrayPart::getColumnInt (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnInt (rownrs);
    }
    return colNode_p->getElemColumnInt (rownrs, indexNode_p->getSlicer(0));
}
Array<uInt>     TableExprNodeArrayPart::getColumnuInt (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnuInt (rownrs);
    }
    return colNode_p->getElemColumnuInt (rownrs, indexNode_p->getSlicer(0));
}
Array<Float>    TableExprNodeArrayPart::getColumnFloat (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnFloat (rownrs);
    }
    return colNode_p->getElemColumnFloat (rownrs, indexNode_p->getSlicer(0));
}
Array<Double>   TableExprNodeArrayPart::getColumnDouble (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnDouble (rownrs);
    }
    return colNode_p->getElemColumnDouble (rownrs, indexNode_p->getSlicer(0));
}
Array<Complex>  TableExprNodeArrayPart::getColumnComplex (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnComplex (rownrs);
    }
    return colNode_p->getElemColumnComplex (rownrs, indexNode_p->getSlicer(0));
}
Array<DComplex> TableExprNodeArrayPart::getColumnDComplex (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnDComplex (rownrs);
    }
    return colNode_p->getElemColumnDComplex (rownrs, indexNode_p->getSlicer(0));
}
Array<String>   TableExprNodeArrayPart::getColumnString (const Vector<uInt>& rownrs)
{
    if (colNode_p == 0) {
	return TableExprNodeRep::getColumnString (rownrs);
    }
    return colNode_p->getElemColumnString (rownrs, indexNode_p->getSlicer(0));
}

} //# NAMESPACE CASA - END

