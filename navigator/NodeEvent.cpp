#include "NodeEvent.h"

NodeEvent::NodeEvent(time_t timestamp, Type type, Datas* datas) :
    mTimestamp(timestamp),
    mType(type),
    mDatas(datas)
{
}

//-------------------------------------------------------------------------------------
NodeEvent::~NodeEvent()
{
    delete mDatas;
}

//-------------------------------------------------------------------------------------
time_t NodeEvent::getTimestamp()
{
    return mTimestamp;
}

//-------------------------------------------------------------------------------------
NodeEvent::Type NodeEvent::getType()
{
    return mType;
}

//-------------------------------------------------------------------------------------
NodeEvent::Datas* NodeEvent::getDatas()
{
    return mDatas;
}
