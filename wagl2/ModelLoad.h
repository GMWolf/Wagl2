//
// Created by felix on 5/5/2019.
//

#ifndef WAGL2_MODELLOAD_H
#define WAGL2_MODELLOAD_H

#include <string>
#include <Model.h>
#include <tiny_gltf.h>
#include <string>
#include <Transform.h>

namespace wagl {

    Mesh::MeshData loadModel(tinygltf::Model& gltf, std::string_view name);

    Transform transformFromNode(const tinygltf::Node& node);
}


#endif //WAGL2_MODELLOAD_H
