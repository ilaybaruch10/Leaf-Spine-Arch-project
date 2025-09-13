//
// Generated file, do not edit! Created by opp_msgtool 6.1 from CustomMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "CustomMsg_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(CustomMsg)

CustomMsg::CustomMsg(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

CustomMsg::CustomMsg(const CustomMsg& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

CustomMsg::~CustomMsg()
{
}

CustomMsg& CustomMsg::operator=(const CustomMsg& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void CustomMsg::copy(const CustomMsg& other)
{
    this->srcAddr = other.srcAddr;
    this->dstAddr = other.dstAddr;
    this->sentAt = other.sentAt;
    this->receivedAt = other.receivedAt;
    this->msgId = other.msgId;
    this->msgSize = other.msgSize;
    this->hopCount = other.hopCount;
    this->seqNum = other.seqNum;
}

void CustomMsg::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->srcAddr);
    doParsimPacking(b,this->dstAddr);
    doParsimPacking(b,this->sentAt);
    doParsimPacking(b,this->receivedAt);
    doParsimPacking(b,this->msgId);
    doParsimPacking(b,this->msgSize);
    doParsimPacking(b,this->hopCount);
    doParsimPacking(b,this->seqNum);
}

void CustomMsg::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->srcAddr);
    doParsimUnpacking(b,this->dstAddr);
    doParsimUnpacking(b,this->sentAt);
    doParsimUnpacking(b,this->receivedAt);
    doParsimUnpacking(b,this->msgId);
    doParsimUnpacking(b,this->msgSize);
    doParsimUnpacking(b,this->hopCount);
    doParsimUnpacking(b,this->seqNum);
}

const char * CustomMsg::getSrcAddr() const
{
    return this->srcAddr.c_str();
}

void CustomMsg::setSrcAddr(const char * srcAddr)
{
    this->srcAddr = srcAddr;
}

const char * CustomMsg::getDstAddr() const
{
    return this->dstAddr.c_str();
}

void CustomMsg::setDstAddr(const char * dstAddr)
{
    this->dstAddr = dstAddr;
}

omnetpp::simtime_t CustomMsg::getSentAt() const
{
    return this->sentAt;
}

void CustomMsg::setSentAt(omnetpp::simtime_t sentAt)
{
    this->sentAt = sentAt;
}

omnetpp::simtime_t CustomMsg::getReceivedAt() const
{
    return this->receivedAt;
}

void CustomMsg::setReceivedAt(omnetpp::simtime_t receivedAt)
{
    this->receivedAt = receivedAt;
}

int CustomMsg::getMsgId() const
{
    return this->msgId;
}

void CustomMsg::setMsgId(int msgId)
{
    this->msgId = msgId;
}

int CustomMsg::getMsgSize() const
{
    return this->msgSize;
}

void CustomMsg::setMsgSize(int msgSize)
{
    this->msgSize = msgSize;
}

int CustomMsg::getHopCount() const
{
    return this->hopCount;
}

void CustomMsg::setHopCount(int hopCount)
{
    this->hopCount = hopCount;
}

int CustomMsg::getSeqNum() const
{
    return this->seqNum;
}

void CustomMsg::setSeqNum(int seqNum)
{
    this->seqNum = seqNum;
}

class CustomMsgDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_srcAddr,
        FIELD_dstAddr,
        FIELD_sentAt,
        FIELD_receivedAt,
        FIELD_msgId,
        FIELD_msgSize,
        FIELD_hopCount,
        FIELD_seqNum,
    };
  public:
    CustomMsgDescriptor();
    virtual ~CustomMsgDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(CustomMsgDescriptor)

CustomMsgDescriptor::CustomMsgDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(CustomMsg)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

CustomMsgDescriptor::~CustomMsgDescriptor()
{
    delete[] propertyNames;
}

bool CustomMsgDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<CustomMsg *>(obj)!=nullptr;
}

