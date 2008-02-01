#include "HandleEvtMethod.h"
#include "XmlDatas.h"
#include <sstream>
/////////////////////////////////////
#include "Node.h"
/////////////////////////////////////

using namespace Solipsis;

HandleEvtMethod::HandleEvtMethod(NodeServer* s)
    : AbstractNodeMethod("HandleEvt", "Handle an event post by Solipsis Peer", s)
{
#ifdef ULXR
    mServer->getInternalServer()->addMethod(ulxr::make_method(*this, &HandleEvtMethod::execute),
                                            ulxr::Signature() << ulxr::Struct(),
                                            ULXR_PCHAR(mName),
                                            ulxr::Signature() << ulxr::RpcString(),
                                            ULXR_PCHAR(mHelp));
#endif
}

#ifdef ULXR
ulxr::MethodResponse HandleEvtMethod::execute(const ulxr::MethodCall& calldata)
{
    // Parse parameters
    ulxr::RpcString nodeIdULXR = calldata.getParam(0);
    std::string nodeId = nodeIdULXR.getString();
    if (nodeId.empty())
        return ulxr::MethodResponse(1, ulxr_i18n(ULXR_PCHAR("Invalid parameters !")));

    // Process
    std::string xmlResp;
    INodeClient::RetCode retCode = _execute(nodeId, xmlResp);

    // Format response
    ulxr::MethodResponse resp;
    ulxr::Struct respStruct;
    respStruct << ulxr::make_member(ULXR_PCHAR("retCode"), ulxr::Integer(retCode))
               << ulxr::make_member(ULXR_PCHAR("response"), ulxr::RpcString(xmlResp.c_str()));
    resp.setResult(respStruct);
    return resp;
}
#else
void HandleEvtMethod::execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
{
    // Parse parameters
    int offset = 0;
    XmlRpc::XmlRpcValue paramsTable(params.toXml(), &offset);
    std::string nodeId = paramsTable[0];
    if (nodeId.empty())
    {
        throw XmlRpc::XmlRpcException("Invalid parameters !");
    }

    // Process
    std::string xmlResp;
    INodeClient::RetCode retCode = _execute(nodeId, xmlResp);

    // Format response
    result["retCode"] = retCode;
    result["response"] = xmlResp.c_str();
}
#endif

