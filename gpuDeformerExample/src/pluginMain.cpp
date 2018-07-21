/**
    Copyright (c) 2018 Ryan Porter    
    You may use, distribute, or modify this code under the terms of the MIT license.
*/

#include "sphereDeformer.h"

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MGPUDeformerRegistry.h>
#include <maya/MObject.h>
#include <maya/MPxNode.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MTypeId.h>


const char* AUTHOR = "Ryan Porter";
const char* VERSION = "0.0.1";
const char* REQUIRED_API_VERSION = "Any";

MString SphereDeformer::nodeTypeName = "sphereDeformer";
MTypeId SphereDeformer::nodeTypeId   = 0x80000001;

#if MAYA_API_VERSION >= 201600
    MString SphereDeformerGPU::nodeTypeName = "sphereDeformerGPU";
    MString SphereDeformerGPU::pluginPath;
#endif

MStatus initializePlugin(MObject pluginObj)
{
    MStatus status;

    MFnPlugin fnPlugin(
        pluginObj, 
        AUTHOR, 
        VERSION, 
        REQUIRED_API_VERSION, 
        &status
    );

    status = fnPlugin.registerNode(
        SphereDeformer::nodeTypeName,
        SphereDeformer::nodeTypeId,
        SphereDeformer::create,
        SphereDeformer::initialize,
        MPxNode::Type::kDeformerNode
    );

    #if MAYA_API_VERSION >= 201600
        SphereDeformerGPU::pluginPath = fnPlugin.loadPath();

        status = MGPUDeformerRegistry::registerGPUDeformerCreator(
            SphereDeformer::nodeTypeName,
            SphereDeformerGPU::nodeTypeName,
            SphereDeformerGPU::getGPUDeformerInfo()
        );

        MGlobal::displayInfo("Registered GPU Deformer creator for '" + SphereDeformerGPU::nodeTypeName + "'.");
        MGlobal::displayInfo("Plugin path: " + SphereDeformerGPU::pluginPath);
    #endif

    return status;
}

MStatus uninitializePlugin(MObject pluginObj)
{
    MStatus status;

    MFnPlugin fnPlugin(
        pluginObj, 
        AUTHOR, 
        VERSION, 
        REQUIRED_API_VERSION, 
        &status
    );

    #if MAYA_API_VERSION >= 201600
        status = MGPUDeformerRegistry::deregisterGPUDeformerCreator(
            SphereDeformer::nodeTypeName,
            SphereDeformerGPU::nodeTypeName
        );
    #endif

    status = fnPlugin.deregisterNode(SphereDeformer::nodeTypeId);

    return status;
}