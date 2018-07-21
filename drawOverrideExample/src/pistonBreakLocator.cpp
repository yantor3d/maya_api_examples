#include "pistonBreakLocator.h"

#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnData.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MTypeId.h>

MObject PistonBreakLocator::startMatrixAttr;
MObject PistonBreakLocator::endMatrixAttr;
MObject PistonBreakLocator::minLengthAttr;
MObject PistonBreakLocator::maxLengthAttr;

MObject PistonBreakLocator::drawAttr; 
MObject PistonBreakLocator::radiusAttr;

MObject PistonBreakLocator::lengthAttr;
MObject PistonBreakLocator::isBrokenAttr;

PistonBreakLocator::PistonBreakLocator()  
{

}

PistonBreakLocator::~PistonBreakLocator() 
{

}

void* PistonBreakLocator::create()
{
    return new PistonBreakLocator();
}

MStatus PistonBreakLocator::initialize()
{
    MStatus status;

    MFnNumericAttribute n;
    MFnTypedAttribute t;

    startMatrixAttr = t.create("startMatrix", "sm", MFnData::kMatrix, &status);
    endMatrixAttr = t.create("endMatrix", "em", MFnData::kMatrix, &status);

    minLengthAttr = n.create("minLength", "mnl", MFnNumericData::Type::kDouble, 1.0, &status);
    n.setKeyable(true);
    n.setChannelBox(true);

    maxLengthAttr = n.create("maxLength", "mxl", MFnNumericData::Type::kDouble, 1.0, &status);
    n.setKeyable(true);
    n.setChannelBox(true);

    drawAttr = n.create("draw", "d", MFnNumericData::Type::kBoolean, true, &status);
    n.setKeyable(true);
    n.setChannelBox(true);

    radiusAttr = n.create("radius", "r", MFnNumericData::Type::kDouble, 1.0, &status);
    n.setKeyable(true);
    n.setChannelBox(true);
    n.setMin(0.0);

    lengthAttr = n.create("length", "len", MFnNumericData::Type::kDouble, 0.0, &status);
    n.setWritable(false);
    n.setStorable(false);

    isBrokenAttr = n.create("isBroken", "ib", MFnNumericData::Type::kBoolean, false, &status);
    n.setWritable(false);
    n.setStorable(false);

    addAttribute(startMatrixAttr);
    addAttribute(endMatrixAttr);
    addAttribute(minLengthAttr);
    addAttribute(maxLengthAttr);

    addAttribute(drawAttr);
    addAttribute(radiusAttr);

    addAttribute(lengthAttr);
    addAttribute(isBrokenAttr);

    attributeAffects(startMatrixAttr, lengthAttr);
    attributeAffects(endMatrixAttr, lengthAttr);
    attributeAffects(minLengthAttr, lengthAttr);
    attributeAffects(maxLengthAttr, lengthAttr);

    attributeAffects(startMatrixAttr, isBrokenAttr);
    attributeAffects(endMatrixAttr, isBrokenAttr);
    attributeAffects(minLengthAttr, isBrokenAttr);
    attributeAffects(maxLengthAttr, isBrokenAttr);

    return status;
}

MStatus PistonBreakLocator::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    MStatus status;

    if (plug == lengthAttr || plug == isBrokenAttr)
    {
        MObject startMatrixData = dataBlock.inputValue(startMatrixAttr).data();
        MObject endMatrixData = dataBlock.inputValue(endMatrixAttr).data();

        MMatrix startMatrix = MFnMatrixData(startMatrixData).matrix();
        MMatrix endMatrix   = MFnMatrixData(endMatrixData).matrix();

        double minLength = dataBlock.inputValue(minLengthAttr).asDouble();
        double maxLength = dataBlock.inputValue(maxLengthAttr).asDouble();

        MPoint startPoint(startMatrix(3, 0), startMatrix(3, 1), startMatrix(3, 2));
        MPoint endPoint(endMatrix(3, 0), endMatrix(3, 1), endMatrix(3, 2));

        double length = startPoint.distanceTo(endPoint);
        bool isBroken = maxLength < length || length < minLength;

        MDataHandle lengthDataHandle = dataBlock.outputValue(lengthAttr);
        lengthDataHandle.setDouble(length);
        lengthDataHandle.setClean();

        MDataHandle isBrokenDataHandle = dataBlock.outputValue(isBrokenAttr);
        isBrokenDataHandle.setBool(isBroken);
        isBrokenDataHandle.setClean();

        status = MStatus::kSuccess;
    } else {
        return MStatus::kUnknownParameter;
    }

    return status;
}