INodeClient::RetCode HandleEvtMethod::_execute(const INodeClient::NodeId& nodeId, std::string& xmlRespStr)
{
    xmlRespStr.clear();

    /////////////////////////////////////
    Node& node = Node::getSingleton();
    time_t now;

    if (nodeId.compare("00000001") != 0)
        return INodeClient::RCNoEvt;

    time(&now);
    switch (node.mState)
    {
    case -1:
        node.mFirstEvtDate = now;
        node.mState++;
#ifdef ULXR
        Sleep(11000);
        time(&now);
#else
        break;
    case 0:
#endif
        if (now - node.mFirstEvtDate > 10)
        {
            xmlRespStr = "\
<solipsis>\
 <evt type=\"0\">\
  <object uid=\"11111111\" type=\"0\" name=\"User\">\
   <position x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
   <orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
   <content>\
    <lod level=\"0\">\
     <files>\
      <file name=\"User.xml\" />\
      <file name=\"models/salamandra.mesh\" />\
      <file name=\"models/salamandra.mesh.skeleton\" />\
      <file name=\"materials/scripts/salamandra.material\" />\
      <file name=\"materials/textures/LightMapSalamandra2.tga\" />\
     </files>\
    </lod>\
   </content>\
  </object>\
 </evt>\
</solipsis>\
";
            node.mState++;
        }
        break;
    case 1:
#ifdef ULXR
        Sleep(11000);
        time(&now);
#endif
        if (now - node.mFirstEvtDate > 20)
        {
            xmlRespStr = "\
<solipsis>\
 <evt type=\"0\">\
  <object uid=\"11112222\" type=\"1\" name=\"Deltastation1\">\
   <position x=\"0.0\" y=\"100.0\" z=\"0.0\" />\
   <orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
   <content>\
    <lod level=\"0\">\
     <files>\
      <file name=\"Deltastation1.xml\" />\
      <file name=\"scenes/Deltastation1.osm\" />\
      <file name=\"scenes/gun6.mesh\" />\
      <file name=\"scenes/sidegunR.mesh\" />\
      <file name=\"scenes/gun5.mesh\" />\
      <file name=\"scenes/sidegunL.mesh\" />\
      <file name=\"scenes/gun4.mesh\" />\
      <file name=\"scenes/midgunR.mesh\" />\
      <file name=\"scenes/gun3.mesh\" />\
      <file name=\"scenes/midgunL.mesh\" />\
      <file name=\"scenes/reargate.mesh\" />\
      <file name=\"scenes/ingateR.mesh\" />\
      <file name=\"scenes/ingateL.mesh\" />\
      <file name=\"scenes/gun2.mesh\" />\
      <file name=\"scenes/reargunR.mesh\" />\
      <file name=\"scenes/gun1.mesh\" />\
      <file name=\"scenes/reargunL.mesh\" />\
      <file name=\"scenes/lift.mesh\" />\
      <file name=\"scenes/landgate.mesh\" />\
      <file name=\"scenes/skydome.mesh\" />\
      <file name=\"scenes/gun8.mesh\" />\
      <file name=\"scenes/frontgunR.mesh\" />\
      <file name=\"scenes/gun7.mesh\" />\
      <file name=\"scenes/frontgunL.mesh\" />\
      <file name=\"scenes/outgateR.mesh\" />\
      <file name=\"scenes/outgateL.mesh\" />\
      <file name=\"scenes/frontis.mesh\" />\
      <file name=\"scenes/roof.mesh\" />\
      <file name=\"scenes/station.mesh\" />\
      <file name=\"scenes/Cercle01.mesh\" />\
      <file name=\"materials/scripts/Deltastation1.material\" />\
      <file name=\"materials/textures/gunsdoors.bmp\" />\
      <file name=\"materials/textures/deltachrome.bmp\" />\
      <file name=\"materials/textures/deltacolor.bmp\" />\
      <file name=\"materials/textures/luminred.bmp\" />\
      <file name=\"materials/textures/deltaglass.bmp\" />\
      <file name=\"materials/textures/luminambar.bmp\" />\
      <file name=\"materials/textures/lumingreen.bmp\" />\
      <file name=\"materials/textures/luminred.bmp\" />\
      <file name=\"materials/textures/deltafloors.bmp\" />\
      <file name=\"materials/textures/ciel.tga\" />\
     </files>\
    </lod>\
   </content>\
  </object>\
 </evt>\
</solipsis>\
";
            node.mState++;
        }
        break;
    case 2:
#ifdef ULXR
        Sleep(11000);
        time(&now);
#endif
        if (now - node.mFirstEvtDate > 30)
        {
            xmlRespStr = "\
<solipsis>\
 <evt type=\"0\">\
  <object uid=\"22222222\" type=\"0\" name=\"z6-salamandra\">\
   <position x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
   <orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
   <content>\
    <lod level=\"0\">\
     <files>\
      <file name=\"z6-salamandra.xml\" />\
      <file name=\"models/salamandra.mesh\" />\
      <file name=\"models/salamandra.mesh.skeleton\" />\
      <file name=\"materials/scripts/salamandra.material\" />\
      <file name=\"materials/textures/LightMapSalamandra2.tga\" />\
     </files>\
    </lod>\
   </content>\
  </object>\
 </evt>\
</solipsis>\
";
            node.mState++;
        }
        break;
    default:
#ifdef ULXR
        while (!node.mAvatarDirty)
            Sleep(10);
#endif
        if (node.mAvatarDirty)
        {
            std::stringstream xmlRespStream;
            xmlRespStream << "\
<solipsis>\
 <evt type=\"2\">\
  <object uid=\"11111111\">\
   <position x=\"" << node.mAvatarPosition.x << "\" y=\"" << node.mAvatarPosition.y << "\" z=\"" << node.mAvatarPosition.z << "\" />\
   <orientation x=\"" << node.mAvatarOrientation.x << "\" y=\"" << node.mAvatarOrientation.y << "\" z=\"" << node.mAvatarOrientation.z << "\" w=\"" << node.mAvatarOrientation.w << "\" />\
  </object>\
 </evt>\
</solipsis>\
";
            xmlRespStr = xmlRespStream.str();
            node.mAvatarDirty = false;
        }

        break;
    }
    /////////////////////////////////////

    return (xmlRespStr.empty() ? INodeClient::RCNoEvt : INodeClient::RCOk);
}