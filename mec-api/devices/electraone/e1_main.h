#pragma once


#include "e1_basemode.h"


namespace mec {

struct ParameterID {
    ParameterID(unsigned ei, const Kontrol::EntityId &r, const Kontrol::EntityId &m, const Kontrol::EntityId &p);
    unsigned eId_;
    const Kontrol::EntityId rack_;
    const Kontrol::EntityId module_;
    const Kontrol::EntityId parameter_;
};

class ElectraOneMainMode : public ElectraOneBaseMode {
public:
    explicit ElectraOneMainMode(ElectraOne &p) : ElectraOneBaseMode(p){ ; }

    bool init() override { return true; };
    void activate() override;

    void rack(Kontrol::ChangeSource, const Kontrol::Rack &) override { ; }

    void module(Kontrol::ChangeSource source, const Kontrol::Rack &rack, const Kontrol::Module &module) override;
    void changed(Kontrol::ChangeSource, const Kontrol::Rack &, const Kontrol::Module &,
                 const Kontrol::Parameter &) override;
    void page(Kontrol::ChangeSource source, const Kontrol::Rack &rack, const Kontrol::Module &module,
              const Kontrol::Page &page) override;

    void param(Kontrol::ChangeSource, const Kontrol::Rack &, const Kontrol::Module &,
               const Kontrol::Parameter &) override { ; }

    void publishStart(Kontrol::ChangeSource, unsigned numRacks)  override;
    void publishRackFinished(Kontrol::ChangeSource, const Kontrol::Rack &) override;

    void resource(Kontrol::ChangeSource, const Kontrol::Rack &, const std::string &, const std::string &) override { ; }

    void deleteRack(Kontrol::ChangeSource, const Kontrol::Rack &) override { ; }

    void loadModule(Kontrol::ChangeSource, const Kontrol::Rack &, const Kontrol::EntityId &,
                    const std::string &) override;
    void activeModule(Kontrol::ChangeSource, const Kontrol::Rack &, const Kontrol::Module &) override;
    void loadPreset(Kontrol::ChangeSource source, const Kontrol::Rack &rack, std::string preset) override;


    void onButton(unsigned id, unsigned value) override;
    void onEncoder(unsigned id, int value) override;

protected:
    virtual unsigned displayParamNum(unsigned pageid, unsigned ctrlsetid, unsigned kpageid, unsigned pos,
                                 unsigned pid,const Kontrol::Parameter &p, bool local);

    void changeParam(unsigned idx, int relValue);
    void display();

    std::string moduleType_;
    unsigned selectedPresetIdx_ = 0;
    unsigned selectedModuleIdx_ = 0;
    unsigned rackPublishing_ = 0;

    std::unordered_map<unsigned, std::shared_ptr<ParameterID>> paramMap_;

}; //ElectraOneMainMode

} //mec