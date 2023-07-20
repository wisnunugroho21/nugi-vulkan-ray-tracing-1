#include "load_model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace nugiEngine
{
  LoadedModel loadModelFromFile(const std::string &filePath, uint32_t materialIndex, uint32_t offsetIndex) {
		tinyobj::attrib_t attrib{};
		std::vector<tinyobj::shape_t> shapes{};
		std::vector<tinyobj::material_t> materials{};
		// std::unordered_map<RayTraceVertex, uint32_t> uniqueVertices{};
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}
		
		auto primitives = std::make_shared<std::vector<Primitive>>();
		auto vertices = std::make_shared<std::vector<RayTraceVertex>>();

		for (const auto &shape: shapes) {
			uint32_t numTriangle = static_cast<uint32_t>(shape.mesh.indices.size()) / 3;

			for (uint32_t i = 0; i < numTriangle; i++) {
				int vertexIndex0 = shape.mesh.indices[3 * i + 0].vertex_index;
				int vertexIndex1 = shape.mesh.indices[3 * i + 1].vertex_index;
				int vertexIndex2 = shape.mesh.indices[3 * i + 2].vertex_index;

				int textCoordIndex0 = shape.mesh.indices[3 * i + 0].texcoord_index;
				int textCoordIndex1 = shape.mesh.indices[3 * i + 1].texcoord_index;
				int textCoordIndex2 = shape.mesh.indices[3 * i + 2].texcoord_index;

				RayTraceVertex vertex0;
				RayTraceVertex vertex1;
				RayTraceVertex vertex2;
				
				vertex0.position = glm::vec3{
					attrib.vertices[3 * vertexIndex0 + 0],
					attrib.vertices[3 * vertexIndex0 + 1],
					attrib.vertices[3 * vertexIndex0 + 2]
				};

				vertex1.position = glm::vec3{
					attrib.vertices[3 * vertexIndex1 + 0],
					attrib.vertices[3 * vertexIndex1 + 1],
					attrib.vertices[3 * vertexIndex1 + 2]
				};

				vertex2.position = glm::vec3{
					attrib.vertices[3 * vertexIndex2 + 0],
					attrib.vertices[3 * vertexIndex2 + 1],
					attrib.vertices[3 * vertexIndex2 + 2]
				};

				vertex0.texel = glm::vec2{
					attrib.texcoords[2 * textCoordIndex0 + 0],
					attrib.texcoords[2 * textCoordIndex0 + 1]
				};

				vertex1.texel = glm::vec2{
					attrib.texcoords[2 * textCoordIndex1 + 0],
					attrib.texcoords[2 * textCoordIndex1 + 1]
				};

				vertex2.texel = glm::vec2{
					attrib.texcoords[2 * textCoordIndex2 + 0],
					attrib.texcoords[2 * textCoordIndex2 + 1]
				};

				/* if (uniqueVertices.count(vertex0) == 0) {
					uniqueVertices[vertex0] = static_cast<uint32_t>(vertices->size()) + offsetIndex;
					vertices->emplace_back(vertex0);
				}

				if (uniqueVertices.count(vertex1) == 0) {
					uniqueVertices[vertex1] = static_cast<uint32_t>(vertices->size()) + offsetIndex;
					vertices->emplace_back(vertex1);
				}

				if (uniqueVertices.count(vertex2) == 0) {
					uniqueVertices[vertex2] = static_cast<uint32_t>(vertices->size()) + offsetIndex;
					vertices->emplace_back(vertex2);
				}

				uint32_t index0 = uniqueVertices[vertex0];
				uint32_t index1 = uniqueVertices[vertex1];
				uint32_t index2 = uniqueVertices[vertex2]; */

				uint32_t index0 = getIndex(vertices, vertex0);
				if (index0 == -1) {
					index0 = static_cast<uint32_t>(vertices->size()) + offsetIndex;
					vertices->emplace_back(vertex0);
				} else {
					index0 += offsetIndex;
				}

				uint32_t index1 = getIndex(vertices, vertex1);
				if (index1 == -1) {
					index1 = static_cast<uint32_t>(vertices->size()) + offsetIndex;
					vertices->emplace_back(vertex1);
				} else {
					index1 += offsetIndex;
				}

				uint32_t index2 = getIndex(vertices, vertex2);
				if (index2 == -1) {
					index2 = static_cast<uint32_t>(vertices->size()) + offsetIndex;
					vertices->emplace_back(vertex1);
				} else {
					index2 += offsetIndex;
				}

				primitives->emplace_back(Primitive{
					glm::uvec3{ index0, index1, index2 },
					materialIndex
				});
			}
		}

		return LoadedModel{ primitives, vertices };
	}
  
} // namespace nugiEngine
