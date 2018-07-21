#include "pistonBreakLocator.h"
#include "pistonBreakShape_VP2.h"

#include <maya/MFnPlugin.h>
#include <maya/MObject.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#if MAYA_API_VERSION >= 201600
    #include <maya/MDrawRegistry.h>
#endif 

const char* AUTHOR         = "Ryan Porter";
const char* PLUGIN_VERSION = "0.1.0";

MString PistonBreakLocator::nodeTypeName = "pistonBreak";
MTypeId PistonBreakLocator::nodeTypeId = 0x80000001;
MString PistonBreakLocator::drawClassification("drawdb/geometry/pistonBreak");
MString PistonBreakLocator::drawRegistrantId("PistonBreakNodePlugin");

MStatus initializePlugin(MObject pluginObj)
{
    MStatus status;

    MFnPlugin fnPlugin(pluginObj, AUTHOR, PLUGIN_VERSION, "Any");

    MString* classification = &PistonBreakLocator::drawClassification;

    status = fnPlugin.registerNode(
        PistonBreakLocator::nodeTypeName,
        PistonBreakLocator::nodeTypeId,
        PistonBreakLocator::create,
        PistonBreakLocator::initialize,
        MPxNode::Type::kLocatorNode,
        classification
    );

    CHECK_MSTATUS_AND_RETURN_IT(status);

#if MAYA_API_VERSION >= 201600
    status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
        PistonBreakLocator::drawClassification,
        PistonBreakLocator::drawRegistrantId,
        PistonBreakLocatorDrawOverride::create
    );

    CHECK_MSTATUS_AND_RETURN_IT(status);
#endif 

    return status;
}


MStatus uninitializePlugin(MObject pluginObj)
{
    MStatus status;

    MFnPlugin fnPlugin(pluginObj, AUTHOR, PLUGIN_VERSION,  "Any");

#if MAYA_API_VERSION >= 201600
    status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(
        PistonBreakLocator::drawClassification,
        PistonBreakLocator::drawRegistrantId
    );

    CHECK_MSTATUS_AND_RETURN_IT(status);
#endif 

    status = fnPlugin.deregisterNode(PistonBreakLocator::nodeTypeId);

    return status;
}
