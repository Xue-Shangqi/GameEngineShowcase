#ifndef RAYLIB_CPP_INCLUDE_MESH_HPP_
#define RAYLIB_CPP_INCLUDE_MESH_HPP_

#include <string>
#include <vector>

#include "./raylib.hpp"
#include "./raylib-cpp-utils.hpp"
#include "./BoundingBox.hpp"
#include "./Vector3.hpp"
// #include "./Model.hpp"

namespace raylib {
/**
 * Vertex data definning a mesh
 */
class Mesh : public ::Mesh {
 public:
    Mesh(const ::Mesh& mesh) {
        set(mesh);
    }

    /**
     * Load meshes from model file
     */
    // static std::vector<Mesh> Load(const std::string_view fileName) {
    //    int count = 0;
    //    ::Mesh* meshes = LoadMeshes(fileName.data(), &count);
    //    return std::vector<Mesh>(meshes, meshes + count);
    // }

    Mesh(const Mesh&) = delete;

    Mesh(Mesh&& other) {
        set(other);

        other.vertexCount = 0;
        other.triangleCount = 0;
        other.vertices = nullptr;
        other.texcoords = nullptr;
        other.texcoords2 = nullptr;
        other.normals = nullptr;
        other.tangents = nullptr;
        other.colors = nullptr;
        other.indices = nullptr;
        other.animVertices = nullptr;
        other.animNormals = nullptr;
        other.boneIds = nullptr;
        other.boneWeights = nullptr;
        other.vaoId = 0;
        other.vboId = nullptr;
    }

    /**
     * Generate polygonal mesh
     */
    static ::Mesh Poly(int sides, float radius) {
        return ::GenMeshPoly(sides, radius);
    }

    /**
     * Generate plane mesh (with subdivisions)
     */
    static ::Mesh Plane(float width, float length, int resX, int resZ) {
        return ::GenMeshPlane(width, length, resX, resZ);
    }

    static ::Mesh Plane(float width, float length, int resX, int resZ, float textureScale) {
        ::Mesh mesh = {};

        resX++;
        resZ++;

        // Vertices definition
        int vertexCount = resX*resZ; // vertices get reused for the faces

        Vector3 *vertices = (Vector3 *)RL_MALLOC(vertexCount*sizeof(Vector3));
        for (int z = 0; z < resZ; z++)
        {
            // [-length/2, length/2]
            float zPos = ((float)z/(resZ - 1) - 0.5f)*length;
            for (int x = 0; x < resX; x++)
            {
                // [-width/2, width/2]
                float xPos = ((float)x/(resX - 1) - 0.5f)*width;
                vertices[x + z*resX] = (Vector3){ xPos, 0.0f, zPos };
            }
        }

        // Normals definition
        Vector3 *normals = (Vector3 *)RL_MALLOC(vertexCount*sizeof(Vector3));
        for (int n = 0; n < vertexCount; n++) normals[n] = (Vector3){ 0.0f, 1.0f, 0.0f };   // Vector3.up;

        // TexCoords definition
        Vector2 *texcoords = (Vector2 *)RL_MALLOC(vertexCount*sizeof(Vector2));
        for (int v = 0; v < resZ; v++)
        {
            for (int u = 0; u < resX; u++)
            {
                texcoords[u + v*resX] = (Vector2){ (float)u/(resX - 1), (float)v/(resZ - 1) };
            }
        }

        // Triangles definition (indices)
        int numFaces = (resX - 1)*(resZ - 1);
        int *triangles = (int *)RL_MALLOC(numFaces*6*sizeof(int));
        int t = 0;
        for (int face = 0; face < numFaces; face++)
        {
            // Retrieve lower left corner from face ind
            int i = face % (resX - 1) + (face/(resZ - 1)*resX);

            triangles[t++] = i + resX;
            triangles[t++] = i + 1;
            triangles[t++] = i;

            triangles[t++] = i + resX;
            triangles[t++] = i + resX + 1;
            triangles[t++] = i + 1;
        }

        mesh.vertexCount = vertexCount;
        mesh.triangleCount = numFaces*2;
        mesh.vertices = (float *)RL_MALLOC(mesh.vertexCount*3*sizeof(float));
        mesh.texcoords = (float *)RL_MALLOC(mesh.vertexCount*2*sizeof(float));
        mesh.normals = (float *)RL_MALLOC(mesh.vertexCount*3*sizeof(float));
        mesh.indices = (unsigned short *)RL_MALLOC(mesh.triangleCount*3*sizeof(unsigned short));

        // Mesh vertices position array
        for (int i = 0; i < mesh.vertexCount; i++)
        {
            mesh.vertices[3*i] = vertices[i].x;
            mesh.vertices[3*i + 1] = vertices[i].y;
            mesh.vertices[3*i + 2] = vertices[i].z;
        }

        // Mesh texcoords array
        for (int i = 0; i < mesh.vertexCount; i++)
        {
            mesh.texcoords[2*i] = texcoords[i].x * textureScale;
            mesh.texcoords[2*i + 1] = texcoords[i].y * textureScale;
        }

        // Mesh normals array
        for (int i = 0; i < mesh.vertexCount; i++)
        {
            mesh.normals[3*i] = normals[i].x;
            mesh.normals[3*i + 1] = normals[i].y;
            mesh.normals[3*i + 2] = normals[i].z;
        }

        // Mesh indices array initialization
        for (int i = 0; i < mesh.triangleCount*3; i++) mesh.indices[i] = triangles[i];

        RL_FREE(vertices);
        RL_FREE(normals);
        RL_FREE(texcoords);
        RL_FREE(triangles);

        // Upload vertex data to GPU (static mesh)
        UploadMesh(&mesh, false);

        return mesh;
    }

