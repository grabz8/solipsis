#include "NodeManager.h"
#include "Peer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
NodeManager::NodeManager()
{
}

//-------------------------------------------------------------------------------------
NodeManager::~NodeManager()
{
}

//-------------------------------------------------------------------------------------
AvatarNode* NodeManager::login(XmlLogin* xmlLogin)
{
    // Authentication
    EntityUID AvatarEntityId;
    NodeId nodeId;
    if (xmlLogin->getPwd().compare("demo") == 0)
    {
        char nidhex[16];
        sprintf(nidhex, "%08X", Peer::getSingleton().mConnectionsCount + 1);
        nodeId = nidhex;
        AvatarEntityId = 0x10000 + Peer::getSingleton().mConnectionsCount + 1;
    }
    else
        return 0;

    // Create the avatar node
    std::string xmlAvatarStr = "\
<entity uid=\"" + Ogre::StringConverter::toString(AvatarEntityId) + "\" owner=\"" + nodeId + "\" type=\"0\" name=\"" + xmlLogin->getUsername() + "\">\
 <flags bitmask=\"1\" />\
 <position x=\"17.0\" y=\"-57.0\" z=\"115.0\" />\
 <orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
 <aabb>\
  <min x=\"-0.82447118\" y=\"-0.013709042\" z=\"-0.64538133\" />\
  <max x=\"0.82353306\" y=\"1.4500649\" z=\"0.69156337\" />\
 </aabb>\
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
</entity>\
";
    TiXmlDocument xmlAvatarDoc;
    xmlAvatarDoc.Parse(xmlAvatarStr.c_str());
    XmlEntity* avatarXmlEntity = new XmlEntity();
    avatarXmlEntity->fromXmlElt(xmlAvatarDoc.RootElement());
    avatarXmlEntity->setDisplacement(Vector3::ZERO);
    AvatarNode* avatarNode = new AvatarNode(nodeId, avatarXmlEntity);
    mNodes[nodeId] = avatarNode;

    // Simulate the scene around the avatar
    if (nodeId.compare("00000001") == 0)
    {
        std::string xmlSiteStr = "\
<entity uid=\"11112222\" owner=\"00000001\" type=\"1\" name=\"Deltastation1\">\
 <position x=\"18.0\" y=\"-58.0\" z=\"133.0\" />\
 <orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
 <aabb>\
  <min x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
  <max x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
 </aabb>\
 <content>\
  <lod level=\"0\">\
   <files>"
"    <file name=\"Deltastation1.xml\" />\
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
    <file name=\"materials/textures/ciel.tga\" />"
"   </files>\
  </lod>\
 </content>\
</entity>\
";
        TiXmlDocument xmlSiteDoc;
        xmlSiteDoc.Parse(xmlSiteStr.c_str());
        XmlEntity* siteEntityDesc = new XmlEntity();
        siteEntityDesc->fromXmlElt(xmlSiteDoc.RootElement());
        NodeId siteNodeId = "00000010";
        SiteNode* siteNode = new SiteNode(siteNodeId, siteEntityDesc);
        mNodes[siteNodeId] = siteNode;
//        avatarNode->addAwareEntity(&siteNode->getEntity());
    }

    // Add other avatars to aware of
    for (NodeMap::iterator node = mNodes.begin(); node != mNodes.end(); ++node)
        if (node->first != avatarNode->getNodeId())
        {
            if (node->second->getType().compare("avatar") == 0)
            {
                AvatarNode* an = (AvatarNode*)node->second;
                avatarNode->addAwareEntity(&an->getEntity());
                an->addAwareEntity(&avatarNode->getEntity());
            }
            else if (node->second->getType().compare("site") == 0)
            {
                SiteNode* sn = (SiteNode*)node->second;
                avatarNode->addAwareEntity(&sn->getEntity());
            }
        }

    return avatarNode;
}

//-------------------------------------------------------------------------------------
bool NodeManager::logout(const NodeId& nodeId)
{
    AvatarNode* avatarNode = (AvatarNode*)mNodes[nodeId];
    delete avatarNode;
    mNodes.erase(nodeId);

    for (NodeMap::iterator node = mNodes.begin(); node != mNodes.end(); ++node)
        delete node->second;
    mNodes.clear();

    return true;
}

//-------------------------------------------------------------------------------------
bool NodeManager::processEvt(const NodeId& nodeId, XmlEvt& xmlEvt, std::string& xmlRespStr)
{
    Node* node = mNodes[nodeId];
    if (node == 0)
        return false;

    return node->processEvt(xmlEvt, xmlRespStr);
}

//-------------------------------------------------------------------------------------
XmlEvt* NodeManager::getNextEvtToHandle(const NodeId& nodeId)
{
    Node* node = mNodes[nodeId];
    if (node == 0)
        return 0;

    return node->getNextEvtToHandle();
}

//-------------------------------------------------------------------------------------
bool NodeManager::freeEvt(const NodeId& nodeId, XmlEvt* evt)
{
    Node* node = mNodes[nodeId];
    if (node == 0)
        return false;

    return node->freeEvt(evt);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
