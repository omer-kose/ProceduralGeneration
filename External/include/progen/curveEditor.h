#ifndef CURVE_EDITOR_H
#define CURVE_EDITOR_H
// ImGui Bezier widget. @r-lyeh, public domain
// v1.03: improve grabbing, confine grabbers to area option, adaptive size, presets, preview.
// v1.02: add BezierValue(); comments; usage
// v1.01: out-of-bounds coord snapping; custom border width; spacing; cosmetics
// v1.00: initial version
//
// [ref] http://robnapier.net/faster-bezier
// [ref] http://easings.net/es#easeInSine
//
// Usage:
// {  static float v[5] = { 0.390f, 0.575f, 0.565f, 1.000f }; 
//    ImGui::Bezier( "easeOutSine", v );       // draw
//    float y = ImGui::BezierValue( 0.5f, v ); // x delta in [0..1] range
// }

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <time.h>

namespace ImGui
{
    template<int steps>
    void bezier_table(ImVec2 P[4], ImVec2 results[steps + 1]);

    float BezierValue(float dt01, float P[4]);

    int Bezier(const char* label, float P[5]);

    void ShowBezierDemo();
    
}
#endif