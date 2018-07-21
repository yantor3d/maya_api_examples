#include "implicitCubeNode.h"
#include "implicitCubeGeometryOverride.h"

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
const char* PLUGIN_VERSION = "1.0.0";

MString ImplicitCubeNode::nodeTypeName = "implicitCube";
MTypeId ImplicitCubeNode::nodeTypeId = 0x80000001;
MString ImplicitCubeNode::drawClassification = "drawdb/geometry/implicitCube";
MString ImplicitCubeNode::drawRegistrantId = "ImplicitCubeNodePlugin";

MStatus initializePlugin(MObject pluginObj)
{
    MStatus status;

    MFnPlugin fnPlugin(pluginObj, AUTHOR, PLUGIN_VERSION, "Any");

    status = fnPlugin.registerNode(
        ImplicitCubeNode::nodeTypeName,
        ImplicitCubeNode::nodeTypeId,
        ImplicitCubeNode::create,
        ImplicitCubeNode::initialize,
        MPxNode::Type::kLocatorNode,
        &ImplicitCubeNode::drawClassification
    );

    CHECK_MSTATUS_AND_RETURN_IT(status);

#if MAYA_API_VERSION >= 201600
    status = MHWRender::MDrawRegistry::registerGeometryOverrideCreator(
        ImplicitCubeNode::drawClassification,
        ImplicitCubeNode::drawRegistrantId,
        ImplicitCubeGeometryOverride::create
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
    status = MHWRender::MDrawRegistry::deregisterGeometryOverrideCreator(
        ImplicitCubeNode::drawClassification,
        ImplicitCubeNode::drawRegistrantId
    );

    CHECK_MSTATUS_AND_RETURN_IT(status);
#endif 

    status = fnPlugin.deregisterNode(ImplicitCubeNode::nodeTypeId);

    return status;
}
