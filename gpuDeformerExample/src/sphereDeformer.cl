__kernel void sphereDeformer(
    const uint       numPoints,
    __global float*  inPoints,
    __global float4* sphereMatrix,
    __global float4* sphereInverseMatrix,
    __global float*  weights,
    __global float*  outPoints
)
{
    unsigned int pointIndex = get_global_id(0);

    if (pointIndex >= numPoints) {
        return;          
    }

    unsigned int pid = pointIndex * 3;

    float wt = weights[pointIndex];
    
    float4 inPoint;
    inPoint.x = inPoints[pid + 0];
    inPoint.y = inPoints[pid + 1];
    inPoint.z = inPoints[pid + 2];
    inPoint.w = 1.0f;

    float4 outPoint;
    outPoint.x = inPoint.x;
    outPoint.y = inPoint.y;
    outPoint.z = inPoint.z;
    outPoint.w = 1.0f;

    float4 spherePoint;
    spherePoint.x = dot(outPoint, sphereInverseMatrix[0]);
    spherePoint.y = dot(outPoint, sphereInverseMatrix[1]);
    spherePoint.z = dot(outPoint, sphereInverseMatrix[2]);
    spherePoint.w = 0.0f;

    if (length(spherePoint) < 1.0)
    {
        spherePoint = normalize(spherePoint);
        spherePoint.w = 1.0f;

        outPoint.x = dot(spherePoint, sphereMatrix[0]);
        outPoint.y = dot(spherePoint, sphereMatrix[1]);
        outPoint.z = dot(spherePoint, sphereMatrix[2]);

        outPoint.x = inPoint.x + ((outPoint.x - inPoint.x) * wt);
        outPoint.y = inPoint.y + ((outPoint.y - inPoint.y) * wt);
        outPoint.z = inPoint.z + ((outPoint.z - inPoint.z) * wt);
    }

    outPoints[pid + 0] = outPoint.x;
    outPoints[pid + 1] = outPoint.y;
    outPoints[pid + 2] = outPoint.z;
}