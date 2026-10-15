#include "apex_pro_mini_scene.h"
#include <cassert>
using namespace apex_mini;
int main() {
    using namespace scene;
    const Color black{0,0,0}, white{255,255,255};
    const auto same = [](Color a, Color b) { return a.r==b.r && a.g==b.g && a.b==b.b; };
    assert(same(combine(black, {white,0.0,Blend::Normal}),black));
    assert(same(combine(black, {white,1.0,Blend::Normal}),white));
    assert(same(combine({100,50,0}, {{200,50,255},1.0,Blend::Add}), Color{255,100,255}));
    assert(same(combine(white, {{128,64,0},1.0,Blend::Multiply}), Color{128,64,0}));
    assert(same(combine(black, {{128,64,0},1.0,Blend::Screen}), Color{128,64,0}));
    assert(same(combine(black, {white,-1.0,Blend::Normal}),black));
    assert(same(combine(black, {white,2.0,Blend::Normal}),white));
    assert(aperture(0.5,0.5,0.5,0.5,0.3,0.1)==1.0);
    assert(aperture(1,1,0.5,0.5,0.3,0.1)==0.0);
    assert(aperture(0.5,0.5,0.5,0.5,0,0)==0.0);
}
