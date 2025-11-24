#include <memory>
#include "Engine.hpp"
#include "Demo.hpp"

int main(){
    RenderToy::Engine engine;
    Demo::DemoGame game;

    return engine.run(game);
}
