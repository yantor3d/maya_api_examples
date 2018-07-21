#include "implicitCubeGeometryOverride.h"

#include <maya/MBoundingBox.h>
#include <maya/MColor.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>

#include <maya/MStringArray.h>

#if MAYA_API_VERSION >= 201600
#include <maya/MFrameContext.h>
#include <maya/MGeometryRequirements.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MPxGeometryOverride.h>
#include <maya/MShaderManager.h>
#include <maya/MUIDrawManager.h>

static float POINTS[][3] = {
    {-1.0, -1.0,  1.0},
    { 1.0, -1.0,  1.0},
    {-1.0,  1.0,  1.0},
    { 1.0,  1.0,  1.0},
    {-1.0,  1.0, -1.0},
    { 1.0,  1.0, -1.0},
    {-1.0, -1.0, -1.0},
    { 1.0, -1.0, -1.0}
};

static unsigned int EDGES[][2] = {
    {0, 1},
    {2, 3},
    {4, 5},
    {6, 7},
    {0, 2},
    {1, 3},
    {2, 4},
    {3, 5},
    {4, 6},
    {5, 7},
    {6, 0},
    {7, 1}
};

static unsigned int TRIANGLES[][3] = {
    {2, 1, 0},
    {2, 1, 3},
    {2, 3, 4},
    {4, 3, 5},
    {4, 5, 6},
    {6, 5, 7},
    {6, 7, 0},
    {0, 7, 1},
    {1, 7, 3},
    {3, 7, 5},
    {6, 0, 4},
    {4, 0, 2}
};

ImplicitCubeGeometryOverride::ImplicitCubeGeometryOverride(const MObject& obj) : MHWRender::MPxGeometryOverride(obj)
{
}

MHWRender::MPxGeometryOverride* ImplicitCubeGeometryOverride::create(const MObject& obj)
{
    return new ImplicitCubeGeometryOverride(obj);
}

ImplicitCubeGeometryOverride::~ImplicitCubeGeometryOverride() 
{

}

MHWRender::DrawAPI ImplicitCubeGeometryOverride::supportedDrawAPIs() const
{
    return (MHWRender::kOpenGL | MHWRender::kDirectX11 | MHWRender::kOpenGLCoreProfile);
}


void ImplicitCubeGeometryOverride::updateDG()
{
}

void ImplicitCubeGeometryOverride::updateRenderItems(const MDagPath& path, MHWRender::MRenderItemList& renderItems)
{
    MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();

    if (!renderer)
    {
        return;
    }

    const MHWRender::MShaderManager* shaderManager = renderer->getShaderManager();

    if (!shaderManager)
    {
        return;
    }

    updateWireframeRenderItem(path, renderItems);
    updateShadedRenderItem(path, renderItems);
}

void ImplicitCubeGeometryOverride::updateWireframeRenderItem(const MDagPath& path, MHWRender::MRenderItemList& renderItems)
{
    MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
    const MHWRender::MShaderManager* shaderManager = renderer->getShaderManager();

    MHWRender::MRenderItem* renderItem = NULL;
    MString renderItemName = "implicitCubeWireframe";
    
    int index = renderItems.indexOf(renderItemName);

    if (-1 == index)
    {
        renderItem = MHWRender::MRenderItem::Create(
            renderItemName,
            MHWRender::MRenderItem::DecorationItem,
            MHWRender::MGeometry::kLines
        );

        renderItem->setDrawMode(MHWRender::MGeometry::kWireframe);
        renderItem->depthPriority(MHWRender::MRenderItem::sActiveLineDepthPriority);

        MHWRender::MShaderInstance* shader = shaderManager->getStockShader(MHWRender::MShaderManager::k3dSolidShader);

        if (shader)
        {
            renderItem->setShader(shader);
            shaderManager->releaseShader(shader);
        }

        renderItems.append(renderItem);
    } else {
        renderItem = renderItems.itemAt(index);        
    }

    if (renderItem)
    {
        MHWRender::MShaderInstance* shader = renderItem->getShader();

        if (shader)
        {
            MColor c = MHWRender::MGeometryUtilities::wireframeColor(path);
            float color[] = { c.r, c.g, c.b, 1.0f };
            shader->setParameter("solidColor", color);
        }

        renderItem->enable(true);
    }
}

