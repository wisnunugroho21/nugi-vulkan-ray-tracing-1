#include "load_model.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace nugiEngine
{
	void loadModel(std::shared_ptr<tinyobj::attrib_t> attrib, std::shared_ptr<std::vector<tinyobj::shape_t>> shapes, uint32_t shapeIndex, uint32_t triangleIndex, 
		std::shared_ptr<std::vector<Primitive>> outPrimitives, std::shared_ptr<std::vector<RayTraceVertex>> outVertices, uint32_t materialIndex, uint32_t offsetIndex) 
	{
		int vertexIndex0 = (*shapes)[shapeIndex].mesh.indices[3 * triangleIndex + 0].vertex_index;
		int vertexIndex1 = (*shapes)[shapeIndex].mesh.indices[3 * triangleIndex + 1].vertex_index;
		int vertexIndex2 = (*shapes)[shapeIndex].mesh.indices[3 * triangleIndex + 2].vertex_index;

		int textCoordIndex0 = (*shapes)[shapeIndex].mesh.indices[3 * triangleIndex + 0].texcoord_index;
		int textCoordIndex1 = (*shapes)[shapeIndex].mesh.indices[3 * triangleIndex + 1].texcoord_index;
		int textCoordIndex2 = (*shapes)[shapeIndex].mesh.indices[3 * triangleIndex + 2].texcoord_index;

		RayTraceVertex vertex0;
		RayTraceVertex vertex1;
		RayTraceVertex vertex2;
		
		vertex0.position = glm::vec3{
			attrib->vertices[3 * vertexIndex0 + 0],
			attrib->vertices[3 * vertexIndex0 + 1],
			attrib->vertices[3 * vertexIndex0 + 2]
		};

		vertex1.position = glm::vec3{
			attrib->vertices[3 * vertexIndex1 + 0],
			attrib->vertices[3 * vertexIndex1 + 1],
			attrib->vertices[3 * vertexIndex1 + 2]
		};

		vertex2.position = glm::vec3{
			attrib->vertices[3 * vertexIndex2 + 0],
			attrib->vertices[3 * vertexIndex2 + 1],
			attrib->vertices[3 * vertexIndex2 + 2]
		};

		vertex0.texel = glm::vec2{
			attrib->texcoords[2 * textCoordIndex0 + 0],
			1.0f - attrib->texcoords[2 * textCoordIndex0 + 1]
		};

		vertex1.texel = glm::vec2{
			attrib->texcoords[2 * textCoordIndex1 + 0],
			1.0f - attrib->texcoords[2 * textCoordIndex1 + 1]
		};

		vertex2.texel = glm::vec2{
			attrib->texcoords[2 * textCoordIndex2 + 0],
			1.0f - attrib->texcoords[2 * textCoordIndex2 + 1]
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

		uint32_t index0 = static_cast<uint32_t>(outVertices->size()) + offsetIndex;
		(*outVertices)[triangleIndex + 0] = vertex0;

		uint32_t index1 = static_cast<uint32_t>(outVertices->size()) + offsetIndex;
		(*outVertices)[triangleIndex + 1] = vertex1;

		uint32_t index2 = static_cast<uint32_t>(outVertices->size()) + offsetIndex;
		(*outVertices)[triangleIndex + 2] = vertex2;

		(*outPrimitives)[triangleIndex] = {
			glm::uvec3{ index0, index1, index2 },
			materialIndex
		};
	}

  LoadedModel loadModelFromFile(const std::string &filePath, uint32_t materialIndex, uint32_t offsetIndex) {
		auto attrib = std::make_shared<tinyobj::attrib_t>();
		auto shapes = std::make_shared<std::vector<tinyobj::shape_t>>();
		auto materials = std::make_shared<std::vector<tinyobj::material_t>>();
		// std::unordered_map<RayTraceVertex, uint32_t> uniqueVertices{};
		std::string warn, err;

		if (!tinyobj::LoadObj(attrib.get(), shapes.get(), materials.get(), &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		uint32_t totalPrimitive = 0;
		uint32_t totalVertex = 0;

		for (uint32_t i = 0; i < (*shapes).size(); i++) {
			uint32_t numTriangle = static_cast<uint32_t>((*shapes)[i].mesh.indices.size()) / 3;

			totalPrimitive += numTriangle;
			totalVertex += numTriangle * 3;
		}
		
		auto primitives = std::make_shared<std::vector<Primitive>>(totalPrimitive);
		auto vertices = std::make_shared<std::vector<RayTraceVertex>>(totalVertex);

		std::vector<std::thread> threads;

		for (uint32_t i = 0; i < (*shapes).size(); i++) {
			uint32_t numTriangle = static_cast<uint32_t>((*shapes)[i].mesh.indices.size()) / 3;

			for (uint32_t j = 0; j < numTriangle; j++) {
				std::thread th(loadModel, attrib, shapes, i, j, primitives, vertices, materialIndex, offsetIndex);
				threads.emplace_back(std::move(th));
			}
		}

		for (auto &&thread : threads) {
			thread.join();
		}

		return LoadedModel{ primitives, vertices };
	}
  
} // namespace nugiEngine
