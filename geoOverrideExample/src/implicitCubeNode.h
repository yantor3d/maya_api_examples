#ifndef YANTOR_3D_IMPLICIT_CUBE_NODE
#define YANTOR_3D_IMPLICIT_CUBE_NODE

#include <maya/M3dView.h>
#include <maya/MBoundingBox.h>
#include <maya/MDagPath.h>
#include <maya/MDataBlock.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MTypeId.h>

class ImplicitCubeNode : public MPxLocatorNode
{
public:
    static void*   create();
    static MStatus initialize();

    virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock);

    virtual bool isBounded() const;
    virtual MBoundingBox boundingBox() const;

    virtual void draw(M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status);

public:
    static MString nodeTypeName;
    static MTypeId nodeTypeId;
    
    static MString drawClassification;
    static MString drawRegistrantId;
};

#endif
