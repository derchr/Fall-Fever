#include "gltf_loader.h"

static void load_texture(fx::gltf::Texture const &texture,
                  fx::gltf::Document const &gltf,
                  std::filesystem::path const &document_path,
                  Image::ColorFormat colorFormat,
                  entt::resource_cache<Image> &image_cache)
{
    auto const &gltf_image = gltf.images.at(texture.source);
    auto const base_directory = document_path.parent_path();

    if (gltf_image.uri.empty()) {
        auto const &image_buffer_view = gltf.bufferViews.at(gltf_image.bufferView);
        auto const &image_buffer = gltf.buffers.at(image_buffer_view.buffer);

        std::string const image_name = document_path.string() + ".image." + std::to_string(texture.source);
        entt::hashed_string image_hash(image_name.c_str());

        image_cache.load(
            image_hash,
            std::span{image_buffer.data}.subspan(image_buffer_view.byteOffset, image_buffer_view.byteLength),
            colorFormat);
    } else {
        auto const image_path = base_directory / gltf_image.uri;
        std::size_t const image_size = std::filesystem::file_size(image_path);
        auto image_ifstream = std::ifstream(image_path, std::ios::binary);

        std::vector<uint8_t> image_data;
        image_data.reserve(image_size);

        std::copy(std::istreambuf_iterator<char>(image_ifstream),
                  std::istreambuf_iterator<char>(),
                  std::back_inserter(image_data));

        entt::hashed_string image_hash(image_path.c_str());

        image_cache.load(image_hash, image_data, colorFormat);
    }
}

static void load_material(fx::gltf::Material const &material,
                   fx::gltf::Document const &gltf,
                   std::filesystem::path const &document_path,
                   entt::resource_cache<Image> &image_cache)
{
    auto base_color_texture_id = material.pbrMetallicRoughness.baseColorTexture.index;
    auto normal_texture_id = material.normalTexture.index;

    if (base_color_texture_id != -1) {
        auto const &base_color_texture = gltf.textures.at(base_color_texture_id);
        load_texture(base_color_texture, gltf, document_path, Image::ColorFormat::SRGB, image_cache);
    }

    if (normal_texture_id != -1) {
        auto const &normal_texture = gltf.textures.at(normal_texture_id);
        load_texture(normal_texture, gltf, document_path, Image::ColorFormat::RGB, image_cache);
    }
}

auto gltf_loader::operator()(std::filesystem::path const &document_path) -> result_type
{
    fx::gltf::ReadQuotas const read_quotas{.MaxFileSize = MAX_SIZE, .MaxBufferByteLength = MAX_SIZE};

    fx::gltf::Document gltf = [&]() {
        if (document_path.extension() == ".gltf") {
            return fx::gltf::LoadFromText(document_path, read_quotas);
        }

        return fx::gltf::LoadFromBinary(document_path, read_quotas);
    }();

    // Load here all the rest...
    auto const base_directory = document_path.parent_path();

    // Load textures
    for (auto const &material : gltf.materials) {
        load_material(material, gltf, document_path, image_cache);
    }

    // Load meshes

    return std::make_shared<fx::gltf::Document>(std::move(gltf));
}
