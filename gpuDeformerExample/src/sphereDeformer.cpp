#include "sphereDeformer.h"

#include <stdio.h>

#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MGlobal.h>
#include <maya/MItGeometry.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>
#include <maya/MVector.h>
#include <maya/MStringArray.h>

#if MAYA_API_VERSION >= 201600
    #include <clew/clew_cl.h>
    #include <maya/MOpenCLAutoPtr.h>
    #include <maya/MEvaluationNode.h>
    #include <maya/MGPUDeformerRegistry.h>
    #include <maya/MOpenCLInfo.h>
    #include <maya/MPxGPUDeformer.h>
#endif 


MObject SphereDeformer::sphereMatrixAttr;


SphereDeformer::SphereDeformer()
{

}


SphereDeformer::~SphereDeformer()
{

}


void* SphereDeformer::create()
{
    return new SphereDeformer();
}


MStatus SphereDeformer::initialize()
{
    MStatus status;

    MFnTypedAttribute t;

    sphereMatrixAttr = t.create("sphereMatrix", "m", MFnData::kMatrix, MObject::kNullObj, &status);

    status = addAttribute(sphereMatrixAttr);
    status = attributeAffects(sphereMatrixAttr, outputGeom);

    MGlobal::executeCommand("makePaintable -attrType multiFloat -sm deformer " + SphereDeformer::nodeTypeName + " weights");

    return status;
}


MStatus SphereDeformer::deform(
    MDataBlock& dataBlock, 
    MItGeometry& iterGeo, 
    const MMatrix& worldMatrix, 
    unsigned int geoIndex
) {
    MStatus status;

    MMatrix sphereMatrix = getSphereMatrix(dataBlock, &status);
    MMatrix sphereInverseMatrix = sphereMatrix.inverse();
    MMatrix worldInverseMatrix = worldMatrix.inverse();

    MPointArray points;
    iterGeo.allPositions(points, MSpace::Space::kObject);

    unsigned int numberOfPoints = points.length();
    float envelope_ = dataBlock.inputValue(envelope).asFloat();

    for (unsigned int i = 0; i < numberOfPoints; i++)
    {
        MPoint inPoint = points[i];

        MVector v(inPoint * worldInverseMatrix * sphereInverseMatrix);

        float vlen = (float) v.length();

        float weight = this->weightValue(dataBlock, geoIndex, i) * ((float) vlen < 1.0);
        MPoint deformedPoint = MPoint(v / vlen) * sphereMatrix * worldInverseMatrix;

        MVector delta = (deformedPoint - inPoint) * (envelope_ * weight);

        MPoint p(points[i] + delta);
        points.set(p, i);
    }

    iterGeo.setAllPositions(points, MSpace::Space::kObject);

    return status;
}


MMatrix SphereDeformer::getSphereMatrix(MDataBlock& dataBlock, MStatus* ReturnStatus)
{
    MMatrix sphereMatrix;
    MStatus status;

    MDataHandle   sphereMatrixHandle = dataBlock.inputValue(SphereDeformer::sphereMatrixAttr);
    MObject       sphereMatrixObj = sphereMatrixHandle.data();
    MFnMatrixData sphereMatrixData(sphereMatrixObj);

    sphereMatrix = sphereMatrixData.matrix();

    if (ReturnStatus != NULL)
    {
        ReturnStatus = &status;
    }

    return sphereMatrix;
}


#if MAYA_API_VERSION >= 201600
#include <clew/clew_cl.h>
#include <maya/MOpenCLAutoPtr.h>
#include <maya/MEvaluationNode.h>
#include <maya/MGPUDeformerRegistry.h>
#include <maya/MOpenCLInfo.h>
#include <maya/MPxGPUDeformer.h>

SphereDeformerGPU::SphereDeformerGPU()
{
}


SphereDeformerGPU::~SphereDeformerGPU()
{
    this->terminate();
}


