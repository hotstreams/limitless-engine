#pragma once

namespace Limitless {
    class Context;
    class Scene;
    class Assets;
    class Camera;

    /**
     *
     *
     *
     *
     */

    class SceneRenderer {
    private:
    public:


        void draw(Context& context, const Assets& assets, Scene& scene, Camera& camera);


        class Builder {

        };
    };
}