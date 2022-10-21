#include "gltf_loader.h"
#include "util/Log.h"
#include "definitions/attribute_locations.h"

#include <iterator>

template <typename T>
static auto create_vertex_attribute_data(std::span<uint8_t const> vertex_attribute_data)
    -> VertexAttributeData
{
    T attribute_data;
    attribute_data.resize(vertex_attribute_data.size_bytes());

    std::memcpy(
        attribute_data.data(), vertex_attribute_data.data(), vertex_attribute_data.size_bytes());

    return VertexAttributeData{.values = std::move(attribute_data)};
}

template <typename T>
static auto create_indices(std::span<uint8_t const> gltf_index_data) -> Indices
{
    T index_data;
    index_data.resize(gltf_index_data.size_bytes());

    std::memcpy(index_data.data(), gltf_index_data.data(), gltf_index_data.size_bytes());

    return Indices{.values = std::move(index_data)};
}

static auto load_texture(fx::gltf::Texture const &texture,
                         fx::gltf::Document const &gltf,
                         std::filesystem::path const &document_path,
                         Image::ColorFormat colorFormat,
                         entt::resource_cache<Image> &image_cache) -> entt::resource<Image>
{
    auto const &gltf_image = gltf.images.at(texture.source);
    auto const base_directory = document_path.parent_path();

    if (gltf_image.uri.empty()) {
        auto const &image_buffer_view = gltf.bufferViews.at(gltf_image.bufferView);
        auto const &image_buffer = gltf.buffers.at(image_buffer_view.buffer);

        std::string const image_name =
            document_path.string() + ".image." + std::to_string(texture.source);
        entt::hashed_string const image_hash(image_name.c_str());

        return image_cache
            .load(image_hash,
                  std::span{image_buffer.data}.subspan(image_buffer_view.byteOffset,
                                                       image_buffer_view.byteLength),
                  colorFormat)
            .first->second;
    }

    auto const image_path = base_directory / gltf_image.uri;
    std::size_t const image_size = std::filesystem::file_size(image_path);
    auto image_ifstream = std::ifstream(image_path, std::ios::binary);

    std::vector<uint8_t> image_data;
    image_data.reserve(image_size);

    std::copy(std::istreambuf_iterator<char>(image_ifstream),
              std::istreambuf_iterator<char>(),
              std::back_inserter(image_data));

    entt::hashed_string const image_hash(image_path.c_str());

    return image_cache.load(image_hash, image_data, colorFormat).first->second;
}

static auto load_material(fx::gltf::Material const &material,
                          fx::gltf::Document const &gltf,
                          std::filesystem::path const &document_path,
                          entt::resource_cache<Material> &material_cache,
                          entt::resource_cache<Image> &image_cache) -> entt::resource<Material>
{
    auto base_color_texture_id = material.pbrMetallicRoughness.baseColorTexture.index;
    auto normal_texture_id = material.normalTexture.index;

    std::optional<entt::resource<Image>> base_color_image;
    if (base_color_texture_id != -1) {
        auto const &base_color_texture = gltf.textures.at(base_color_texture_id);
        base_color_image = load_texture(
            base_color_texture, gltf, document_path, Image::ColorFormat::SRGB, image_cache);
    }

    std::optional<entt::resource<Image>> normal_map_image;
    if (normal_texture_id != -1) {
        auto const &normal_texture = gltf.textures.at(normal_texture_id);
        normal_map_image =
            load_texture(normal_texture, gltf, document_path, Image::ColorFormat::RGB, image_cache);
    }

    if (material.name.empty()) {
        Log::logger().warn("glTF material has no name.");
    }

    entt::hashed_string material_hash(material.name.c_str());
    return material_cache
        .load(material_hash,
              Material{.base_color_texture = base_color_image,
                       .normal_map_texture = normal_map_image})
        .first->second;
}