const char **CustomMsgDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *CustomMsgDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int CustomMsgDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int CustomMsgDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_srcAddr
        FD_ISEDITABLE,    // FIELD_dstAddr
        FD_ISEDITABLE,    // FIELD_sentAt
        FD_ISEDITABLE,    // FIELD_receivedAt
        FD_ISEDITABLE,    // FIELD_msgId
        FD_ISEDITABLE,    // FIELD_msgSize
        FD_ISEDITABLE,    // FIELD_hopCount
        FD_ISEDITABLE,    // FIELD_seqNum
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *CustomMsgDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "srcAddr",
        "dstAddr",
        "sentAt",
        "receivedAt",
        "msgId",
        "msgSize",
        "hopCount",
        "seqNum",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int CustomMsgDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "srcAddr") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "dstAddr") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "sentAt") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "receivedAt") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "msgId") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "msgSize") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "hopCount") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "seqNum") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *CustomMsgDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_srcAddr
        "string",    // FIELD_dstAddr
        "omnetpp::simtime_t",    // FIELD_sentAt
        "omnetpp::simtime_t",    // FIELD_receivedAt
        "int",    // FIELD_msgId
        "int",    // FIELD_msgSize
        "int",    // FIELD_hopCount
        "int",    // FIELD_seqNum
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **CustomMsgDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *CustomMsgDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int CustomMsgDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    CustomMsg *pp = omnetpp::fromAnyPtr<CustomMsg>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void CustomMsgDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    CustomMsg *pp = omnetpp::fromAnyPtr<CustomMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'CustomMsg'", field);
    }
}

const char *CustomMsgDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    CustomMsg *pp = omnetpp::fromAnyPtr<CustomMsg>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string CustomMsgDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    CustomMsg *pp = omnetpp::fromAnyPtr<CustomMsg>(object); (void)pp;
    switch (field) {
        case FIELD_srcAddr: return oppstring2string(pp->getSrcAddr());
        case FIELD_dstAddr: return oppstring2string(pp->getDstAddr());
        case FIELD_sentAt: return simtime2string(pp->getSentAt());
        case FIELD_receivedAt: return simtime2string(pp->getReceivedAt());
        case FIELD_msgId: return long2string(pp->getMsgId());
        case FIELD_msgSize: return long2string(pp->getMsgSize());
        case FIELD_hopCount: return long2string(pp->getHopCount());
        case FIELD_seqNum: return long2string(pp->getSeqNum());
        default: return "";
    }
}

void CustomMsgDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CustomMsg *pp = omnetpp::fromAnyPtr<CustomMsg>(object); (void)pp;
    switch (field) {
        case FIELD_srcAddr: pp->setSrcAddr((value)); break;
        case FIELD_dstAddr: pp->setDstAddr((value)); break;
        case FIELD_sentAt: pp->setSentAt(string2simtime(value)); break;
        case FIELD_receivedAt: pp->setReceivedAt(string2simtime(value)); break;
        case FIELD_msgId: pp->setMsgId(string2long(value)); break;
        case FIELD_msgSize: pp->setMsgSize(string2long(value)); break;
        case FIELD_hopCount: pp->setHopCount(string2long(value)); break;
        case FIELD_seqNum: pp->setSeqNum(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CustomMsg'", field);
    }
}

omnetpp::cValue CustomMsgDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    CustomMsg *pp = omnetpp::fromAnyPtr<CustomMsg>(object); (void)pp;
    switch (field) {
        case FIELD_srcAddr: return pp->getSrcAddr();
        case FIELD_dstAddr: return pp->getDstAddr();
        case FIELD_sentAt: return pp->getSentAt().dbl();
        case FIELD_receivedAt: return pp->getReceivedAt().dbl();
        case FIELD_msgId: return pp->getMsgId();
        case FIELD_msgSize: return pp->getMsgSize();
        case FIELD_hopCount: return pp->getHopCount();
        case FIELD_seqNum: return pp->getSeqNum();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'CustomMsg' as cValue -- field index out of range?", field);
    }
}

void CustomMsgDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    CustomMsg *pp = omnetpp::fromAnyPtr<CustomMsg>(object); (void)pp;
    switch (field) {
        case FIELD_srcAddr: pp->setSrcAddr(value.stringValue()); break;
        case FIELD_dstAddr: pp->setDstAddr(value.stringValue()); break;
        case FIELD_sentAt: pp->setSentAt(value.doubleValue()); break;
        case FIELD_receivedAt: pp->setReceivedAt(value.doubleValue()); break;
        case FIELD_msgId: pp->setMsgId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_msgSize: pp->setMsgSize(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_hopCount: pp->setHopCount(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_seqNum: pp->setSeqNum(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CustomMsg'", field);
    }
}

const char *CustomMsgDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr CustomMsgDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    CustomMsg *pp = omnetpp::fromAnyPtr<CustomMsg>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void CustomMsgDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    CustomMsg *pp = omnetpp::fromAnyPtr<CustomMsg>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'CustomMsg'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

