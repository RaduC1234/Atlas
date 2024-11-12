#pragma once

class ImGuiLayer {
public:

    static void init();

    static void onUpdate(float deltaTime);

    static void onImGuiRender();

    static void shutdown();
};
