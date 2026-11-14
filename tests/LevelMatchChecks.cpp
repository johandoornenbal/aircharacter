#include <juce_audio_utils/juce_audio_utils.h>
#include "../Source/CharacterControls.h"
#include "../Source/PurestWarm3Engine.h"
#include "../Source/DiscontapeityEngine.h"
#include "../Source/SweetenEngine.h"
#include "../Source/InflamerEngine.h"
#include "../Source/Coils2Engine.h"
#include "../Source/Density3Engine.h"
#include "../Source/WeaveEngine.h"
#include "../Source/LonghandEngine.h"
#include "../Source/PurestSaturationEngine.h"
#include "../Source/HypersoftEngine.h"
#include "../Source/CreatureEngine.h"
#include "../Source/Tube2Engine.h"


#include "../Source/LoudnessMatch.h"
#include <iostream>
#include <stdexcept>
void check(bool ok,const char* text) { if(!ok) throw std::runtime_error(text); }
juce::RangedAudioParameter& param(juce::AudioProcessor& p,const juce::String& id) {
    for(auto* v:p.getParameters()) if(auto* r=dynamic_cast<juce::RangedAudioParameter*>(v);r && r->paramID==id) return *r;
    throw std::runtime_error("Missing parameter "+id.toStdString());
}
juce::ValueTree snapshot(juce::AudioProcessor& p) {
    juce::MemoryBlock bytes; p.getStateInformation(bytes);
    auto xml=juce::AudioProcessor::getXmlFromBinary(bytes.getData(),static_cast<int>(bytes.getSize()));
    check(xml!=nullptr,"Invalid saved state"); return juce::ValueTree::fromXml(*xml);
}
void restore(juce::AudioProcessor& p,const juce::ValueTree& state) {
    auto xml=state.createXml(); juce::MemoryBlock bytes; juce::AudioProcessor::copyXmlToBinary(*xml,bytes);
    p.setStateInformation(bytes.getData(),static_cast<int>(bytes.getSize()));
}
juce::String key(int model,const char* property) { return controls::prefix(model)+property; }
struct UI {
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    explicit UI(juce::AudioProcessor& p):editor(p.createEditor()) {}
    juce::Component& component(const char* id) {
        for(auto* c:editor->getChildComponent(0)->getChildren()) if(c->getComponentID()==id) return *c;
        throw std::runtime_error("Missing UI component");
    }
    void click(const char* id) { dynamic_cast<juce::TextButton&>(component(id)).onClick(); }
    void select(int model) {
        for(auto* c:editor->getChildComponent(0)->getChildren()) if(auto* combo=dynamic_cast<juce::ComboBox*>(c);combo && combo->getName()=="Character model") combo->setSelectedId(model+1,juce::sendNotificationSync);
    }
    void refresh(int model) { select(model==0?1:0); select(model); }
};
void fill(juce::AudioBuffer<double>& b,int base) {
    for(int i=0;i<b.getNumSamples();++i) {
        const auto t=static_cast<double>(base+i)/48000.;
        b.setSample(0,i,.11*std::sin(t*2*3.141592653589793*997)+.03*std::sin(t*2*3.141592653589793*213));
        b.setSample(1,i,.07*std::sin(t*2*3.141592653589793*521));
    }
}
template<class Engine> void learn(juce::AudioProcessor& p,UI& ui,int model) {
    ui.select(model);
    param(p,controls::originalId(model)).setValueNotifyingHost(0);
    p.setProcessingPrecision(juce::AudioProcessor::doublePrecision);
    p.setRateAndBufferSizeDetails(48000,256); p.prepareToPlay(48000,256);
    Engine reference; reference.reset(48000);
    for(int i=0;i<controls::count(model);++i) reference.setParameter(i,param(p,controls::id(model,i)).getValue());
    LoudnessMatch expected; expected.prepare(48000); expected.start();
    juce::AudioBuffer<double> b(2,256),wet(2,256); juce::MidiBuffer midi;
    ui.click("learnMatch");
    int base=0;
    for(;base<48000*4;base+=256) {
        fill(b,base); wet.makeCopyOf(b);
        double* data[]{wet.getWritePointer(0),wet.getWritePointer(1)};
        wet.applyGain(juce::Decibels::decibelsToGain(static_cast<double>(param(p,controls::inputId(model)).convertFrom0to1(param(p,controls::inputId(model)).getValue()))));
        reference.processDoubleReplacing(data,data,256);
        wet.applyGain(juce::Decibels::decibelsToGain(static_cast<double>(param(p,controls::outputId(model)).convertFrom0to1(param(p,controls::outputId(model)).getValue()))));
        for(int i=0;i<256;++i) expected.sample(b.getSample(0,i),b.getSample(1,i),wet.getSample(0,i),wet.getSample(1,i));
        p.processBlock(b,midi);
        for(int c=0;c<2;++c) for(int i=0;i<256;++i) check(b.getSample(c,i)==wet.getSample(c,i),"Learning changes unmatched character output");
    }
    check(expected.finish()==LoudnessMatch::Result::success,"Reference passage not learnable");
    ui.click("learnMatch"); // Finish after four seconds, using the real audio-thread command.
    for(int block=0;block<8;++block,base+=256) {
        fill(b,base); wet.makeCopyOf(b);
        double* data[]{wet.getWritePointer(0),wet.getWritePointer(1)};
        wet.applyGain(juce::Decibels::decibelsToGain(static_cast<double>(param(p,controls::inputId(model)).convertFrom0to1(param(p,controls::inputId(model)).getValue()))));
        reference.processDoubleReplacing(data,data,256);
        wet.applyGain(juce::Decibels::decibelsToGain(static_cast<double>(param(p,controls::outputId(model)).convertFrom0to1(param(p,controls::outputId(model)).getValue()))));
        p.processBlock(b,midi);
        if(block==7) {
            auto saved=snapshot(p);
            const auto db=static_cast<float>(saved.getProperty(key(model,"matchDb")));
            check(static_cast<bool>(saved.getProperty(key(model,"hasMatch"))),"Learned match not saved");
            check(std::abs(db-expected.trimDb)<1.e-4,"Learned trim differs from original-engine measurement");
            check(std::abs(static_cast<double>(saved.getProperty(key(model,"inputLufs")))-expected.inputLufs)<1.e-4,"Measured input LUFS wrong");
            const auto gain=std::pow(10.,static_cast<double>(db)/20.);
            for(int c=0;c<2;++c) for(int i=0;i<256;++i) check(std::abs(b.getSample(c,i)-wet.getSample(c,i)*gain)<1.e-12,"Gain is not applied exactly once after the engine");
            std::cout<<controls::modelName(model)<<": learned "<<db<<" dB, expected "<<expected.trimDb<<'\n';
        }
    }
    ui.refresh(model);
    auto& label=dynamic_cast<juce::Label&>(ui.component("matchReadout"));
    check(label.getText().contains("dB") && !label.getText().contains("--"),"Learned readout missing");
    check(label.getFont().getHeight()<=14,"Applied gain readout too prominent");
    for(const char* id:{"learnMatch","resetMatch","matchEnabled","abOriginal","matchReadout","matchStatus"})
        check(ui.editor->getLocalBounds().contains(ui.component(id).getBounds()),"Matching controls outside editor");
    p.releaseResources();
}
template<class Engine> void verifyAppliedAfterSwitch(juce::AudioProcessor& p,int model) {
    param(p,"model").setValueNotifyingHost(param(p,"model").convertTo0to1(static_cast<float>(model)));
    Engine reference; reference.reset(48000);
    for(int i=0;i<controls::count(model);++i) reference.setParameter(i,param(p,controls::id(model,i)).getValue());
    juce::AudioBuffer<double> b(2,256),wet(2,256); juce::MidiBuffer midi;
    const auto saved=snapshot(p);
    const auto db=static_cast<double>(saved.getProperty(key(model,"matchDb")));
    const auto gain=param(p,controls::matchId(model)).getValue()>.5f?std::pow(10.,db/20.):1.;
    for(int block=0;block<8;++block) {
        fill(b,block*256); wet.makeCopyOf(b);
        double* data[]{wet.getWritePointer(0),wet.getWritePointer(1)};
        wet.applyGain(juce::Decibels::decibelsToGain(static_cast<double>(param(p,controls::inputId(model)).convertFrom0to1(param(p,controls::inputId(model)).getValue()))));
        reference.processDoubleReplacing(data,data,256);
        wet.applyGain(juce::Decibels::decibelsToGain(static_cast<double>(param(p,controls::outputId(model)).convertFrom0to1(param(p,controls::outputId(model)).getValue())))); p.processBlock(b,midi);
        if(block==7) for(int c=0;c<2;++c) for(int i=0;i<256;++i)
            check(std::abs(b.getSample(c,i)-wet.getSample(c,i)*gain)<1.e-12,"Model switch did not apply that model's learned adjustment/enable setting");
    }
    check(p.getLatencySamples()==controls::latency(model),"Model switch did not update host latency");
}
template<class T> void original(juce::AudioProcessor& p,int model) {
    param(p,"model").setValueNotifyingHost(param(p,"model").convertTo0to1(static_cast<float>(model)));
    param(p,controls::originalId(model)).setValueNotifyingHost(1);
    p.setProcessingPrecision(std::is_same_v<T,float>?juce::AudioProcessor::singlePrecision:juce::AudioProcessor::doublePrecision);
    p.prepareToPlay(48000,37);
    juce::AudioBuffer<T> b(2,37); juce::MidiBuffer midi;

    std::array<std::array<T,2>,2> history{};
    for(int block=0;block<8;++block) {
        const auto frames=block%3==0?1:block%3==1?2:37; b.setSize(2,frames);
        for(int c=0;c<2;++c) for(int i=0;i<frames;++i) b.setSample(c,i,static_cast<T>(.15*std::sin((block*37+i)*.07*(c+1))));
        juce::AudioBuffer<T> dry; dry.makeCopyOf(b);
        p.processBlock(b,midi);
        const auto delay=controls::latency(model);
        for(int i=0;i<frames;++i) {
            for(int c=0;c<2;++c) check(b.getSample(c,i)==(delay?history[static_cast<size_t>(delay-1)][static_cast<size_t>(c)]:dry.getSample(c,i)),"Original A/B delay is wrong");
            history[1]=history[0]; history[0]={dry.getSample(0,i),dry.getSample(1,i)};
        }
    }
    p.releaseResources();
}
int main() try {
    juce::ScopedJuceInitialiser_GUI init;
    std::unique_ptr<juce::AudioProcessor> p(createPluginFilter()); UI ui(*p);
    learn<PurestWarm3Engine>(*p,ui,0);
    learn<DiscontapeityEngine>(*p,ui,1);
    learn<SweetenEngine>(*p,ui,2);
    learn<InflamerEngine>(*p,ui,3);
    learn<Coils2Engine>(*p,ui,4);
    param(*p,controls::id(5,0)).setValueNotifyingHost(.4f);
    learn<Density3Engine>(*p,ui,5);
    learn<WeaveEngine>(*p,ui,6);
    learn<LonghandEngine>(*p,ui,7);
    learn<PurestSaturationEngine>(*p,ui,8);
    learn<HypersoftEngine>(*p,ui,9);
    learn<CreatureEngine>(*p,ui,10);
    learn<Tube2Engine>(*p,ui,11);
    ui.select(0); p->prepareToPlay(48000,256);
    verifyAppliedAfterSwitch<Density3Engine>(*p,5);
    verifyAppliedAfterSwitch<Coils2Engine>(*p,4);
    verifyAppliedAfterSwitch<InflamerEngine>(*p,3);
    verifyAppliedAfterSwitch<SweetenEngine>(*p,2);
    verifyAppliedAfterSwitch<DiscontapeityEngine>(*p,1);
    verifyAppliedAfterSwitch<PurestWarm3Engine>(*p,0);
    verifyAppliedAfterSwitch<WeaveEngine>(*p,6);
    verifyAppliedAfterSwitch<LonghandEngine>(*p,7);
    verifyAppliedAfterSwitch<PurestSaturationEngine>(*p,8);
    verifyAppliedAfterSwitch<HypersoftEngine>(*p,9);
    verifyAppliedAfterSwitch<CreatureEngine>(*p,10);
    verifyAppliedAfterSwitch<Tube2Engine>(*p,11);
    const auto saved=snapshot(*p);
    std::unique_ptr<juce::AudioProcessor> recalled(createPluginFilter()); restore(*recalled,saved);
    for(int m=0;m<controls::modelCount;++m) {
        check(static_cast<bool>(snapshot(*recalled).getProperty(key(m,"hasMatch"))),"Session lost learned match");
        original<float>(*p,m); original<double>(*p,m);
    }
    ui.select(0); ui.click("resetMatch");
    check(!static_cast<bool>(snapshot(*p).getProperty(key(0,"hasMatch"))),"Reset failed");
    check(static_cast<bool>(snapshot(*p).getProperty(key(1,"hasMatch"))),"Reset crossed model banks");
    std::cout<<"Per-model LUFS learning, fixed gain, model switching, original A/B and recall passed\n";
} catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