MPxGPUDeformer::DeformerStatus SphereDeformerGPU::evaluate(
    MDataBlock&            dataBlock,
    const MEvaluationNode& node,
    const MPlug&           plug,
    unsigned int           numElements,
    const MAutoCLMem       inputPositionBuffer,
    const MAutoCLEvent     inputEvent,
    MAutoCLMem             outputPositionBuffer,
    MAutoCLEvent&          outputEvent
)
{
    MStatus status;

    status = getSphereMatrixBuffer(dataBlock, node);
    status = getWeightsBuffer(dataBlock, node, plug, numElements);

    if (!kernel.get())
    {
        MString clKernelFilePath(pluginPath);
        clKernelFilePath += "/sphereDeformer.cl";

        kernel = MOpenCLInfo::getOpenCLKernel(clKernelFilePath, "sphereDeformer");

        if (kernel.isNull())
        {
            MGlobal::displayError("Could not compile kernal " + clKernelFilePath);
            return MPxGPUDeformer::kDeformerFailure;
        }
    }

    cl_int err = CL_SUCCESS;
    unsigned int parameterId = 0;

    err = clSetKernelArg(kernel.get(), parameterId++, sizeof(cl_uint), (void*)&numElements);
    MOpenCLInfo::checkCLErrorStatus(err);

    err = clSetKernelArg(kernel.get(), parameterId++, sizeof(cl_mem), (void*)inputPositionBuffer.getReadOnlyRef());
    MOpenCLInfo::checkCLErrorStatus(err);

    err = clSetKernelArg(kernel.get(), parameterId++, sizeof(cl_mem), (void*)sphereMatrixBuffer.getReadOnlyRef());
    MOpenCLInfo::checkCLErrorStatus(err);

    err = clSetKernelArg(kernel.get(), parameterId++, sizeof(cl_mem), (void*)sphereInverseMatrixBuffer.getReadOnlyRef());
    MOpenCLInfo::checkCLErrorStatus(err);

    err = clSetKernelArg(kernel.get(), parameterId++, sizeof(cl_mem), (void*)weightsBuffer.getReadOnlyRef());
    MOpenCLInfo::checkCLErrorStatus(err);

    err = clSetKernelArg(kernel.get(), parameterId++, sizeof(cl_mem), (void*)outputPositionBuffer.getReadOnlyRef());
    MOpenCLInfo::checkCLErrorStatus(err);

    size_t workGroupSize;
    size_t retSize;

    err = clGetKernelWorkGroupInfo(
        kernel.get(),
        MOpenCLInfo::getOpenCLDeviceId(),
        CL_KERNEL_WORK_GROUP_SIZE,
        sizeof(size_t),
        &workGroupSize,
        &retSize
    );
    MOpenCLInfo::checkCLErrorStatus(err);

    size_t localWorkSize = retSize > 0 ? workGroupSize : 256;
    size_t globalWorkSize = (localWorkSize - numElements % localWorkSize) + numElements;

    unsigned int numInputEvents = inputEvent.get() ? 1 : 0;

    err = clEnqueueNDRangeKernel(
            MOpenCLInfo::getOpenCLCommandQueue(),
            kernel.get(),
            1,
            NULL,
            &globalWorkSize,
            &localWorkSize,
            numInputEvents,
            numInputEvents ? inputEvent.getReadOnlyRef() : 0,
            outputEvent.getReferenceForAssignment()
    );
    MOpenCLInfo::checkCLErrorStatus(err);

    return MPxGPUDeformer::kDeformerSuccess;
}