    /**
     * Generate cuboid mesh
     */
    static ::Mesh Cube(float width, float height, float length) {
        return ::GenMeshCube(width, height, length);
    }

    /**
     * Generate sphere mesh (standard sphere)
     */
    static ::Mesh Sphere(float radius, int rings, int slices) {
        return ::GenMeshSphere(radius, rings, slices);
    }

    /**
     * Generate half-sphere mesh (no bottom cap)
     */
    static ::Mesh HemiSphere(float radius, int rings, int slices) {
        return ::GenMeshHemiSphere(radius, rings, slices);
    }

    /**
     * Generate cylinder mesh
     */
    static ::Mesh Cylinder(float radius, float height, int slices) {
        return ::GenMeshCylinder(radius, height, slices);
    }

    /**
     * Generate cone/pyramid mesh
     */
    static ::Mesh Cone(float radius, float height, int slices) {
        return ::GenMeshCone(radius, height, slices);
    }

    /**
     * Generate torus mesh
     */
    static ::Mesh Torus(float radius, float size, int radSeg, int sides) {
        return ::GenMeshTorus(radius, size, radSeg, sides);
    }

    /**
     * Generate trefoil knot mesh
     */
    static ::Mesh Knot(float radius, float size, int radSeg, int sides) {
        return ::GenMeshKnot(radius, size, radSeg, sides);
    }

    /**
     * Generate heightmap mesh from image data
     */
    static ::Mesh Heightmap(const ::Image& heightmap, ::Vector3 size) {
        return ::GenMeshHeightmap(heightmap, size);
    }

    /**
     * Generate cubes-based map mesh from image data
     */
    static ::Mesh Cubicmap(const ::Image& cubicmap, ::Vector3 cubeSize) {
        return ::GenMeshCubicmap(cubicmap, cubeSize);
    }

    GETTERSETTER(int, VertexCount, vertexCount)
    GETTERSETTER(int, TriangleCount, triangleCount)
    GETTERSETTER(float*, Vertices, vertices)
    GETTERSETTER(float *, TexCoords, texcoords)
    GETTERSETTER(float *, TexCoords2, texcoords2)
    GETTERSETTER(float *, Normals, normals)
    GETTERSETTER(float *, Tangents, tangents)
    GETTERSETTER(unsigned char *, Colors, colors)
    GETTERSETTER(unsigned short *, Indices, indices) // NOLINT
    GETTERSETTER(float *, AnimVertices, animVertices)
    GETTERSETTER(float *, AnimNormals, animNormals)
    GETTERSETTER(unsigned char *, BoneIds, boneIds)
    GETTERSETTER(float *, BoneWeights, boneWeights)
    GETTERSETTER(unsigned int, VaoId, vaoId)
    GETTERSETTER(unsigned int *, VboId, vboId)

    Mesh& operator=(const ::Mesh& mesh) {
        set(mesh);
        return *this;
    }

    Mesh& operator=(const Mesh&) = delete;

    Mesh& operator=(Mesh&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        Unload();
        set(other);

        other.vertexCount = 0;
        other.triangleCount = 0;
        other.vertices = nullptr;
        other.texcoords = nullptr;
        other.texcoords2 = nullptr;
        other.normals = nullptr;
        other.tangents = nullptr;
        other.colors = nullptr;
        other.indices = nullptr;
        other.animVertices = nullptr;
        other.animNormals = nullptr;
        other.boneIds = nullptr;
        other.boneWeights = nullptr;
        other.vaoId = 0;
        other.vboId = nullptr;

        return *this;
    }