static auto load_attribute(std::string_view attribute_name,
                           uint32_t attribute_id,
                           fx::gltf::Document const &gltf)
    -> std::optional<std::pair<std::size_t, VertexAttributeData>>
{
    auto const &attribute_accessor = gltf.accessors.at(attribute_id);

    if (attribute_accessor.componentType != fx::gltf::Accessor::ComponentType::Float) {
        Log::logger().critical("Only float attributes supported!");
        std::terminate();
    }

    auto vertex_attribute_id = [attribute_name]() -> std::optional<std::size_t> {
        if (attribute_name == "POSITION") {
            return ATTRIBUTE_LOCATION.position;
        }
        if (attribute_name == "TEXCOORD_0") {
            return ATTRIBUTE_LOCATION.uv;
        }
        if (attribute_name == "NORMAL") {
            return ATTRIBUTE_LOCATION.normal;
        }
        if (attribute_name == "TANGENT") {
            return ATTRIBUTE_LOCATION.tangent;
        }

        return {};
    }();

    // Skip unsupported attribute
    if (!vertex_attribute_id.has_value()) {
        return {};
    }

    auto const &buffer_view = gltf.bufferViews.at(attribute_accessor.bufferView);
    auto const &buffer = gltf.buffers.at(buffer_view.buffer);

    std::span<uint8_t const> buffer_span(buffer.data);
    std::span<uint8_t const> vertex_attribute_data_span =
        buffer_span.subspan(buffer_view.byteOffset, buffer_view.byteLength);

    auto vertex_attribute_data = [vertex_attribute_data_span, &attribute_accessor]() {
        if (attribute_accessor.type == fx::gltf::Accessor::Type::Scalar) {
            return create_vertex_attribute_data<VertexAttributeData::Scalar>(
                vertex_attribute_data_span);
        }
        if (attribute_accessor.type == fx::gltf::Accessor::Type::Vec2) {
            return create_vertex_attribute_data<VertexAttributeData::Vec2>(
                vertex_attribute_data_span);
        }
        if (attribute_accessor.type == fx::gltf::Accessor::Type::Vec3) {
            return create_vertex_attribute_data<VertexAttributeData::Vec3>(
                vertex_attribute_data_span);
        }
        if (attribute_accessor.type == fx::gltf::Accessor::Type::Vec4) {
            return create_vertex_attribute_data<VertexAttributeData::Vec4>(
                vertex_attribute_data_span);
        }

        Log::logger().critical("Unsupported vertex attribute type!");
        std::terminate();
    }();

    return std::make_pair(vertex_attribute_id.value(), std::move(vertex_attribute_data));
}

auto load_gltf_primitive(fx::gltf::Primitive const &gltf_primitive,
                         fx::gltf::Document const &gltf,
                         std::string_view primitive_identifier,
                         entt::resource_cache<Material> &material_cache,
                         entt::resource_cache<Mesh> &mesh_cache) -> GltfPrimitive
{
    // Load attributes
    auto tangent_it =
        std::find_if(gltf_primitive.attributes.cbegin(),
                     gltf_primitive.attributes.cend(),
                     [](auto const &attribute) { return attribute.first == "TANGENT"; });

    if (tangent_it == gltf_primitive.attributes.cend()) {
        Log::logger().critical("glTF scene has to include tangent and normal components!");
        std::terminate();
    }

    std::map<Mesh::VertexAttributeId, VertexAttributeData> attributes;
    for (auto const &attribute : gltf_primitive.attributes) {
        auto vertex_attribute = load_attribute(attribute.first, attribute.second, gltf);

        if (!vertex_attribute.has_value()) {
            continue;
        }

        attributes.emplace(vertex_attribute.value().first,
                           std::move(vertex_attribute.value().second));
    }

    // Load indices
    auto const &indices_accessor = gltf.accessors.at(gltf_primitive.indices);
    auto const &indices_buffer_view = gltf.bufferViews.at(indices_accessor.bufferView);
    auto const &indices_buffer = gltf.buffers.at(indices_buffer_view.buffer);

    std::span<uint8_t const> indices_buffer_span(indices_buffer.data);
    std::span<uint8_t const> indices_data_span =
        indices_buffer_span.subspan(indices_buffer_view.byteOffset, indices_buffer_view.byteLength);

    Indices indices = [indices_data_span, &indices_accessor]() {
        if (indices_accessor.componentType == fx::gltf::Accessor::ComponentType::UnsignedByte) {
            return create_indices<Indices::UnsignedByte>(indices_data_span);
        }
        if (indices_accessor.componentType == fx::gltf::Accessor::ComponentType::UnsignedShort) {
            return create_indices<Indices::UnsignedShort>(indices_data_span);
        }
        if (indices_accessor.componentType == fx::gltf::Accessor::ComponentType::UnsignedInt) {
            return create_indices<Indices::UnsignedInt>(indices_data_span);
        }

        Log::logger().critical("Unsupported indices type!");
        std::terminate();
    }();

    entt::hashed_string const mesh_hash(primitive_identifier.data());

    entt::resource<Mesh> mesh =
        mesh_cache.load(mesh_hash, Mesh{.attributes = attributes, .indices = std::move(indices)})
            .first->second;

    // Get material by hash
    auto const &gltf_material = gltf.materials.at(gltf_primitive.material);
    entt::hashed_string material_hash(gltf_material.name.c_str());
    entt::resource<Material> material = material_cache[material_hash];

    return GltfPrimitive{.mesh = mesh, .material = material};
}

