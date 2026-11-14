#pragma once
#include <array>
namespace controls {
constexpr int modelCount=12;
constexpr std::array<const char*,modelCount> models{"PurestWarm3","Discontapeity","Sweeten","Inflamer","Coils2","Density3","Weave","Longhand","PurestSaturation","Hypersoft","Creature","Tube2"};
inline juce::String modelName(int m) { return models[static_cast<size_t>(m)]; }
inline int count(int m) { constexpr int counts[]{1,1,1,3,3,4,3,3,2,4,3,2}; return counts[m]; }
inline juce::String prefix(int m) { return modelName(m).toLowerCase()+"_"; }
inline juce::String id(int m,int i) { return prefix(m)+juce::String(i); }
inline juce::String matchId(int m) { return prefix(m)+"matchEnabled"; }
inline juce::String originalId(int m) { return prefix(m)+"original"; }
inline juce::String inputId(int m) { return prefix(m)+"inputTrim"; }
inline juce::String outputId(int m) { return prefix(m)+"outputTrim"; }
inline juce::String linkId(int m) { return prefix(m)+"linkIO"; }
inline bool hasFrequencies(int) { return false; }
inline int primaryCount(int m) { return count(m); }
inline int primaryIndex(int,int column) { return column; }
inline int latency(int) { return 0; }
inline juce::StringArray choices() { juce::StringArray a; for(auto* n:models) a.add(n); return a; }
inline const char* name(int m,int i) { static const char* names[12][4]={{"Asym"},{"More"},{"Sweeten"},{"Drive","Curve","Effect"},{"Saturate","Cheapness","Dry/Wet"},{"Density","Highpas","Output","Dry/Wet"},{"Input","Smooth","Output"},{"Input","Output","Dry/Wet"},{"Input","Output"},{"Input","Depth","Bright","Output"},{"Drive","Depth","Inv/Wet"},{"Input","Tube"}}; return names[m][i]; }
inline float defaultValue(int m,int i) { static const float defaults[12][4]={{0.5f},{0.0f},{0.0f},{0.5f,0.5f,1.0f},{0.0f,0.0f,1.0f},{0.0f,0.0f,1.0f,1.0f},{0.1f,0.5f,1.0f},{0.445f,1.0f,1.0f},{0.1f,1.0f},{0.5f,0.5f,0.5f,0.5f},{0.26f,0.26f,1.0f},{0.5f,0.5f}}; return defaults[m][i]; }
inline juce::String description(int m) { return modelName(m).toUpperCase()+"  /  CHARACTER"; }
inline juce::String tip(int,int) { return "Original Airwindows normalized control and mapping."; }
inline juce::String display(int,double v) { return juce::String(v*100.,1)+" %"; }
inline double parse(int,const juce::String& t) { return juce::jlimit(0.,1.,t.getDoubleValue()/100.); }
}
