//
// Created by felix on 5/5/2019.
//

#include "ModelLoad.h"

#include <iostream>
#include <algorithm>

wagl::Mesh::MeshData wagl::loadModel(tinygltf::Model& gltf, const std::string_view name) {
    Mesh::MeshData mesh;

    auto& gltfMesh = *std::find_if(gltf.meshes.begin(), gltf.meshes.end(), [&name](const tinygltf::Mesh& mesh) {
        return mesh.name == name;
    });

    //todo: support multiple primitives
    auto& gltfPrim = gltfMesh.primitives[0];


    auto readAttribute = [&](const tinygltf::Accessor& accessor, int index) {
        auto& view = gltf.bufferViews[accessor.bufferView];
        auto& buffer = gltf.buffers[view.buffer];

        auto byteOffset = view.byteOffset + accessor.byteOffset + (index * accessor.ByteStride(view));
        return buffer.data.data() + byteOffset;
    };


    //read elements
    {
        auto &accessor = gltf.accessors[gltfPrim.indices];

        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);

        auto &view = gltf.bufferViews[accessor.bufferView];
        auto &buffer = gltf.buffers[view.buffer];
        mesh.elements.resize(accessor.count);
        for (int i = 0; i < accessor.count; i++) {
            mesh.elements[i] = ((GLushort*)(buffer.data.data() + view.byteOffset + accessor.byteOffset))[i];
        }
    }

    {
        auto& posAccessor  = gltf.accessors[gltfPrim.attributes["POSITION"]];
        auto& normAccessor = gltf.accessors[gltfPrim.attributes["NORMAL"]];
        auto& texcAccessor = gltf.accessors[gltfPrim.attributes["TEXCOORD_0"]];

        assert(posAccessor.count == normAccessor.count && posAccessor.count == texcAccessor.count);

        auto& posView = gltf.bufferViews[posAccessor.bufferView];
        auto& posBuffer = gltf.buffers[posView.buffer];

        mesh.vertices.resize(posAccessor.count);

        for(int i = 0; i < posAccessor.count; i++) {
            mesh.vertices[i].position = *(glm::vec3*)readAttribute(posAccessor, i);
            mesh.vertices[i].normal = *(glm::vec3*)readAttribute(normAccessor, i);
            mesh.vertices[i].texcoord = *(glm::vec2*)readAttribute(texcAccessor, i);
        }
    }

    return mesh;
}

wagl::Transform wagl::transformFromNode(const tinygltf::Node &node) {
    Transform t;
    if (!node.translation.empty()) {
        t.position.x = node.translation[0];
        t.position.y = node.translation[1];
        t.position.z = node.translation[2];
    } else {
        t.position = glm::vec3(0,0,0);
    }
    if(!node.scale.empty()) {
        t.scale = node.scale[0];
    } else{
        t.scale = 1;
    }
    if(!node.rotation.empty()) {
        t.orientation.x = node.rotation[0];
        t.orientation.y = node.rotation[1];
        t.orientation.z = node.rotation[2];
        t.orientation.w = node.rotation[3];
    } else {
        t.orientation = glm::quat();
    }

    return t;
}
