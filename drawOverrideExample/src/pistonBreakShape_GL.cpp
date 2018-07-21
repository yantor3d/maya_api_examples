#include "pistonBreakLocator.h"

#include <maya/M3dView.h>
#include <maya/MBoundingBox.h>
#include <maya/MColor.h>
#include <maya/MDagPath.h>
#include <maya/MFnMatrixData.h>
#include <maya/MGL.h>
#include <maya/MGlobal.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MString.h>
#include <maya/MStatus.h>

bool PistonBreakLocator::isBounded() const
{
    return true;
}

MBoundingBox PistonBreakLocator::boundingBox() const
{
    MPoint corner1(-1.0, 0.0, -1.0);
    MPoint corner2( 1.0, 0.0,  1.0);

    return MBoundingBox(corner1, corner2);
}

void PistonBreakLocator::draw(M3dView& view, const MDagPath &path, M3dView::DisplayStyle style, M3dView::DisplayStatus displayStatus)
{
    MStatus status;

    MObject thisNode = thisMObject();

    bool draw = MPlug(thisNode, drawAttr).asBool();

    if (draw)
    {
        bool isBroken = MPlug(thisNode, isBrokenAttr).asBool();
        double radius = MPlug(thisNode, radiusAttr).asDouble();

        MObject worldInverseMatrixData = MPlug(thisNode, worldInverseMatrix).elementByLogicalIndex(0).asMObject();
        MObject startMatrixData = MPlug(thisNode, startMatrixAttr).asMObject();
        MObject endMatrixData = MPlug(thisNode, endMatrixAttr).asMObject();

        MMatrix wim = MFnMatrixData(worldInverseMatrixData, &status).matrix();
        CHECK_MSTATUS(status);

        MMatrix startMatrix = MFnMatrixData(startMatrixData).matrix() * wim;
        MMatrix endMatrix   = MFnMatrixData(endMatrixData).matrix() * wim;

        MPoint s(startMatrix(3, 0), startMatrix(3, 1), startMatrix(3, 2));
        MPoint e(endMatrix(3, 0), endMatrix(3, 1), endMatrix(3, 2));

        view.beginGL();

        if (isBroken)
        {
            MColor red(1.0f, 0.0f, 0.0);

            view.setDrawColor(red);

            GLUquadricObj *opj;
            opj = gluNewQuadric();

            gluSphere(opj, radius, 36, 8);   
        }

        view.endGL();
    }
}
