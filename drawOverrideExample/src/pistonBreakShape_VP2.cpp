#include "pistonBreakLocator.h"
#include "pistonBreakShape_VP2.h"

#include <maya/MBoundingBox.h>
#include <maya/MColor.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>

#if MAYA_API_VERSION >= 201600
#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>

PistonBreakLocatorDrawOverride::PistonBreakLocatorDrawOverride(const MObject& obj) : MHWRender::MPxDrawOverride(obj, NULL)
{
}

MHWRender::MPxDrawOverride* PistonBreakLocatorDrawOverride::create(const MObject& obj)
{
    return new PistonBreakLocatorDrawOverride(obj);
}

MHWRender::DrawAPI PistonBreakLocatorDrawOverride::supportedDrawAPIs() const
{
    return (MHWRender::kOpenGL | MHWRender::kOpenGLCoreProfile | MHWRender::kDirectX11);
}

bool PistonBreakLocatorDrawOverride::isBounded(const MDagPath& objPath, const MDagPath& cameraPath) const
{
    return false;
}

MBoundingBox PistonBreakLocatorDrawOverride::boundingBox(const MDagPath& objPath, const MDagPath& cameraPath) const
{
    MPoint corner1(-10., -1.0, -1.0);
    MPoint corner2( 1.0,  1.0,  1.0);

    return MBoundingBox(corner1, corner2);
}

MUserData* PistonBreakLocatorDrawOverride::prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* oldData)
{
    PistonBreakLocatorData* data = dynamic_cast<PistonBreakLocatorData*>(oldData);

    if (!data)
    {
        data = new PistonBreakLocatorData();
    }

    MObject obj = objPath.node();

    data->draw     = MPlug(obj, PistonBreakLocator::drawAttr).asBool();
    data->isBroken = MPlug(obj, PistonBreakLocator::isBrokenAttr).asBool();
    data->radius   = MPlug(obj, PistonBreakLocator::radiusAttr).asDouble();

    return data;
}

void PistonBreakLocatorDrawOverride::addUIDrawables(const MDagPath &objPath, MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext, const MUserData* data)
{
    PistonBreakLocatorData* pistonData = (PistonBreakLocatorData*) data;

    if (!pistonData) 
    {
        return;
    }

    drawManager.beginDrawable();

    if (pistonData->draw && pistonData->isBroken)
    {
        MColor red(1.0f, 0.0, 0.0);
        drawManager.setColor(red);
        drawManager.sphere(MPoint::origin, pistonData->radius, true);
    }

    drawManager.endDrawable();
}

#endif