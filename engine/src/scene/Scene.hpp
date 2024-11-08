#pragma once


class Scene {
public:

    virtual ~Scene() = default;

    virtual void onCreate() = 0;

    virtual void onStart() = 0;

    virtual void onUpdate(float deltaTime) = 0;

    virtual void onRender(int screenWidth, int screenHeight) = 0;

    virtual void onDestroy() = 0;

};
