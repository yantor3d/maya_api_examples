#ifndef YANTOR_3D_PISTON_BREAK_LOCATOR
#define YANTOR_3D_PISTON_BREAK_LOCATOR

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

class PistonBreakLocator : public MPxLocatorNode
{
public:
             PistonBreakLocator();
    virtual ~PistonBreakLocator();

    static void*   create();
    static MStatus initialize();

    virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock);

    virtual bool isBounded() const;
    virtual MBoundingBox boundingBox() const;
    virtual void draw(M3dView& view, const MDagPath &path, M3dView::DisplayStyle style, M3dView::DisplayStatus displayStatus);

public:
    static MString nodeTypeName;
    static MTypeId nodeTypeId;
    
    static MString drawClassification;
    static MString drawRegistrantId;

    // Input Attributes
    static MObject startMatrixAttr;
    static MObject endMatrixAttr;
    static MObject minLengthAttr;
    static MObject maxLengthAttr;

    // Draw Attributes
    static MObject drawAttr; 
    static MObject radiusAttr;

    // Output Attributes
    static MObject lengthAttr;
    static MObject isBrokenAttr;
};

#endif
