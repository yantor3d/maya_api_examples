#ifndef YANTOR_3D_PISTON_BREAK_VP2
#define YANTOR_3D_PISTON_BREAK_VP2

#include "pistonBreakLocator.h"

#include <maya/MBoundingBox.h>
#include <maya/MDagPath.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>

#if MAYA_API_VERSION >= 201600
#include <maya/MDrawContext.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>

class PistonBreakLocatorData : public MUserData
{
public:
    PistonBreakLocatorData() : MUserData(false) {};
    virtual ~PistonBreakLocatorData() {}

public:
    bool draw;
    bool isBroken;    
    double radius;
};

class PistonBreakLocatorDrawOverride : public MHWRender::MPxDrawOverride
{
public:
    static MHWRender::MPxDrawOverride* create(const MObject& obj);

    virtual MHWRender::DrawAPI supportedDrawAPIs() const;

    virtual bool isBounded(const MDagPath &objPath, const MDagPath& cameraPath) const;
    virtual MBoundingBox boundingBox(const MDagPath &objPath, const MDagPath& cameraPath) const;

    virtual bool hasUIDrawables() const { return true; }
    virtual MUserData* prepareForDraw(const MDagPath& objPath, const MDagPath& cameraPath, const MHWRender::MFrameContext& frameContext, MUserData* oldData);
    virtual void addUIDrawables(const MDagPath &objPath, MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext, const MUserData* data);

private:
    PistonBreakLocatorDrawOverride(const MObject &obj);
};

#endif 

#endif 