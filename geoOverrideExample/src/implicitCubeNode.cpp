#include "implicitCubeNode.h"

#include <maya/MBoundingBox.h>
#include <maya/MDataBlock.h>
#include <maya/MGL.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPoint.h>
#include <maya/MStatus.h>

void* ImplicitCubeNode::create()
{
    return new ImplicitCubeNode();
}

MStatus ImplicitCubeNode::initialize()
{
    return MStatus::kSuccess;
}

MStatus ImplicitCubeNode::compute(const MPlug& plug, MDataBlock& dataBlock)
{
    return MStatus::kSuccess;
}

bool ImplicitCubeNode::isBounded() const
{ 
    return true;
}

MBoundingBox ImplicitCubeNode::boundingBox() const 
{
    return MBoundingBox(
        MPoint(-1.0, -1.0, -1.0),
        MPoint( 1.0,  1.0,  1.0)
    );
}

void ImplicitCubeNode::draw(M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status)
{
    view.beginGL();

    glBegin(GL_POLYGON);
    glVertex3f(  1.0, -1.0, -1.0 );
    glVertex3f(  1.0,  1.0, -1.0 );
    glVertex3f( -1.0,  1.0, -1.0 );
    glVertex3f( -1.0, -1.0, -1.0 );
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(  1.0, -1.0, 1.0 );
    glVertex3f(  1.0,  1.0, 1.0 );
    glVertex3f( -1.0,  1.0, 1.0 );
    glVertex3f( -1.0, -1.0, 1.0 );
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f( 1.0, -1.0, -1.0 );
    glVertex3f( 1.0,  1.0, -1.0 );
    glVertex3f( 1.0,  1.0,  1.0 );
    glVertex3f( 1.0, -1.0,  1.0 );
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f( -1.0, -1.0,  1.0 );
    glVertex3f( -1.0,  1.0,  1.0 );
    glVertex3f( -1.0,  1.0, -1.0 );
    glVertex3f( -1.0, -1.0, -1.0 );
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(  1.0,  1.0,  1.0 );
    glVertex3f(  1.0,  1.0, -1.0 );
    glVertex3f( -1.0,  1.0, -1.0 );
    glVertex3f( -1.0,  1.0,  1.0 );
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(  1.0, -1.0, -1.0 );
    glVertex3f(  1.0, -1.0,  1.0 );
    glVertex3f( -1.0, -1.0,  1.0 );
    glVertex3f( -1.0, -1.0, -1.0 );
    glEnd();

    view.endGL();
}