auto GltfLoader::operator()(std::filesystem::path const &document_path) -> result_type
{
    fx::gltf::ReadQuotas const read_quotas{.MaxFileSize = MAX_SIZE,
                                           .MaxBufferByteLength = MAX_SIZE};

    fx::gltf::Document gltf = [&]() {
        if (document_path.extension() == ".gltf") {
            return fx::gltf::LoadFromText(document_path, read_quotas);
        }

        return fx::gltf::LoadFromBinary(document_path, read_quotas);
    }();

    // Load here all the rest...
    auto const base_directory = document_path.parent_path();

    // Load materials
    std::vector<entt::resource<Material>> materials;
    for (auto const &gltf_material : gltf.materials) {
        entt::resource<Material> material =
            load_material(gltf_material, gltf, document_path, material_cache, image_cache);
        materials.push_back(material);
    }

    // Load meshes
    std::vector<entt::resource<GltfMesh>> gltf_meshes;
    gltf_meshes.reserve(gltf.meshes.size());

    for (auto const &gltf_mesh : gltf.meshes) {
        // Load primitives
        unsigned primitive_count = 0;

        std::vector<GltfPrimitive> primitives;
        primitives.reserve(gltf_mesh.primitives.size());

        for (auto const &gltf_primitive : gltf_mesh.primitives) {
            std::string const primitive_identifier = document_path.string() + "." + gltf_mesh.name +
                                                     "." + ".primitive." +
                                                     std::to_string(primitive_count);

            primitives.push_back(load_gltf_primitive(
                gltf_primitive, gltf, primitive_identifier, material_cache, mesh_cache));
            ++primitive_count;
        }

        if (gltf_mesh.name.empty()) {
            Log::logger().warn("glTF mesh has no name.");
        }

        entt::hashed_string gltf_mesh_hash(gltf_mesh.name.c_str());
        entt::resource<GltfMesh> gltf_mesh_resource =
            gltf_mesh_cache.load(gltf_mesh_hash, GltfMesh{.primitives = std::move(primitives)})
                .first->second;

        gltf_meshes.push_back(gltf_mesh_resource);
    }

    // Load nodes
    std::unordered_map<std::size_t, entt::resource<GltfNode>> nodes_map;
    nodes_map.reserve(gltf.nodes.size());
    for (std::size_t i = 0; i < gltf.nodes.size(); ++i) {
        auto const &node = gltf.nodes.at(i);

        auto mesh = [this, &node, &gltf_meshes]() -> std::optional<entt::resource<GltfMesh>> {
            if (node.mesh != -1) {
                auto const &gltf_mesh = gltf_meshes.at(node.mesh);
                return {gltf_mesh};
            }

            return {};
        }();

        glm::vec3 translation(node.translation[0], node.translation[1], node.translation[2]);
        glm::quat rotation(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
        glm::vec3 scale(node.scale[0], node.scale[1], node.scale[2]);

        Transform transform{.translation = translation, .rotation = rotation, .scale = scale};

        if (node.name.empty()) {
            Log::logger().warn("glTF node has no name.");
        }

        entt::hashed_string node_hash(node.name.c_str());
        entt::resource<GltfNode> node_resource =
            gltf_node_cache
                .load(node_hash,
                      GltfNode{
                          .name = node.name, .transform = transform, .mesh = mesh, .children = {}})
                .first->second;

        nodes_map.emplace(i, node_resource);
    }

    // Resolve child hierarchy
    // TODO WRONG!!! does only work for 1 child generation
    for (std::size_t i = 0; i < gltf.nodes.size(); ++i) {
        auto const &gltf_node = gltf.nodes.at(i);
        std::vector<entt::resource<GltfNode>> children;
        for (int child_node_id : gltf_node.children) {
            auto child_node = nodes_map.extract(child_node_id);
            children.push_back(child_node.mapped());
        }
        auto const &node = nodes_map.at(i);
        node->children = std::move(children);
    };

    std::vector<entt::resource<GltfNode>> nodes;
    nodes.reserve(nodes_map.size());
    for (auto const &node : nodes_map) {
        nodes.push_back(node.second);
    }

    // Load scenes
    std::vector<entt::resource<GltfScene>> scenes;
    for (auto const &scene : gltf.scenes) {
        // Get nodes by hash
        std::vector<entt::resource<GltfNode>> nodes;
        nodes.reserve(scene.nodes.size());

        for (auto node_id : scene.nodes) {
            auto const &node = gltf.nodes.at(node_id);
            entt::hashed_string node_hash(node.name.c_str());
            nodes.push_back(gltf_node_cache[node_hash]);
        }

        if (scene.name.empty()) {
            Log::logger().warn("glTF scene has no name.");
        }

        entt::hashed_string scene_hash(scene.name.c_str());
        entt::resource<GltfScene> scene_resource =
            gltf_scene_cache.load(scene_hash, GltfScene{.nodes = std::move(nodes)}).first->second;
        scenes.push_back(scene_resource);
    }

    // Default scene
    entt::resource<GltfScene> default_scene = [&gltf, &scenes]() {
        if (gltf.scene != -1) {
            return scenes.at(gltf.scene);
        }

        return scenes.at(0);
    }();

    return std::make_shared<Gltf>(Gltf{.materials = std::move(materials),
                                       .meshes = std::move(gltf_meshes),
                                       .nodes = std::move(nodes),
                                       .scenes = std::move(scenes),
                                       .default_scene = default_scene,
                                       .document = std::move(gltf)});
}