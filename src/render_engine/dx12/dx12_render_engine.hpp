/*!
 * \author ddubois
 * \date 03-Apr-19.
 */

#pragma once

#include "nova_renderer/render_engine.hpp"

#include <d3d12.h>

#include <DirectXMath.h>
#include <dxgi1_4.h>
#include <wrl.h>

#include <spirv_cross/spirv_hlsl.hpp>

namespace nova::renderer::rhi {
    /*!
     * \brief D3D12 implementation of a render engine
     */
    class DX12RenderEngine final : public RenderEngine {
    public:
        DX12RenderEngine(NovaSettings& settings);

        DX12RenderEngine(DX12RenderEngine&& old) noexcept = delete;
        DX12RenderEngine& operator=(DX12RenderEngine&& old) noexcept = delete;

        DX12RenderEngine(const DX12RenderEngine& other) = delete;
        DX12RenderEngine& operator=(const DX12RenderEngine& other) = delete;

        virtual ~DX12RenderEngine() = default;

        // Inherited via render_engine
        std::shared_ptr<Window> get_window() const override final;

        void set_num_renderpasses(uint32_t num_renderpasses) override final;

        Result<Renderpass*> create_renderpass(const shaderpack::RenderPassCreateInfo& data) override final;

        Framebuffer* create_framebuffer(const Renderpass* renderpass,
                                        const std::vector<Image*>& attachments,
                                        const glm::uvec2& framebuffer_size) override final;

        Result<PipelineInterface*> create_pipeline_interface(
            const std::unordered_map<std::string, ResourceBindingDescription>& bindings,
            const std::vector<shaderpack::TextureAttachmentInfo>& color_attachments,
            const std::optional<shaderpack::TextureAttachmentInfo>& depth_texture) override final;

        std::vector<DescriptorSet*> create_descriptor_sets(const PipelineInterface* pipeline_interface) override final;

        Result<Pipeline*> create_pipeline(const PipelineInterface* pipeline_interface,
                                          const shaderpack::PipelineCreateInfo& data) override final;

        Buffer* create_buffer(const BufferCreateInfo& info) override final;
        Image* create_texture(const shaderpack::TextureCreateInfo& info) override final;
        Semaphore* create_semaphore() override final;
        std::vector<Semaphore*> create_semaphores(uint32_t num_semaphores) override final;
        Fence* create_fence(bool signaled = false) override final;
        std::vector<Fence*> create_fences(uint32_t num_fences, bool signaled = false) override final;

        void destroy_renderpass(Renderpass* pass) override final;

        void destroy_framebuffer(const Framebuffer* framebuffer) override final;

        void destroy_pipeline(Pipeline* pipeline) override final;

        void destroy_texture(Image* resource) override final;
        void destroy_semaphores(const std::vector<Semaphore*>& semaphores) override final;
        void destroy_fences(const std::vector<Fence*>& fences) override final;
        CommandList* allocate_command_list(uint32_t thread_idx, QueueType needed_queue_type, CommandList::Level level) override final;
        void submit_command_list(CommandList* cmds,
                                 QueueType queue,
                                 Fence* fence_to_signal = nullptr,
                                 const std::vector<Semaphore*>& wait_semaphores = {},
                                 const std::vector<Semaphore*>& signal_semaphores = {}) override final;
        void open_window_and_create_surface(const NovaSettings::WindowOptions& options) override final;

    private:
        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory;

        Microsoft::WRL::ComPtr<ID3D12Device> device; // direct3d device

        Microsoft::WRL::ComPtr<IDXGISwapChain3> swapchain; // swapchain used to switch between render targets

        Microsoft::WRL::ComPtr<ID3D12CommandQueue> direct_command_queue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> compute_command_queue;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> copy_command_queue;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap;
        uint32_t rtv_descriptor_size = 0;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> uav_descriptor_heap;
        uint32_t uav_descriptor_size = 0;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> texture_descriptor_heap;
        uint32_t texture_descriptor_size = 0;

#pragma region Initialization
        void create_device();

        void create_queues();
#pragma endregion

#pragma region Helpers
        ID3D12RootSignature* create_root_signature(const std::unordered_map<std::string, ResourceBindingDescription>& bindings);
#pragma endregion
    };

    Microsoft::WRL::ComPtr<ID3DBlob> compile_shader(const shaderpack::ShaderSource& shader,
                                                    const std::string& target,
                                                    const spirv_cross::CompilerHLSL::Options& options,
                                                    std::unordered_map<uint32_t, std::vector<D3D12_DESCRIPTOR_RANGE1>>& tables);
} // namespace nova::renderer::rhi