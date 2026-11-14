#include <juce_audio_utils/juce_audio_utils.h>
#include "../Source/CharacterControls.h"
#include "../Source/MeterBallistics.h"
#include <iostream>
#include <stdexcept>
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

void require(bool ok,const char* message) { if(!ok) throw std::runtime_error(message); }
juce::RangedAudioParameter& parameter(juce::AudioProcessor& p,const juce::String& id) {
    for(auto* v:p.getParameters()) if(auto* r=dynamic_cast<juce::RangedAudioParameter*>(v);r && r->paramID==id) return *r;
    throw std::runtime_error("Missing parameter "+id.toStdString());
}
void select(juce::AudioProcessor& p,int model) { parameter(p,"model").setValueNotifyingHost(parameter(p,"model").convertTo0to1(static_cast<float>(model))); }
template<class Engine,class T> void compare(juce::AudioProcessor& p,int model,double rate) {
    select(p,model);
    p.setProcessingPrecision(std::is_same_v<T,float>?juce::AudioProcessor::singlePrecision:juce::AudioProcessor::doublePrecision);
    p.setRateAndBufferSizeDetails(rate,513); p.prepareToPlay(rate,513);
    Engine reference; reference.reset(rate);
    for(int i=0;i<controls::count(model);++i) {
        const auto value=static_cast<float>((i*3+4)%11)*.1f;
        parameter(p,controls::id(model,i)).setValueNotifyingHost(value); reference.setParameter(i,value);
    }
    juce::MidiBuffer midi;
    for(int block=0;block<20;++block) {
        const auto n=std::array<int,4>{1,37,257,513}[static_cast<size_t>(block%4)];
        juce::AudioBuffer<T> b(2,n),expected(2,n);
        for(int c=0;c<2;++c) for(int i=0;i<n;++i) {
            const auto value=static_cast<T>(.3*std::sin((block*513+i)*.05*(c+1)));
            b.setSample(c,i,value); expected.setSample(c,i,value);
        }
        p.processBlock(b,midi);
        T* ptr[]{expected.getWritePointer(0),expected.getWritePointer(1)};
        if constexpr(std::is_same_v<T,float>) reference.processReplacing(ptr,ptr,n);
        else reference.processDoubleReplacing(ptr,ptr,n);
        for(int c=0;c<2;++c) for(int i=0;i<n;++i)
            require(std::isfinite(b.getSample(c,i)) && b.getSample(c,i)==expected.getSample(c,i),"Wrapper altered engine output at unity trims");
    }
    require(p.getLatencySamples()==controls::latency(model),"Wrong host latency"); p.releaseResources();
}

