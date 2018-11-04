/*!
 * \author ddubois 
 * \date 30-Aug-18.
 */

#include "general_test_setup.hpp"
#undef TEST

#include <iostream>
#include <gtest/gtest.h>

namespace nova {
    int main() {
        TEST_SETUP_LOGGER();

        char buff[FILENAME_MAX];
        getcwd(buff, FILENAME_MAX);
        NOVA_LOG(DEBUG) << "Running in " << buff << std::flush;
        NOVA_LOG(DEBUG) << "Predefined resources at: " << CMAKE_DEFINED_RESOURCES_PREFIX;

        nova_settings settings;
        settings.get_options().api = graphics_api::vulkan;
        auto renderer = nova_renderer::initialize(settings);

        renderer->load_shaderpack(CMAKE_DEFINED_RESOURCES_PREFIX "shaderpacks/DefaultShaderpack");

        std::shared_ptr<iwindow> window = renderer->get_engine()->get_window();

        NOVA_LOG(DEBUG) << window->should_close();

        while (!window->should_close()) {
            renderer->execute_frame();
            window->on_frame_end();
        }

        nova_renderer::deinitialize();

        return 0;
    }
}

TEST(nova_renderer, end_to_end) {
    nova::main();
}