void ImplicitCubeGeometryOverride::updateShadedRenderItem(const MDagPath& path, MHWRender::MRenderItemList& renderItems)
{
    MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
    const MHWRender::MShaderManager* shaderManager = renderer->getShaderManager();

    MHWRender::MRenderItem* renderItem = NULL;
    MString renderItemName = "implicitCubeShaded";
    
    int index = renderItems.indexOf(renderItemName);

    if (-1 == index)
    {
        renderItem = MHWRender::MRenderItem::Create(
            renderItemName,
            MHWRender::MRenderItem::NonMaterialSceneItem,
            MHWRender::MGeometry::kTriangles
        );

        int drawMode = MHWRender::MGeometry::kShaded | MHWRender::MGeometry::kTextured;

        renderItem->setDrawMode((MHWRender::MGeometry::DrawMode) drawMode);
        renderItem->depthPriority(MHWRender::MRenderItem::sDormantFilledDepthPriority);

        MHWRender::MShaderInstance* shader = shaderManager->getStockShader(MHWRender::MShaderManager::k3dSolidShader);

        if (shader)
        {
            renderItem->setShader(shader);
            shaderManager->releaseShader(shader);
        }

        renderItems.append(renderItem);
    } else {
        renderItem = renderItems.itemAt(index);
    }

    if (renderItem)
    {
        MHWRender::MShaderInstance* shader = renderItem->getShader();

        if (shader)
        {
            MColor c = MHWRender::MGeometryUtilities::wireframeColor(path);
            float color[] = { c.r, c.g, c.b, 0.5f };
            shader->setParameter("solidColor", color);
        }

        renderItem->enable(true);
    }
}

void ImplicitCubeGeometryOverride::populateGeometry(const MHWRender::MGeometryRequirements& requirements, const MHWRender::MRenderItemList& renderItems, MHWRender::MGeometry& geo)
{
    fillVertexBuffers(requirements, geo);
    fillIndexBuffers(renderItems, geo);
}

void ImplicitCubeGeometryOverride::fillVertexBuffers(const MHWRender::MGeometryRequirements& requirements, MHWRender::MGeometry& geo)
{
    const MHWRender::MVertexBufferDescriptorList& descList = requirements.vertexRequirements();

    for (int i = 0; i < descList.length(); i++)
    {
        MHWRender::MVertexBufferDescriptor desc;

        if (!descList.getDescriptor(i, desc))
        {
            continue;
        }
         
        switch (desc.semantic())
        {
            case MHWRender::MGeometry::kPosition:
                MHWRender::MVertexBuffer* vertexBuffer = geo.createVertexBuffer(desc);

                if (vertexBuffer)
                {
                    unsigned int vertexCount = 8;
                    bool writeOnly = true;

                    float* vertices = (float*) vertexBuffer->acquire(vertexCount, writeOnly);

                    if (vertices)
                    {
                        for (unsigned int i = 0; i < vertexCount; i++)
                        {
                            vertices[i*3 + 0] = POINTS[i][0];
                            vertices[i*3 + 1] = POINTS[i][1];
                            vertices[i*3 + 2] = POINTS[i][2];
                        }

                        vertexBuffer->commit(vertices);
                    }
                }
            break;
        }
    }
}

void ImplicitCubeGeometryOverride::fillIndexBuffers(const MHWRender::MRenderItemList& renderItems, MHWRender::MGeometry& geo)
{
    for (int i = 0; i < renderItems.length(); i++)
    {
        const MHWRender::MRenderItem* renderItem = renderItems.itemAt(i);

        if (!renderItem)
        {
            continue;
        }

        MHWRender::MIndexBuffer* indexBuffer = geo.createIndexBuffer(MHWRender::MGeometry::kUnsignedInt32);

        if (!indexBuffer) 
        {
            continue;
        }

        if (renderItem->primitive() == MHWRender::MGeometry::kTriangles)
        {
            unsigned int numTriangles = 12;
            bool writeOnly = true;

            unsigned int* triangles = (unsigned int*) indexBuffer->acquire(3 * numTriangles, writeOnly);

            if (triangles)
            {
                for (unsigned int i = 0; i < numTriangles; i++)
                {
                    triangles[i*3 + 0] = TRIANGLES[i][0];
                    triangles[i*3 + 1] = TRIANGLES[i][1];
                    triangles[i*3 + 2] = TRIANGLES[i][2];
                }

                indexBuffer->commit(triangles);
            }
        }
        else if (renderItem->primitive() == MHWRender::MGeometry::kLines)
        {
            unsigned int numEdges = 12;
            bool writeOnly = true;

            unsigned int* edges = (unsigned int*) indexBuffer->acquire(2 * numEdges, writeOnly);

            if (edges)
            {
                for (unsigned int i = 0; i < numEdges; i++)
                {
                    edges[i*2 + 0] = EDGES[i][0];
                    edges[i*2 + 1] = EDGES[i][1];
                }

                indexBuffer->commit(edges);
            }
        }

        renderItem->associateWithIndexBuffer(indexBuffer);
    }
}

void ImplicitCubeGeometryOverride::cleanUp()
{
}

#endif