    ~Mesh() {
        Unload();
    }

    /**
     * Upload mesh vertex data to GPU (VRAM)
     */
    inline void Upload(bool dynamic = false) {
        ::UploadMesh(this, dynamic);
    }

    /**
     * Upload mesh vertex data to GPU (VRAM)
     */
    inline void UpdateBuffer(int index, void *data, int dataSize, int offset = 0) {
        ::UpdateMeshBuffer(*this, index, data, dataSize, offset);
    }

    /**
     * Draw a 3d mesh with material and transform
     */
    inline void Draw(const ::Material& material, const ::Matrix& transform) const {
        ::DrawMesh(*this, material, transform);
    }

    /**
     * Draw multiple mesh instances with material and different transforms
     */
    inline void Draw(const ::Material& material, ::Matrix* transforms, int instances) const {
        ::DrawMeshInstanced(*this, material, transforms, instances);
    }

    /**
     * Draw multiple mesh instances with material and different transforms
     */
    inline void Draw(const ::Material& material, std::span<::Matrix> transforms) const {
        ::DrawMeshInstanced(*this, material, transforms.data(), transforms.size());
    }

    /**
     * Export mesh data to file
     *
     * @throws raylib::RaylibException Throws if failed to export the Mesh.
     */
    inline void Export(const std::string_view fileName) {
        if (!::ExportMesh(*this, fileName.data())) {
            throw new RaylibException("Failed to export the Mesh");
        }
    }

    /**
     * Unload mesh from memory (RAM and/or VRAM)
     */
    inline void Unload() {
        if (vboId != nullptr) {
            ::UnloadMesh(*this);
            vboId = nullptr;
        }
    }

    /**
     * Compute mesh bounding box limits
     */
    inline raylib::BoundingBox BoundingBox() const {
        return ::GetMeshBoundingBox(*this);
    }

    /**
     * Compute mesh bounding box limits with respect to the given transformation
     */
    raylib::BoundingBox GetTransformedBoundingBox(raylib::Matrix transform) const {
        // Get min and max vertex to construct bounds (AABB)
        raylib::Vector3 minVertex = { 0 };
        raylib::Vector3 maxVertex = { 0 };

        if (vertices != NULL)
        {
            minVertex = raylib::Vector3{ vertices[0], vertices[1], vertices[2] }.Transform(transform);
            maxVertex = raylib::Vector3{ vertices[0], vertices[1], vertices[2] }.Transform(transform);

            for (int i = 1; i < vertexCount; i++)
            {
                minVertex = Vector3Min(minVertex, raylib::Vector3{ vertices[i*3], vertices[i*3 + 1], vertices[i*3 + 2] }.Transform(transform));
                maxVertex = Vector3Max(maxVertex, raylib::Vector3{ vertices[i*3], vertices[i*3 + 1], vertices[i*3 + 2] }.Transform(transform));
            }
        }

        // Create the bounding box
        raylib::BoundingBox box = {};
        box.min = minVertex;
        box.max = maxVertex;

        return box;
    }

    /**
     * Compute mesh bounding box limits
     */
    operator raylib::BoundingBox() {
        return BoundingBox();
    }

    /**
     * Compute mesh tangents
     */
    inline Mesh& GenTangents() {
        ::GenMeshTangents(this);
        return *this;
    }

    /**
     * Load model from generated mesh
     */
    inline Model LoadModelFrom() const {
        return ::LoadModelFromMesh(*this);
    }

    /**
     * Load model from generated mesh
     */
    operator Model() {
        return ::LoadModelFromMesh(*this);
    }

 private:
    void set(const ::Mesh& mesh) {
        vertexCount = mesh.vertexCount;
        triangleCount = mesh.triangleCount;
        vertices = mesh.vertices;
        texcoords = mesh.texcoords;
        texcoords2 = mesh.texcoords2;
        normals = mesh.normals;
        tangents = mesh.tangents;
        colors = mesh.colors;
        indices = mesh.indices;
        animVertices = mesh.animVertices;
        animNormals = mesh.animNormals;
        boneIds = mesh.boneIds;
        boneWeights = mesh.boneWeights;
        vaoId = mesh.vaoId;
        vboId = mesh.vboId;
    }
};
}  // namespace raylib

using RMesh = raylib::Mesh;

#endif  // RAYLIB_CPP_INCLUDE_MESH_HPP_
