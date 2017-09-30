#include "Parameter.h"

#include <iostream>

namespace oKontrol {

// PT_Invalid
// PT_Float
// PT_Pct
// PT_LinHz
// PT_mSec
// PT_Semis
// PT_Boolean


int operator>(const ParamValue& lhs, const ParamValue& rhs) {
    if (lhs.type() != rhs.type()) return lhs.type() > rhs.type();
    switch (lhs.type()) {
    case ParamValue::T_Float : {
        return lhs.floatValue() > rhs.floatValue();
    }
    case ParamValue::T_String:
        return lhs.stringValue() > rhs.stringValue();
    default:
        ;
    }
    return lhs.stringValue() > rhs.stringValue();
}

int operator<(const ParamValue& lhs, const ParamValue& rhs) {
    if (lhs.type() != rhs.type()) return lhs.type() < rhs.type();
    switch (lhs.type()) {
    case ParamValue::T_Float : {
        return lhs.floatValue() < rhs.floatValue();
    }
    case ParamValue::T_String:
        return lhs.stringValue() < rhs.stringValue();
    default:
        ;
    }
    return lhs.stringValue() > rhs.stringValue();
}

int operator!=(const ParamValue& lhs, const ParamValue& rhs) {
    return ! (lhs == rhs);
}

int operator==(const ParamValue& lhs, const ParamValue& rhs) {
    if (lhs.type() != rhs.type()) return false;
    switch (lhs.type()) {
    case ParamValue::T_Float : {
        return lhs.floatValue() == rhs.floatValue();
    }
    case ParamValue::T_String:
        return lhs.stringValue() == rhs.stringValue();
    default:
        ;
    }
    return lhs.stringValue() == rhs.stringValue();
}


std::shared_ptr<Parameter> createParameter(const std::string& t) {
            if (t == "float")  return std::make_shared<Parameter_Float>(PT_Float);
    else    if (t == "bool")   return std::make_shared<Parameter_Boolean>(PT_Boolean);
    else    if (t == "pct")   return std::make_shared<Parameter_Percent>(PT_Pct);
    else    if (t == "hz")   return std::make_shared<Parameter_LinearHz>(PT_LinHz);
    else    if (t == "msec")   return std::make_shared<Parameter_Time>(PT_mSec);

    std::cerr << "parameter type not found: " << t << std::endl;

    return std::make_shared<Parameter>(PT_Invalid);
}

void Parameter::createArgs(std::vector<ParamValue>& args) const {
    switch(type_) {
        case PT_Float: args.push_back("float");break;
        case PT_Boolean: args.push_back("bool");break;
        case PT_Pct: args.push_back("pct");break;
        case PT_LinHz: args.push_back("hz");break;
        case PT_mSec: args.push_back("msec");break;
        default:
            args.push_back("unsupported");
    }
    args.push_back(ParamValue(id_));
    args.push_back(ParamValue(type_));
}


// factory for all type creation
std::shared_ptr<Parameter> Parameter::create(const std::vector<ParamValue>& args) {
    unsigned pos = 0;
    std::shared_ptr<Parameter> p;

    if ( args.size() > pos && args[pos].type() == ParamValue::T_String ) p = createParameter(args[pos++].stringValue());
    try {
        if (p->type() != PT_Invalid) p->init(args, pos);
    } catch (const std::runtime_error& e) {
        // perhaps report here why
        std::cerr << "error: " << e.what() << std::endl;
        p->type_ = PT_Invalid;
    }

    return p;
}

static void throwError(const std::string id, const char* what) {
    std::string w = id + std::string(what);
    std::runtime_error(w.c_str()); 
}

// Parameter : type id displayname
Parameter::Parameter(ParameterType type) : type_(type) , current_(0.0f) {
   ;
}

void Parameter::init(const std::vector<ParamValue>& args, unsigned& pos) {
    if ( args.size() > pos && args[pos].type() == ParamValue::T_String ) id_ = args[pos++].stringValue() ; else throwError("null", "missing id");
    if ( args.size() > pos && args[pos].type() == ParamValue::T_String ) displayName_ = args[pos++].stringValue() ; else throwError(id(),"missing displayName");
}


const std::string& Parameter::displayName() const  {
    return displayName_;
}

std::string Parameter::displayValue() const {
    static std::string sNullString = "";
    return sNullString;
}

const std::string& Parameter::displayUnit() const {
    static std::string sNullString = "";
    return sNullString;
}


ParamValue Parameter::calcRelative(float f) {
    switch (current_.type()) {
    case ParamValue::T_Float : {
        float v = current_.floatValue() + f;
        return calcFloat(v);
    }
    case ParamValue::T_String:
    default:
        ;
    }
    return current_;
}

ParamValue Parameter::calcFloat(float f) {
    switch (current_.type()) {
    case ParamValue::T_Float : {
        return ParamValue(f);
    }
    case ParamValue::T_String:
    default:
        ;
    }
    return current_;
}

ParamValue Parameter::calcMidi(int midi) {
    float f = (float) midi / 127.0;
    return calcFloat(f);
}


bool  Parameter::change(const ParamValue& c) {
    if (current_ != c) {
        current_ = c;
        return true;
    }
    return false;
}

// Parameter_Float : [parameter] min max default
Parameter_Float::Parameter_Float(ParameterType type) :  Parameter(type) {
    ;
}

void Parameter_Float::init(const std::vector<ParamValue>& args, unsigned& pos) {
    Parameter::init(args, pos);
    if ( args.size() > pos && args[pos].type() == ParamValue::T_Float  ) min_ = args[pos++].floatValue() ; else throwError(id(),"missing min");
    if ( args.size() > pos && args[pos].type() == ParamValue::T_Float  ) max_ = args[pos++].floatValue() ; else throwError(id(),"missing max");
    if ( args.size() > pos && args[pos].type() == ParamValue::T_Float ) current_ = def_ = args[pos++].floatValue() ; else throwError(id(),"missing def");
}

void Parameter_Float::createArgs(std::vector<ParamValue>& args) const {
    Parameter::createArgs(args);
    args.push_back(ParamValue(min_));
    args.push_back(ParamValue(max_));
    args.push_back(ParamValue(def_));
}

std::string Parameter_Float::displayValue() const {
    char numbuf[11];
    sprintf(numbuf, "%10.2f", current_.floatValue());
    return std::string(numbuf);
}


// Parameter_Float can assume float value
ParamValue Parameter_Float::calcRelative(float f) {
    return calcFloat(current().floatValue() + f);
}

ParamValue Parameter_Float::calcMidi(int midi) {
    float f = (float) midi / 127.0;
    return calcFloat(f);
}

ParamValue Parameter_Float::calcFloat(float f) {
    float v = (f * (max() - min())) + min();
    v = std::max(v, min());
    v = std::min(v, max());
    return ParamValue(v);
}

bool  Parameter_Float::change(const ParamValue& c) {
    switch (current_.type()) {
    case ParamValue::T_Float  : {
        float v = c.floatValue();
        v = std::max(v, min());
        v = std::min(v, max());
        return Parameter::change(ParamValue(v));
    }
    case ParamValue::T_String:
    default:
        ;
    }
    return false;
}

// Parameter_Boolean :  [parameter] default
Parameter_Boolean::Parameter_Boolean( ParameterType type) : Parameter (type) {
    ;
}

std::string Parameter_Boolean::displayValue() const {
    if (current_.floatValue() > 0.5) {
        return "on";
    } else {
        return "off";
    }
}



void Parameter_Boolean::init(const std::vector<ParamValue>& args, unsigned& pos) {
    Parameter::init(args, pos);
    if ( args.size() > pos && args[pos].type() == ParamValue::T_String ) {
        if (args[pos].stringValue() == "true")
            current_ = def_ = true;
        else
            current_ = def_ = false;
        pos++;
    } else
        throwError(id(),"missing def");
}

void Parameter_Boolean::createArgs(std::vector<ParamValue>& args) const {
    Parameter::createArgs(args);
    args.push_back(def_ ? ParamValue("true") : ParamValue("false"));
}

bool  Parameter_Boolean::change(const ParamValue& c) {
    switch (current_.type()) {
    case ParamValue::T_Float  : {
        float v = c.floatValue() > 0.5 ? 1.0 : 0.0;
        return Parameter::change(ParamValue(v));
    }
    case ParamValue::T_String:
    default:
        ;
    }
    return false;
}

ParamValue Parameter_Boolean::calcRelative(float f) {
    return calcFloat(current_.floatValue() + f);
}

ParamValue Parameter_Boolean::calcFloat(float f) {
    return ParamValue(f > 0.5 ? 1.0 : 0.0);
}

ParamValue Parameter_Boolean::calcMidi(int midi) {
    return ParamValue(midi > 63 ? 1.0 : 0.0);;
}


// simple derivative types
const std::string& Parameter_Percent::displayUnit() const {
    static std::string sUnit="%";
    return sUnit;
}

const std::string& Parameter_LinearHz::displayUnit() const {
    static std::string sUnit="Hz";
    return sUnit;
}

const std::string& Parameter_Time::displayUnit() const {
    static std::string sUnit="mSec";
    return sUnit;
}



} //namespace