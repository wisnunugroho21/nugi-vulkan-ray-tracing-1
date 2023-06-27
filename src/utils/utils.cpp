#include "utils.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace std {
	template<>
	struct hash<nugiEngine::RayTraceVertex> {
		size_t operator () (nugiEngine::RayTraceVertex const &vertex) const {
			size_t seed = 0;
			nugiEngine::hashCombine(seed, vertex.position, vertex.texel);
			return seed;
		}
	};
}

namespace nugiEngine {
  RayTraceVertexPrimitive loadPrimitiveVertexFromFile(const std::string &filePath, uint32_t materialIndex, uint32_t firstVertexIndex) {
    tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		auto primitives = std::make_shared<std::vector<Primitive>>();
    auto vertices = std::make_shared<std::vector<RayTraceVertex>>();

    std::unordered_map<RayTraceVertex, uint32_t> uniqueVertices{};

		for (const auto &shape: shapes) {
			uint32_t numTriangle = static_cast<uint32_t>(shape.mesh.indices.size()) / 3;

			for (uint32_t i = 0; i < numTriangle; i++) {
        RayTraceVertex vertex0{};
        RayTraceVertex vertex1{};
        RayTraceVertex vertex2{};

				int vertexIndex0 = shape.mesh.indices[3 * i + 0].vertex_index;
				int vertexIndex1 = shape.mesh.indices[3 * i + 1].vertex_index;
				int vertexIndex2 = shape.mesh.indices[3 * i + 2].vertex_index;

				int textCoordIndex0 = shape.mesh.indices[3 * i + 0].texcoord_index;
				int textCoordIndex1 = shape.mesh.indices[3 * i + 1].texcoord_index;
				int textCoordIndex2 = shape.mesh.indices[3 * i + 2].texcoord_index;

        vertex0.position = {
					attrib.vertices[3 * vertexIndex0 + 0],
					attrib.vertices[3 * vertexIndex0 + 1],
					attrib.vertices[3 * vertexIndex0 + 2]
				};

				vertex1.position = {
					attrib.vertices[3 * vertexIndex1 + 0],
					attrib.vertices[3 * vertexIndex1 + 1],
					attrib.vertices[3 * vertexIndex1 + 2]
				};

				vertex2.position = {
					attrib.vertices[3 * vertexIndex2 + 0],
					attrib.vertices[3 * vertexIndex2 + 1],
					attrib.vertices[3 * vertexIndex2 + 2]
				};

        vertex0.texel = {
          attrib.texcoords[2 * textCoordIndex0 + 0],
          1.0f - attrib.texcoords[2 * textCoordIndex0 + 1]
        };

        vertex1.texel = {
          attrib.texcoords[2 * textCoordIndex1 + 0],
          1.0f - attrib.texcoords[2 * textCoordIndex1 + 1]
        };

        vertex2.texel = {
          attrib.texcoords[2 * textCoordIndex2 + 0],
          1.0f - attrib.texcoords[2 * textCoordIndex2 + 1]
        };

        if (uniqueVertices.count(vertex0) == 0) {
					uniqueVertices[vertex0] = static_cast<uint32_t>(vertices->size());
					vertices->push_back(vertex0);
				}

        uint32_t indices0 = uniqueVertices[vertex0] + firstVertexIndex;

        if (uniqueVertices.count(vertex1) == 0) {
					uniqueVertices[vertex1] = static_cast<uint32_t>(vertices->size());
					vertices->push_back(vertex1);
				}

        uint32_t indices1 = uniqueVertices[vertex1] + firstVertexIndex;

        if (uniqueVertices.count(vertex2) == 0) {
					uniqueVertices[vertex2] = static_cast<uint32_t>(vertices->size());
					vertices->push_back(vertex2);
				}

        uint32_t indices2 = uniqueVertices[vertex2] + firstVertexIndex;

				primitives->emplace_back(Primitive{ glm::uvec3(indices0, indices1, indices2), materialIndex });
			}
		}

		RayTraceVertexPrimitive result { primitives, vertices };
		return result;
  }
}

