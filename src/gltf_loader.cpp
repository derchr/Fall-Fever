#include "gltf_loader.h"
#include "util/Log.h"

template <typename T>
static auto create_vertex_attribute_data(std::span<uint8_t const> vertex_attribute_data)
    -> VertexAttributeData
{
    T attribute_data;
    attribute_data.reserve(vertex_attribute_data.size_bytes());

    std::memcpy(
        attribute_data.data(), vertex_attribute_data.data(), vertex_attribute_data.size_bytes());

    return VertexAttributeData{.values = std::move(attribute_data)};
}

template <typename T>
static auto create_indices(std::span<uint8_t const> gltf_index_data) -> Indices
{
    T index_data;
    index_data.reserve(gltf_index_data.size_bytes());

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

    entt::hashed_string material_hash(material.name.c_str());
    return material_cache
        .load(material_hash,
              Material{.base_color_texture = base_color_image,
                       .normal_map_texture = normal_map_image})
        .first->second;
}

static auto load_attribute(std::string_view attribute_name,
                           uint32_t attribute_id,
                           fx::gltf::Document const &gltf) -> std::optional<VertexAttributeData>
{
    auto const &attribute_accessor = gltf.accessors.at(attribute_id);

    if (attribute_accessor.componentType != fx::gltf::Accessor::ComponentType::Float) {
        Log::logger().critical("Only float attributes supported!");
        std::terminate();
    }

    auto vertex_attribute_id = [attribute_name]() -> std::optional<std::size_t> {
        if (attribute_name == "POSITION") {
            return 0;
        }
        if (attribute_name == "TEXCOORD_0") {
            return 1;
        }
        if (attribute_name == "NORMAL") {
            return 2;
        }
        if (attribute_name == "TANGENT") {
            return 3;
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

    return std::move(vertex_attribute_data);
}

auto load_gltf_primitive(fx::gltf::Primitive const &gltf_primitive,
                         fx::gltf::Document const &gltf,
                         std::filesystem::path const &document_path,
                         unsigned primitive_id,
                         entt::resource_cache<Material> &material_cache,
                         entt::resource_cache<Mesh> &mesh_cache) -> GltfPrimitive
{
    // Load attributes
    std::map<Mesh::VertexAttributeId, VertexAttributeData> attributes;
    for (auto const &attribute : gltf_primitive.attributes) {
        auto vertex_attribute_data = load_attribute(attribute.first, attribute.second, gltf);

        if (!vertex_attribute_data.has_value()) {
            continue;
        }

        attributes.emplace(attribute.second, std::move(vertex_attribute_data.value()));
    }

    // Load indices
    auto const &indices_accessor = gltf.accessors.at(gltf_primitive.indices);
    auto const &indices_buffer_view = gltf.bufferViews.at(indices_accessor.bufferView);
    auto const &indices_buffer = gltf.buffers.at(indices_buffer_view.buffer);

    std::span<uint8_t const> indices_buffer_span(indices_buffer.data);
    std::span<uint8_t const> indices_data_span =
        indices_buffer_span.subspan(indices_buffer_view.byteOffset, indices_buffer_view.byteLength);

    auto indices = [indices_data_span, &indices_accessor]() {
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

    std::string const mesh_name =
        document_path.string() + ".primitive." + std::to_string(primitive_id);
    entt::hashed_string const mesh_hash(mesh_name.c_str());

    entt::resource<Mesh> mesh =
        mesh_cache.load(mesh_hash, Mesh{.attributes = attributes, .indices = indices})
            .first->second;

    // Get material by name
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
            primitives.push_back(load_gltf_primitive(
                gltf_primitive, gltf, document_path, primitive_count, material_cache, mesh_cache));
        }

        entt::hashed_string gltf_mesh_hash(gltf_mesh.name.c_str());
        entt::resource<GltfMesh> gltf_mesh_resource =
            gltf_mesh_cache.load(gltf_mesh_hash, GltfMesh{.primitives = std::move(primitives)})
                .first->second;

        gltf_meshes.push_back(gltf_mesh_resource);
    }

    return std::make_shared<Gltf>(Gltf{.materials = std::move(materials),
                                       .meshes = std::move(gltf_meshes),
                                       .document = std::move(gltf)});
}