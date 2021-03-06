#include "Triangle.h"
#include "TriangleMesh.h"
#include "Ray.h"
#include "Lambert.h"
#include "PointLightShading.h"
#include "DiffuseShading.h"
#include "SpecularReflectionShading.h"
#include "SpecularRefractionShading.h"
#include "SpecularHighlightsShading.h"

Triangle::Triangle(TriangleMesh * m, unsigned int i) :
    m_mesh(m), m_index(i)
{

}


Triangle::~Triangle()
{

}


void
Triangle::renderGL()
{
    TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
    const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
    const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
    const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

    glBegin(GL_TRIANGLES);
        glVertex3f(v0.x, v0.y, v0.z);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}



bool
Triangle::intersect(HitInfo& result, const Ray& r, float tMin, float tMax)
{
	TriangleMesh::TupleI3 ti3_v = m_mesh->vIndices()[m_index]; //get vertices
	TriangleMesh::TupleI3 ti3_n = m_mesh->nIndices()[m_index]; //get normals
	const Vector3 & v0 = m_mesh->vertices()[ti3_v.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3_v.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3_v.z]; //vertex c of triangle
	const Vector3 & n0 = m_mesh->normals()[ti3_n.x]; //normal a of triangle
	const Vector3 & n1 = m_mesh->normals()[ti3_n.y]; //normal b of triangle
	const Vector3 & n2 = m_mesh->normals()[ti3_n.z]; //normal c of triangle
	float t;
	Vector3 rayOrigin = r.o;
	Vector3 rayDir = r.d;

	//Solve for normal of the plane. In this case, the triangle
	// || (V1 - V0) x (V2 x V0) ||	triangle face's normal
	Vector3 triangleNormal = cross((v1 - v0), (v2 - v0));
	triangleNormal.normalize();

	// D = dot(normal, any vertice)
	float D = dot(triangleNormal, v0);

	// Check if dot(n, d) == 0. If it's 0, the ray is parallel to the plane, and would give a divide by zero when caluclating t
	float parallelCheck = dot(triangleNormal, rayDir);
	if (parallelCheck == 0)
	{
		return false;
	}

	// P = o + tR
	// Ax + By + Cz + D = 0
	// t = (D - dot(n, o)) / dot(normal, d)
	if (abs(parallelCheck) > 0.0001f)
	{
		t = (dot(triangleNormal, (v0 - rayOrigin))) / parallelCheck;
		if (t < 0.0001f || t > tMax) return false;
		
	}
	else return false;
	//float t = (D - dot(triangleNormal, rayOrigin)) / parallelCheck;

	// Calculate point where ray intersects plane. Note that this does not mean it intersects the triangle
	Vector3 intersection = rayOrigin + (t*rayDir);

	//Check if point intersects the triangle
	Vector3 edge0 = v1 - v0;
	Vector3 edge1 = v2 - v1;
	Vector3 edge2 = v0 - v2;
	Vector3 check0 = intersection - v0;
	Vector3 check1 = intersection - v1;
	Vector3 check2 = intersection - v2;

	if (dot(cross(edge0, check0), triangleNormal) >= 0 && dot(cross(edge1, check1), triangleNormal) >= 0 && dot(cross(edge2, check2), triangleNormal) >= 0)
	{
		// If it's in the triangle, calculate the barycentric coordinates
		
		float barycentricHelper = dot(cross((v1 - v0), (v2 - v0)), triangleNormal);
		float alpha = dot(cross((v2 - v1), (intersection - v1)), triangleNormal) / barycentricHelper;
		float beta = dot(cross((v0 - v2), (intersection - v2)), triangleNormal) / barycentricHelper;
		float gamma = dot(cross((v1 - v0), (intersection - v0)), triangleNormal) / barycentricHelper;

		// calculate the normal of the barycentric coordinate
		Vector3 normalQ = (alpha*n0 + beta*n1 + gamma*n2);
		normalQ.normalize();

		Vector3 vertexQ = (alpha*v0 + beta*v1 + gamma*v2);
		result.N = normalQ;
		result.P = intersection;
		result.t = t;
		
		//result.material = new Lambert(Vector3(.2f,.6f,.6f));
		result.material = this->m_material;
		return true;
	}
	return false;
} 

/*
bool
Triangle::intersect(HitInfo& result, const Ray& r, float tMin, float tMax)
{
	TriangleMesh::TupleI3 ti3 = m_mesh->vIndices()[m_index];
	const Vector3 & v0 = m_mesh->vertices()[ti3.x]; //vertex a of triangle
	const Vector3 & v1 = m_mesh->vertices()[ti3.y]; //vertex b of triangle
	const Vector3 & v2 = m_mesh->vertices()[ti3.z]; //vertex c of triangle

													// init
	float delta = dot(cross(v0 - v1, v0 - v2), r.d);
	float delta1 = dot(cross(v0 - r.o, v0 - v2), r.d);
	float delta2 = dot(cross(v0 - v1, v0 - r.o), r.d);
	float delta3 = dot(cross(v0 - v1, v0 - v2), v0 - r.o);

	float beta = delta1 / delta;
	float gamma = delta2 / delta;
	float t = delta3 / delta;

	if ((beta < 0) || (beta > 1) || (gamma < 0) || (gamma > 1) || ((1 - beta - gamma) < 0) || ((1 - beta - gamma) > 1) || t < tMin + epsilon || t > tMax)
		return false;

	result.t = t;
	result.P = r.o + result.t * r.d;

	TriangleMesh::TupleI3 tin3 = m_mesh->nIndices()[m_index];
	result.N = m_mesh->normals()[tin3.x] * (1 - beta - gamma) + m_mesh->normals()[tin3.y] * beta + m_mesh->normals()[tin3.z] * gamma;
	result.N.normalize();
	result.material = this->m_material;

	return true;
} */