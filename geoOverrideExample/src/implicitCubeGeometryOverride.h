#ifndef YANTOR_3D_IMPLICIT_CUBE_GEOMETRY_OVERRIDE
#define YANTOR_3D_IMPLICIT_CUBE_GEOMETRY_OVERRIDE

#include <stdio.h>

#include <maya/MBoundingBox.h>
#include <maya/MColor.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MStatus.h>

#if MAYA_API_VERSION >= 201600
#include <maya/MFrameContext.h>
#include <maya/MGeometryRequirements.h>
#include <maya/MHWGeometry.h>
#include <maya/MPxGeometryOverride.h>
#include <maya/MUIDrawManager.h>

class ImplicitCubeGeometryOverride : public MHWRender::MPxGeometryOverride
{
public:
    static MHWRender::MPxGeometryOverride* create(const MObject& obj);

    virtual ~ImplicitCubeGeometryOverride();

    virtual MHWRender::DrawAPI ImplicitCubeGeometryOverride::supportedDrawAPIs() const;

    virtual void updateDG();
    virtual void updateRenderItems(const MDagPath& path, MHWRender::MRenderItemList& renderItems);
    virtual void updateWireframeRenderItem(const MDagPath& path, MHWRender::MRenderItemList& renderItems);
    virtual void updateShadedRenderItem(const MDagPath& path, MHWRender::MRenderItemList& renderItems);

    virtual void populateGeometry(const MHWRender::MGeometryRequirements& requirements, const MHWRender::MRenderItemList& renderItems, MHWRender::MGeometry& geo);

    virtual void fillVertexBuffers(const MHWRender::MGeometryRequirements& requirements, MHWRender::MGeometry& geo);
    virtual void fillIndexBuffers(const MHWRender::MRenderItemList& renderItems, MHWRender::MGeometry& geo);

    virtual void cleanUp();

private:
    ImplicitCubeGeometryOverride(const MObject& obj);
};

#endif 

#endif 