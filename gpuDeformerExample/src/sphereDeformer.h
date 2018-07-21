#ifndef YANTOR_3D_SPHERE_DEFORMER_H
#define YANTOR_3D_SPHERE_DEFORMER_H

#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MStatus.h>
#include <maya/MTypeId.h>

#if MAYA_API_VERSION >= 201600
    #include <clew/clew_cl.h>
    #include <maya/MOpenCLAutoPtr.h>
    #include <maya/MEvaluationNode.h>
    #include <maya/MGPUDeformerRegistry.h>
    #include <maya/MOpenCLInfo.h>
    #include <maya/MPxGPUDeformer.h>
    #include <maya/MPlug.h>
    #include <maya/MStringArray.h>
#endif 

class SphereDeformer : public MPxDeformerNode
{
public:
                        SphereDeformer();
    virtual             ~SphereDeformer();

    static void*        create();
    static MStatus      initialize();

    virtual MStatus     deform(MDataBlock& dataBlock, MItGeometry& iterGeo, const MMatrix& worldMatrix, unsigned int geoIndex);

    static MMatrix      getSphereMatrix(MDataBlock& dataBlock, MStatus* ReturnStatus=NULL);

public:
    static MString nodeTypeName;
    static MTypeId nodeTypeId;

    static MObject sphereMatrixAttr;
};

#if MAYA_API_VERSION >= 201600
class SphereDeformerGPU : public MPxGPUDeformer
{
public:
    SphereDeformerGPU();
    ~SphereDeformerGPU();
    
    virtual MPxGPUDeformer::DeformerStatus evaluate(
        MDataBlock&            dataBlock,
        const MEvaluationNode& node,
        const MPlug&           plug,
        unsigned int           numElements,
        const MAutoCLMem       inputPositionBuffer,
        const MAutoCLEvent     inputEvent,
        MAutoCLMem             outputPositionBuffer,
        MAutoCLEvent&          outputEvent
    );

    static MGPUDeformerRegistrationInfo* getGPUDeformerInfo();
    virtual void terminate();
    
private:
    MStatus getSphereMatrixBuffer(MDataBlock& dataBlock, const MEvaluationNode& evaluationNode);
    MStatus getWeightsBuffer(MDataBlock& dataBlock, const MEvaluationNode& evaluationNode, const MPlug& plug, unsigned int numElements);
    static cl_int enqueueBuffer(MAutoCLMem &clBuffer, size_t bufferSize, void* data);
    
public:
    static MString nodeTypeName;
    static MString pluginPath;

private:
    MAutoCLKernel kernel;

    MAutoCLMem    sphereMatrixBuffer;
    MAutoCLMem    sphereInverseMatrixBuffer;
    MAutoCLMem    weightsBuffer;
};


class SphereDeformerGPUInfo : public MGPUDeformerRegistrationInfo
{
public:
	         SphereDeformerGPUInfo() {}
	virtual ~SphereDeformerGPUInfo() {}

    virtual MPxGPUDeformer* createGPUDeformer();

    virtual bool validateNode(MDataBlock& block, const MEvaluationNode& evaluationNode, const MPlug& plug, MStringArray* messages) 
    {
        return true;  
    }
};
#endif

#endif