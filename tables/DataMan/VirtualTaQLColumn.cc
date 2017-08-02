//# VirtualTaQLColumn.h: Virtual column engine based on TaQL
//# Copyright (C) 2005
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
//# $Id: VirtualTaQLColumn.cc 21521 2014-12-10 08:06:42Z gervandiepen $

#include <casacore/tables/DataMan/VirtualTaQLColumn.h>
#include <casacore/tables/Tables/Table.h>
#include <casacore/tables/Tables/TableColumn.h>
#include <casacore/tables/Tables/TableRecord.h>
#include <casacore/tables/TaQL/TableParse.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/tables/DataMan/DataManError.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Arrays/ArrayMath.h>
#include <casacore/casa/Utilities/Assert.h>


namespace casacore { //# NAMESPACE CASACORE - BEGIN

VirtualTaQLColumn::VirtualTaQLColumn (const String& expr)
: itsDataType     (TpOther),
  itsIsArray      (False),
  itsExpr         (expr),
  itsNode         (0),
  itsTempWritable (False),
  itsCurRow       (-1),
  itsCurResult    (0)
{}

VirtualTaQLColumn::VirtualTaQLColumn (const Record& spec)
: itsDataType     (TpOther),
  itsIsArray      (False),
  itsNode         (0),
  itsTempWritable (False),
  itsCurRow       (-1),
  itsCurResult    (0)
{
  if (spec.isDefined ("TAQLCALCEXPR")) {
    itsExpr = spec.asString ("TAQLCALCEXPR");
  }
}

VirtualTaQLColumn::~VirtualTaQLColumn()
{
  delete itsCurResult;
  delete itsNode;
}

DataManager* VirtualTaQLColumn::clone() const
{
  DataManager* dmPtr = new VirtualTaQLColumn (itsExpr);
  return dmPtr;
}

DataManagerColumn* VirtualTaQLColumn::makeScalarColumn (const String& name,
							int dataType,
							const String&)
{
  AlwaysAssert (dataType!=TpOther, AipsError);
  itsDataType   = dataType;
  itsIsArray    = False;
  itsColumnName = name;
  return this;
}

DataManagerColumn* VirtualTaQLColumn::makeIndArrColumn (const String& name,
							int dataType,
							const String&)
{
  AlwaysAssert (dataType!=TpOther, AipsError);
  itsDataType   = dataType;
  itsIsArray    = True;
  itsColumnName = name;
  return this;
}


void VirtualTaQLColumn::create (uInt)
{
  // Define a keyword in the column telling the expression.
  itsTempWritable = True;
  TableColumn tabcol (table(), itsColumnName);
  itsTempWritable = False;
  tabcol.rwKeywordSet().define ("_VirtualTaQLEngine_CalcExpr", itsExpr);
}

void VirtualTaQLColumn::prepare()
{
  // Get the expression.
  TableColumn tabcol (table(), itsColumnName);
  itsExpr = tabcol.keywordSet().asString ("_VirtualTaQLEngine_CalcExpr");
  // Compile the expression.
  TaQLResult res = tableCommand ("calc from $1 calc " + itsExpr, table());
  itsNode = new TableExprNode(res.node());
  // Check if the expression type matches the column type.
  if (itsNode->isScalar() == itsIsArray) {
    throw DataManError ("VirtualTaQLColumn: "
			"expression and " + itsColumnName +
                        " column type mismatch (not both scalar or array)");
  }
  // Check if the data types match.
  int exptype = itsDataType;
  switch (itsDataType) {
  case TpComplex:
    exptype = TpDComplex;
    break;
  case TpUChar:
  case TpShort:
  case TpUShort:
  case TpInt:
  case TpUInt:
  case TpInt64:
    exptype = TpInt;
    break;
  case TpFloat:
    exptype = TpDouble;
    break;
  default:
    break;
  }
  if (itsNode->dataType() != exptype) {
    throw DataManError ("VirtualTaQLColumn: "
			"expression and column " + itsColumnName +
                        " data type mismatch");
  }
  // Create the correct array type.
  switch (itsDataType) {
  case TpBool:
    itsCurResult = new Array<Bool>();
    break;
  case TpUChar:
    itsCurResult = new Array<uChar>();
    break;
  case TpShort:
    itsCurResult = new Array<Short>();
    break;
  case TpUShort:
    itsCurResult = new Array<uShort>();
    break;
  case TpInt:
    itsCurResult = new Array<Int>();
    break;
  case TpUInt:
    itsCurResult = new Array<uInt>();
    break;
  case TpInt64:
    itsCurResult = new Array<Int64>();
    break;
  case TpFloat:
    itsCurResult = new Array<Float>();
    break;
  case TpDouble:
    itsCurResult = new Array<Double>();
    break;
  case TpComplex:
    itsCurResult = new Array<Complex>();
    break;
  case TpDComplex:
    itsCurResult = new Array<DComplex>();
    break;
  case TpString:
    itsCurResult = new Array<String>();
    break;
  default:
    throw DataManError ("VirtualTaQLColumn::prepare - unknown data type");
  }
}

DataManager* VirtualTaQLColumn::makeObject (const String&,
					    const Record& spec)
{
  DataManager* dmPtr = new VirtualTaQLColumn (spec);
  return dmPtr;
}

void VirtualTaQLColumn::registerClass()
{
  DataManager::registerCtor (className(), makeObject);
}

String VirtualTaQLColumn::className()
{
  return "VirtualTaQLColumn";
}
String VirtualTaQLColumn::dataManagerType() const
{
  return className();
}

Record VirtualTaQLColumn::dataManagerSpec() const
{
  Record spec;
  spec.define ("TAQLCALCEXPR", itsExpr);
  return spec;
}

int VirtualTaQLColumn::dataType() const
{
  return itsDataType;
}

Bool VirtualTaQLColumn::isWritable() const
{
  return itsTempWritable;
}


uInt VirtualTaQLColumn::ndim (uInt rownr)
{
  return shape(rownr).nelements();
}

IPosition VirtualTaQLColumn::shape (uInt rownr)
{
  if (!itsIsArray) {
    return IPosition();
  }
  IPosition shp = itsNode->getNodeRep()->shape();
  if (shp.nelements() > 0) {
    return shp;
  }
  if (Int64(rownr) != itsCurRow) {
    itsCurShape = getResult (rownr, *itsCurResult);
    itsCurRow = rownr;
  }
  return itsCurShape;
}

Bool VirtualTaQLColumn::isShapeDefined (uInt)
{
  return True;
}


void VirtualTaQLColumn::getBoolV (uInt rownr, Bool* dataPtr)
{
  *dataPtr = itsNode->getBool (rownr);
}
void VirtualTaQLColumn::getuCharV (uInt rownr, uChar* dataPtr)
{
  *dataPtr = uChar(itsNode->getInt (rownr));
}
void VirtualTaQLColumn::getShortV (uInt rownr, Short* dataPtr)
{
  *dataPtr = Short(itsNode->getInt (rownr));
}
void VirtualTaQLColumn::getuShortV (uInt rownr, uShort* dataPtr)
{
  *dataPtr = uShort(itsNode->getInt (rownr));
}
void VirtualTaQLColumn::getIntV (uInt rownr, Int* dataPtr)
{
  *dataPtr = Int(itsNode->getInt (rownr));
}
void VirtualTaQLColumn::getuIntV (uInt rownr, uInt* dataPtr)
{
  *dataPtr = uInt(itsNode->getInt (rownr));
}
void VirtualTaQLColumn::getfloatV (uInt rownr, float* dataPtr)
{
  *dataPtr = Float(itsNode->getDouble (rownr));
}
void VirtualTaQLColumn::getdoubleV (uInt rownr, double* dataPtr)
{
  *dataPtr = itsNode->getDouble (rownr);
}
void VirtualTaQLColumn::getComplexV (uInt rownr, Complex* dataPtr)
{
  *dataPtr = Complex(itsNode->getDComplex (rownr));
}
void VirtualTaQLColumn::getDComplexV (uInt rownr, DComplex* dataPtr)
{
  *dataPtr = itsNode->getDComplex (rownr);
}
void VirtualTaQLColumn::getStringV (uInt rownr, String* dataPtr)
{
  *dataPtr = itsNode->getString (rownr);
}

void VirtualTaQLColumn::getArrayV (uInt rownr, ArrayBase& dataPtr)
{
  // Usually getShape is called before getArray.
  // To avoid double calculation of the same value, the result is cached
  // by getShape in itsCurResult (by getResult).
  if (Int64(rownr) != itsCurRow) {
    getResult (rownr, dataPtr);
    return;
  }
  dataPtr.assignBase (*itsCurResult);
}

IPosition VirtualTaQLColumn::getResult (uInt rownr, ArrayBase& dataPtr)
{
  IPosition shp;
  switch (itsDataType) {
  case TpBool:
    {
      Array<Bool> arr = itsNode->getArrayBool (rownr);
      Array<Bool>& out = static_cast<Array<Bool>&>(dataPtr);
      out.reference (arr);
      shp = out.shape();
      break;
    }
  case TpUChar:
    {
      Array<Int64> arr = itsNode->getArrayInt (rownr);
      Array<uChar>& out = static_cast<Array<uChar>&>(dataPtr);
      out.resize (arr.shape());
      convertArray (out, arr);
      shp = out.shape();
      break;
    }
  case TpShort:
    {
      Array<Int64> arr = itsNode->getArrayInt (rownr);
      Array<Short>& out = static_cast<Array<Short>&>(dataPtr);
      out.resize (arr.shape());
      convertArray (out, arr);
      shp = out.shape();
      break;
    }
  case TpUShort:
    {
      Array<Int64> arr = itsNode->getArrayInt (rownr);
      Array<uShort>& out = static_cast<Array<uShort>&>(dataPtr);
      out.resize (arr.shape());
      convertArray (out, arr);
      shp = out.shape();
      break;
    }
  case TpInt:
    {
      Array<Int64> arr = itsNode->getArrayInt (rownr);
      Array<Int>& out = static_cast<Array<Int>&>(dataPtr);
      out.resize (arr.shape());
      convertArray (out, arr);
      shp = out.shape();
      break;
    }
  case TpUInt:
    {
      Array<Int64> arr = itsNode->getArrayInt (rownr);
      Array<uInt>& out = static_cast<Array<uInt>&>(dataPtr);
      out.resize (arr.shape());
      convertArray (out, arr);
      shp = out.shape();
      break;
    }
  case TpFloat:
    {
      Array<Double> arr = itsNode->getArrayDouble (rownr);
      Array<Float>& out = static_cast<Array<Float>&>(dataPtr);
      out.resize (arr.shape());
      convertArray (out, arr);
      shp = out.shape();
      break;
    }
  case TpDouble:
    {
      Array<Double> arr  = itsNode->getArrayDouble (rownr);
      Array<Double>& out = static_cast<Array<Double>&>(dataPtr);
      out.reference (arr);
      shp = out.shape();
      break;
    }
  case TpComplex:
    {
      Array<DComplex> arr = itsNode->getArrayDComplex (rownr);
      Array<Complex>& out = static_cast<Array<Complex>&>(dataPtr);
      out.resize (arr.shape());
      convertArray (out, arr);
      shp = out.shape();
      break;
    }
  case TpDComplex:
    {
      Array<DComplex> arr  = itsNode->getArrayDComplex (rownr);
      Array<DComplex>& out = static_cast<Array<DComplex>&>(dataPtr);
      out.reference (arr);
      shp = out.shape();
      break;
    }
  case TpString:
    {
      Array<String> arr  = itsNode->getArrayString (rownr);
      Array<String>& out = static_cast<Array<String>&>(dataPtr);
      out.reference (arr);
      shp = out.shape();
      break;
    }
  default:
    throw DataManError ("VirtualTaQLColumn::getResult - unknown data type");
  }
  return shp;
}

void VirtualTaQLColumn::getScalarColumnV (ArrayBase& data)
  { dmGetScalarColumnV (data); }
void VirtualTaQLColumn::getScalarColumnCellsV (const RefRows& rownrs,
                                               ArrayBase& data)
  { dmGetScalarColumnCellsV (rownrs, data); }
void VirtualTaQLColumn::getArrayColumnV (ArrayBase& data)
  { dmGetArrayColumnV (data); }
void VirtualTaQLColumn::getArrayColumnCellsV (const RefRows& rownrs,
                                              ArrayBase& data)
  { dmGetArrayColumnCellsV (rownrs, data); }
void VirtualTaQLColumn::getSliceV (uInt rownr,
                                   const Slicer& slicer,
                                   ArrayBase& data)
  { dmGetSliceV (rownr, slicer, data); }
void VirtualTaQLColumn::getColumnSliceV (const Slicer& slicer,
                                         ArrayBase& data)
  { dmGetColumnSliceV (slicer, data); }
void VirtualTaQLColumn::getColumnSliceCellsV (const RefRows& rownrs,
                                              const Slicer& slicer,
                                              ArrayBase& data)
  { dmGetColumnSliceCellsV (rownrs, slicer, data); }


} //# NAMESPACE CASACORE - END

