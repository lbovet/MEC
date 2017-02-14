#include "mec_scaler.h"

namespace mec {


static Scales scaleManager;




Scales::Scales() {
    // scales_["major"] = ScaleArray( {0.0, 2.0, 4.0, 5.0, 7.0, 9.0, 11.0, 12.0} );
    // scales_["minor"] = ScaleArray( {0.0, 2.0, 3.0, 5.0, 7.0, 8.0, 10.0, 12.0} );
    // scales_["chromatic"] = ScaleArray( { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0 } );
}
Scales::~Scales() {
    ;
}
bool    Scales::load(const Preferences& prefs) {
    if (!prefs.valid()) return false;

    std::vector<std::string> keys = prefs.getKeys();
    for (std::string k : keys) {
        ScaleArray scale;
        void* a = prefs.getArray(k);
        for (int i = 0; i < prefs.getArraySize(a); i++) {
            float n = (float) prefs.getArrayDouble(a, i, 0.0f);
            scale.push_back(n);
        }
        if (scale.size() > 0) {
            scales_[k] = scale;
        }
    }
    return true;
}

bool Scales::init(const Preferences& p) {
    return scaleManager.load(p);
}

const ScaleArray& Scales::getScale(const std::string& name) {
    return scaleManager.scales_[name];
}


Scaler::Scaler() :
    id_(-1),
    scale_(Scales::getScale("chromatic")),
    tonic_(0.0f),
    rowOffset_(0.0f), columnOffset_(0.0f) {
    ;
}
Scaler::~Scaler() {
    ;
}

bool Scaler::load(const Preferences& prefs) {
    return true;
}

MusicalTouch Scaler::mapToNote(const Touch& t) {
    // see notes above, important 12 note scale has 13 entries!
    // think , row = string , column = fret
    int ix = t.c_;
    int sz = scale_.size() - 1;
    int n = ix % sz;

    float fx = t.c_ - ix;

    float sn0 = scale_[n];
    float sn1 = scale_[n + 1];

    float sn = sn0 + ((sn1 - sn0 ) * fx);

    float note = (( ix / sz) * scale_[sz] ) + sn;

    note = columnOffset_ + (t.r_ * rowOffset_) + tonic_ + note;

    return MusicalTouch(t, note, id_);
}

float Scaler::getTonic() {
    return tonic_;
}
void  Scaler::setTonic(float f) {
    tonic_ = f;
}

float Scaler::getRowOffset() {
    return rowOffset_;
}

void  Scaler::setRowOffset(float f) {
    rowOffset_ = f;
}

float Scaler::getColumnOffset() {
    return columnOffset_;
}

void  Scaler::setColumnOffset(float f) {
    columnOffset_ = f;
}


void  Scaler::setScale(const ScaleArray& scale) {
    scale_ = scale;
}
void  Scaler::setScale(const std::string& name) {
    scale_ = Scales::getScale(name);
}

const ScaleArray& Scaler::getScale() {
    return scale_;
}


} // namespace