int main() try {
    juce::ScopedJuceInitialiser_GUI init;
    std::unique_ptr<juce::AudioProcessor> p(createPluginFilter());
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<PurestWarm3Engine,float>(*p,0,rate); compare<PurestWarm3Engine,double>(*p,0,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<DiscontapeityEngine,float>(*p,1,rate); compare<DiscontapeityEngine,double>(*p,1,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<SweetenEngine,float>(*p,2,rate); compare<SweetenEngine,double>(*p,2,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<InflamerEngine,float>(*p,3,rate); compare<InflamerEngine,double>(*p,3,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<Coils2Engine,float>(*p,4,rate); compare<Coils2Engine,double>(*p,4,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<Density3Engine,float>(*p,5,rate); compare<Density3Engine,double>(*p,5,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<WeaveEngine,float>(*p,6,rate); compare<WeaveEngine,double>(*p,6,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<LonghandEngine,float>(*p,7,rate); compare<LonghandEngine,double>(*p,7,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<PurestSaturationEngine,float>(*p,8,rate); compare<PurestSaturationEngine,double>(*p,8,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<HypersoftEngine,float>(*p,9,rate); compare<HypersoftEngine,double>(*p,9,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<CreatureEngine,float>(*p,10,rate); compare<CreatureEngine,double>(*p,10,rate); }
    for(double rate:{44100.,48000.,96000.,192000.}) { compare<Tube2Engine,float>(*p,11,rate); compare<Tube2Engine,double>(*p,11,rate); }
    std::unique_ptr<juce::AudioProcessorEditor> editor(p->createEditor());
    auto* panel=editor->getChildComponent(0);
    auto component=[&](const char* id)->juce::Component& {
        for(auto* c:panel->getChildren()) if(c->getComponentID()==id) return *c;
        throw std::runtime_error("Missing component");
    };
    juce::ComboBox* selector=nullptr;
    for(auto* c:panel->getChildren()) if(auto* combo=dynamic_cast<juce::ComboBox*>(c);combo && combo->getName()=="Character model") selector=combo;
    require(selector!=nullptr,"Missing model selector");
    for(int m=0;m<controls::modelCount;++m) {
        selector->setSelectedId(m+1,juce::sendNotificationSync);
        int visible=0;
        juce::Slider *input=nullptr,*output=nullptr;
        juce::ToggleButton* link=nullptr;
        for(auto* c:panel->getChildren()) {
            if(auto* t=dynamic_cast<juce::ToggleButton*>(c)) link=t;
            if(auto* slider=dynamic_cast<juce::Slider*>(c);slider && slider->isVisible()) {
                ++visible;
                if(slider->getName()=="Input trim") input=slider;
                else if(slider->getName()=="Output trim") output=slider;
                else {
                    slider->setValue(.31,juce::sendNotificationSync);
                    bool found=false;
                    for(int i=0;i<controls::count(m);++i) if(slider->getName()==controls::name(m,i)) {
                        require(std::abs(parameter(*p,controls::id(m,i)).getValue()-.31f)<1.e-6,"Wrong bank binding"); found=true;
                        require(slider->getDoubleClickReturnValue()==controls::defaultValue(m,i),"Wrong default");
                    }
                    require(found,"Unexpected knob");
                }
            }
            if(c->isVisible()) require(panel->getLocalBounds().contains(c->getBounds()),"Control outside panel");
        }
        require(visible==controls::count(m)+2,"Wrong number of visible controls");
        require(input && output && link,"Missing trim/link controls");
        input->setValue(0,juce::sendNotificationSync); output->setValue(0,juce::sendNotificationSync);
        link->setToggleState(true,juce::sendNotificationSync);
        input->setValue(6,juce::sendNotificationSync);
        require(std::abs(output->getValue()+6)<1.e-5,"Input/output linking failed");
        dynamic_cast<juce::TextButton&>(component("resetParameters")).onClick();
        for(int i=0;i<controls::count(m);++i) require(std::abs(parameter(*p,controls::id(m,i)).getValue()-controls::defaultValue(m,i))<1.e-6,"Reset lost original default");
        auto& sizes=dynamic_cast<juce::ComboBox&>(component("uiSize"));
        for(int size:{1,2,3}) {
            sizes.setSelectedId(size,juce::sendNotificationSync);
            const auto scale=size==1?.75:size==3?1.25:1.;
            require(editor->getWidth()==juce::roundToInt(980*scale),"UI size failed");
        }
        if(const char* dir=std::getenv("AIRCHARACTER_PREVIEW_DIR")) {
            juce::File folder{juce::String(dir)}; folder.createDirectory();
            auto image=panel->createComponentSnapshot(panel->getLocalBounds());
            auto stream=folder.getChildFile(controls::modelName(m)+".png").createOutputStream();
            require(stream!=nullptr,"Cannot open preview"); stream->setPosition(0); stream->truncate(); juce::PNGImageFormat png; png.writeImageToStream(image,*stream);
        }
    }
    juce::MemoryBlock saved; p->getStateInformation(saved);
    std::unique_ptr<juce::AudioProcessor> recalled(createPluginFilter()); recalled->setStateInformation(saved.getData(),static_cast<int>(saved.getSize()));
    for(auto* raw:p->getParameters()) if(auto* v=dynamic_cast<juce::RangedAudioParameter*>(raw)) require(std::abs(parameter(*recalled,v->paramID).getValue()-v->getValue())<1.e-6,"Session lost parameter");
    MeterBallistics meter;
    for(int i=0;i<120;++i) meter.advance(.5f,1./60.);
    require(std::abs(meter.getLevelDb()+6.0206f)<.1f,"Meter calibration failed");
    for(int i=0;i<300;++i) meter.advance(0,1./60.);
    require(meter.getLevelDb()<-40,"Meter failed to release");
    std::cout<<"Float/double routing, UI banks and sizes, linking, reset, meters and recall passed\n";
} catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 1; }
