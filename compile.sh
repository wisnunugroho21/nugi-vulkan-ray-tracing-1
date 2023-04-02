glslc src/shader/forward_pass.vert -o bin/shader/forward_pass.vert.spv
glslc src/shader/forward_pass.frag -o bin/shader/forward_pass.frag.spv
glslc src/shader/forward_light.vert -o bin/shader/forward_light.vert.spv
glslc src/shader/forward_light.frag -o bin/shader/forward_light.frag.spv
glslc src/shader/direct_illumination.vert -o bin/shader/direct_illumination.vert.spv
glslc src/shader/direct_illumination.frag -o bin/shader/direct_illumination.frag.spv
glslc src/shader/indirect_illumination.comp -o bin/shader/indirect_illumination.comp.spv