MStatus SphereDeformerGPU::getSphereMatrixBuffer(MDataBlock& dataBlock, const MEvaluationNode& evaluationNode)
{
    MStatus status;

    if (
        sphereMatrixBuffer.get() 
        && !evaluationNode.dirtyPlugExists(SphereDeformer::sphereMatrixAttr)
    ) {
        return MStatus::kSuccess;
    }

    MMatrix mat = SphereDeformer::getSphereMatrix(dataBlock, &status);
    MMatrix sm  = mat.transpose();
    MMatrix sim = mat.transpose().inverse();

    unsigned int n = 16;
    float* sphereMatrix = new float[n];
    float* sphereInverseMatrix = new float[n];

    unsigned int i = 0;

    for (unsigned int row = 0; row < 4; row++)
    {
        for (unsigned int col = 0; col < 4; col++)
        {
            sphereMatrix[i++] = (float) sm(row, col);
        }
    }

    i = 0;

    for (unsigned int row = 0; row < 4; row++)
    {
        for (unsigned int col = 0; col < 4; col++)
        {
            sphereInverseMatrix[i++] = (float) sim(row, col);
        }
    }

    enqueueBuffer(
        sphereMatrixBuffer,
        n * sizeof(float), 
        (void*) sphereMatrix
    );
    
    enqueueBuffer(
        sphereInverseMatrixBuffer,
        n * sizeof(float), 
        (void*) sphereInverseMatrix
    );

    delete[] sphereMatrix;
    delete[] sphereInverseMatrix;

    return status;
}


MStatus SphereDeformerGPU::getWeightsBuffer(MDataBlock& dataBlock, const MEvaluationNode& evaluationNode, const MPlug& plug, unsigned int numElements)
{
    MStatus status;

    if (
        (weightsBuffer.get() && !evaluationNode.dirtyPlugExists(SphereDeformer::weightList, &status)) || !status
    ) {
        return MStatus::kSuccess;
    }

    MGlobal::displayInfo("Getting weights...");

    float* weights = new float[numElements];
    for (unsigned int i = 0; i < numElements; i++)
    {
        weights[i] = 1.0f;
    }

    MArrayDataHandle weightListArray = dataBlock.outputArrayValue(SphereDeformer::weightList, &status);

    unsigned int geomIndex = plug.logicalIndex();

    status = weightListArray.jumpToElement(geomIndex);

    if (status)
    {
        MDataHandle weightListItem = weightListArray.inputValue(&status);
        MArrayDataHandle weightsArray = weightListItem.child(SphereDeformer::weights);

        unsigned int numWeights = weightsArray.elementCount(&status);

        for (unsigned i = 0; i < numWeights; i++, weightsArray.next())
        {
            unsigned int idx = weightsArray.elementIndex(&status);
            MDataHandle weight = weightsArray.inputValue(&status);
            weights[idx] = weight.asFloat();
        }
    }

    enqueueBuffer(weightsBuffer, numElements * sizeof(float), (void*) weights);

    delete[] weights;

    return MStatus::kSuccess;
}

void SphereDeformerGPU::terminate()
{
    sphereMatrixBuffer.reset();
    sphereInverseMatrixBuffer.reset();
    weightsBuffer.reset();

    MOpenCLInfo::releaseOpenCLKernel(kernel);
    kernel.reset();
}


MGPUDeformerRegistrationInfo* SphereDeformerGPU::getGPUDeformerInfo()
{
    static SphereDeformerGPUInfo info;
    return &info;
}


MPxGPUDeformer* SphereDeformerGPUInfo::createGPUDeformer()
{
    return new SphereDeformerGPU();
}   


cl_int SphereDeformerGPU::enqueueBuffer(MAutoCLMem &clBuffer, size_t bufferSize, void* data)
{
    cl_int err = CL_SUCCESS;

    if (!clBuffer.get())
    {
        clBuffer.attach(
            clCreateBuffer(
                MOpenCLInfo::getOpenCLContext(),
                CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY,
                bufferSize,
                data,
                &err
            )
        );
    } else {
        err = clEnqueueWriteBuffer(
            MOpenCLInfo::getOpenCLCommandQueue(),
            clBuffer.get(),
            CL_TRUE,
            0,
            bufferSize,
            data,
            0,
            NULL, 
            NULL
        );
    }

    return err;
}
#endif