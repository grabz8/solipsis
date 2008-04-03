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
        int connection = Peer::getSingleton().allocConnection();
        if (connection == -1)
            return 0;
        char nidhex[16];
        sprintf(nidhex, "%08X", connection + 1);
        nodeId = nidhex;
        AvatarEntityId = 0x10000 + connection + 1;
    }
    else
        return 0;

    // Create the avatar node
    std::string xmlAvatarStr = "\
<entity uid=\"" + Ogre::StringConverter::toString(AvatarEntityId) + "\" owner=\"" + nodeId + "\" type=\"0\" name=\"" + xmlLogin->getUsername() + "\">\
 <flags bitmask=\"1\" />\
 <position x=\"17.0\" y=\"-50.0\" z=\"115.0\" />\
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
        std::string xmlSiteStr;
        if (Peer::getSingleton().mSceneDemoLoaded.compare("Ile") == 0)
            xmlSiteStr = "\
<entity uid=\"11112222\" owner=\"00000001\" type=\"1\" name=\"Ile\">\
 <position x=\"18.0\" y=\"-58.0\" z=\"133.0\" />\
 <orientation x=\"0.0\" y=\"0.0\" z=\"0.0\" w=\"1.0\" />\
 <aabb>\
  <min x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
  <max x=\"0.0\" y=\"0.0\" z=\"0.0\" />\
 </aabb>\
 <content>\
  <lod level=\"0\">\
   <files>"
"    <file name=\"Ile.xml\" />\
    <file name=\"scenes/Ile.osm\" />\
    <file name=\"scenes/Ile_bassin.mesh\" />\
    <file name=\"scenes/Ile_bateauPhare.mesh\" />\
    <file name=\"scenes/Ile_bateauPlage.mesh\" />\
    <file name=\"scenes/Ile_batecole01.mesh\" />\
    <file name=\"scenes/Ile_batecole02.mesh\" />\
    <file name=\"scenes/Ile_batecole03.mesh\" />\
    <file name=\"scenes/Ile_batecole04.mesh\" />\
    <file name=\"scenes/Ile_bathaut.mesh\" />\
    <file name=\"scenes/Ile_cabanebathaut.mesh\" />\
    <file name=\"scenes/Ile_cagetigre.mesh\" />\
    <file name=\"scenes/Ile_carton01.mesh\" />\
    <file name=\"scenes/Ile_chaise01grenier.mesh\" />\
    <file name=\"scenes/Ile_chaise02grenier.mesh\" />\
    <file name=\"scenes/Ile_charbon.mesh\" />\
    <file name=\"scenes/Ile_chofogrenier.mesh\" />\
    <file name=\"scenes/Ile_collinefond.mesh\" />\
    <file name=\"scenes/Ile_contourecage.mesh\" />\
    <file name=\"scenes/Ile_contourEcole.mesh\" />\
    <file name=\"scenes/Ile_distributeur.mesh\" />\
    <file name=\"scenes/Ile_echelle.mesh\" />\
    <file name=\"scenes/Ile_ElementsPlage01.mesh\" />\
    <file name=\"scenes/Ile_ElementsPlage02.mesh\" />\
    <file name=\"scenes/Ile_ElementsPlage03.mesh\" />\
    <file name=\"scenes/Ile_ElementsPlage04.mesh\" />\
    <file name=\"scenes/Ile_escalierrhino.mesh\" />\
    <file name=\"scenes/Ile_etageregrenier.mesh\" />\
    <file name=\"scenes/Ile_fleursbathaut.mesh\" />\
    <file name=\"scenes/Ile_glace.mesh\" />\
    <file name=\"scenes/Ile_glacierdeco.mesh\" />\
    <file name=\"scenes/Ile_grenier.mesh\" />\
    <file name=\"scenes/Ile_grilledome.mesh\" />\
    <file name=\"scenes/Ile_grilleloup.mesh\" />\
    <file name=\"scenes/Ile_Ile.mesh\" />\
    <file name=\"scenes/Ile_lancecaramel.mesh\" />\
    <file name=\"scenes/Ile_lancecaramel01.mesh\" />\
    <file name=\"scenes/Ile_Lianes.mesh\" />\
    <file name=\"scenes/Ile_marches01.mesh\" />\
    <file name=\"scenes/Ile_marches02.mesh\" />\
    <file name=\"scenes/Ile_mer.mesh\" />\
    <file name=\"scenes/Ile_mer01.mesh\" />\
    <file name=\"scenes/Ile_palmierIle01.mesh\" />\
    <file name=\"scenes/Ile_palmierIle02.mesh\" />\
    <file name=\"scenes/Ile_palmierIle03.mesh\" />\
    <file name=\"scenes/Ile_palmierIle04.mesh\" />\
    <file name=\"scenes/Ile_palmierIle05.mesh\" />\
    <file name=\"scenes/Ile_palmierIle06.mesh\" />\
    <file name=\"scenes/Ile_palmierIle07.mesh\" />\
    <file name=\"scenes/Ile_palmierIle08.mesh\" />\
    <file name=\"scenes/Ile_palmierIle09.mesh\" />\
    <file name=\"scenes/Ile_palmierPlage01.mesh\" />\
    <file name=\"scenes/Ile_palmierPlage02.mesh\" />\
    <file name=\"scenes/Ile_palmierPlage03.mesh\" />\
    <file name=\"scenes/Ile_palmiersbathaut.mesh\" />\
    <file name=\"scenes/Ile_palmiersecole.mesh\" />\
    <file name=\"scenes/Ile_palmiers_zoo.mesh\" />\
    <file name=\"scenes/Ile_palmiertoit.mesh\" />\
    <file name=\"scenes/Ile_panneaurhino.mesh\" />\
    <file name=\"scenes/Ile_pavillon.mesh\" />\
    <file name=\"scenes/Ile_Phare.mesh\" />\
    <file name=\"scenes/Ile_Plage.mesh\" />\
    <file name=\"scenes/Ile_pontonPhare01.mesh\" />\
    <file name=\"scenes/Ile_pontonPhare02.mesh\" />\
    <file name=\"scenes/Ile_Portezop.mesh\" />\
    <file name=\"scenes/Ile_poubelle01.mesh\" />\
    <file name=\"scenes/Ile_pourtourzoo.mesh\" />\
    <file name=\"scenes/Ile_rochers03.mesh\" />\
    <file name=\"scenes/Ile_rochersmer01.mesh\" />\
    <file name=\"scenes/Ile_rochersmer02.mesh\" />\
    <file name=\"scenes/Ile_rochersmer04.mesh\" />\
    <file name=\"scenes/Ile_rochersmer05.mesh\" />\
    <file name=\"scenes/Ile_rochersmer06.mesh\" />\
    <file name=\"scenes/Ile_rochersmer07.mesh\" />\
    <file name=\"scenes/Ile_rochersmer08.mesh\" />\
    <file name=\"scenes/Ile_rochersmer09.mesh\" />\
    <file name=\"scenes/Ile_rochersPhare.mesh\" />\
    <file name=\"scenes/Ile_RochersPLage01.mesh\" />\
    <file name=\"scenes/Ile_rochersplaine01.mesh\" />\
    <file name=\"scenes/Ile_rochersplaine02.mesh\" />\
    <file name=\"scenes/Ile_rochersplaine03.mesh\" />\
    <file name=\"scenes/Ile_rochersplaine04.mesh\" />\
    <file name=\"scenes/Ile_rochersplaine05.mesh\" />\
    <file name=\"scenes/Ile_rochersplaine06.mesh\" />\
    <file name=\"scenes/Ile_rochersplaine07.mesh\" />\
    <file name=\"scenes/Ile_Sol.mesh\" />\
    <file name=\"scenes/Ile_solbatecole01.mesh\" />\
    <file name=\"scenes/Ile_telescopgrenier.mesh\" />\
    <file name=\"scenes/Ile_terrain.mesh\" />\
    <file name=\"scenes/Ile_Tuyauxeau.mesh\" />\
    <file name=\"scenes/Ile_verriere.mesh\" />\
    <file name=\"scenes/Ile_wc.mesh\" />\
    <file name=\"materials/scripts/Ile.material\" />\
    <file name=\"materials/textures/mapsol.jpg\" />"
"    <file name=\"materials/textures/OI_ail_02_lancecaramel.jpg\" />\
    <file name=\"materials/textures/OI_ail_02_panneaurhino.jpg\" />\
    <file name=\"materials/textures/OI_ail_02_telescopgrenier.jpg\" />\
    <file name=\"materials/textures/OI_ail_03_merTextureComplète.jpg\" />\
    <file name=\"materials/textures/O_ail_02_bassin.jpg\" />\
    <file name=\"materials/textures/O_ail_02_bathaut.jpg\" />\
    <file name=\"materials/textures/O_ail_02_cabanebathaut.jpg\" />\
    <file name=\"materials/textures/O_ail_02_cagetigre.jpg\" />\
    <file name=\"materials/textures/O_ail_02_carton01.jpg\" />\
    <file name=\"materials/textures/O_ail_02_chaise01grenier.jpg\" />\
    <file name=\"materials/textures/O_ail_02_chaise02grenier.jpg\" />\
    <file name=\"materials/textures/O_ail_02_charbon.jpg\" />\
    <file name=\"materials/textures/O_ail_02_chofogrenier.jpg\" />\
    <file name=\"materials/textures/O_ail_02_contourecage.jpg\" />\
    <file name=\"materials/textures/O_ail_02_distributeur.jpg\" />\
    <file name=\"materials/textures/O_ail_02_echelle.jpg\" />\
    <file name=\"materials/textures/O_ail_02_escalierrhino.jpg\" />\
    <file name=\"materials/textures/O_ail_02_etageregrenier.jpg\" />\
    <file name=\"materials/textures/O_ail_02_fleursbathaut.jpg\" />\
    <file name=\"materials/textures/O_ail_02_glaceTextureComplète.jpg\" />\
    <file name=\"materials/textures/O_ail_02_glacierdeco.jpg\" />\
    <file name=\"materials/textures/O_ail_02_grenier.jpg\" />\
    <file name=\"materials/textures/O_ail_02_grilledome.jpg\" />\
    <file name=\"materials/textures/O_ail_02_grilleloup.jpg\" />\
    <file name=\"materials/textures/O_ail_02_Lianes.jpg\" />\
    <file name=\"materials/textures/O_ail_02_palmiersbathaut.jpg\" />\
    <file name=\"materials/textures/O_ail_02_palmiers_zoo.jpg\" />\
    <file name=\"materials/textures/O_ail_02_palmiertoit.jpg\" />\
    <file name=\"materials/textures/O_ail_02_pavillon.jpg\" />\
    <file name=\"materials/textures/O_ail_02_Portezop.jpg\" />\
    <file name=\"materials/textures/O_ail_02_poubelle01.jpg\" />\
    <file name=\"materials/textures/O_ail_02_pourtourzoo.jpg\" />\
    <file name=\"materials/textures/O_ail_02_terrain.jpg\" />\
    <file name=\"materials/textures/O_ail_02_Tuyauxeau.jpg\" />\
    <file name=\"materials/textures/O_ail_02_verriere.jpg\" />\
    <file name=\"materials/textures/O_ail_02_wc.jpg\" />\
    <file name=\"materials/textures/O_ail_03_bateauPlage.jpg\" />\
    <file name=\"materials/textures/O_ail_03_collinefond.jpg\" />\
    <file name=\"materials/textures/O_ail_03_ElementsPlage01.jpg\" />\
    <file name=\"materials/textures/O_ail_03_ElementsPlage02.jpg\" />\
    <file name=\"materials/textures/O_ail_03_ElementsPlage03.jpg\" />\
    <file name=\"materials/textures/O_ail_03_ElementsPlage04.jpg\" />\
    <file name=\"materials/textures/O_ail_03_Ile.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierIle01.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierIle02.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierIle03.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierIle04.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierIle05.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierIle06.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierIle07.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierIle08.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierIle09.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierPlage01.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierPlage02.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierPlage03.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmierPlage03TextureComplete.jpg\" />\
    <file name=\"materials/textures/O_ail_03_palmiersecole.jpg\" />\
    <file name=\"materials/textures/O_ail_03_Plage.jpg\" />\
    <file name=\"materials/textures/O_ail_03_RochersPLage.jpg\" />\
    <file name=\"materials/textures/O_ail_03_rochersplaine01.jpg\" />\
    <file name=\"materials/textures/O_ail_03_rochersplaine02.jpg\" />\
    <file name=\"materials/textures/O_ail_03_rochersplaine03.jpg\" />\
    <file name=\"materials/textures/O_ail_03_rochersplaine04.jpg\" />\
    <file name=\"materials/textures/O_ail_03_rochersplaine05.jpg\" />\
    <file name=\"materials/textures/O_ail_03_rochersplaine06.jpg\" />\
    <file name=\"materials/textures/O_ail_03_rochersplaine07.jpg\" />\
    <file name=\"materials/textures/o_eco_batecole01.jpg\" />\
    <file name=\"materials/textures/o_eco_batecole02.jpg\" />\
    <file name=\"materials/textures/o_eco_batecole03_bis.jpg\" />\
    <file name=\"materials/textures/o_eco_batecole04.jpg\" />\
    <file name=\"materials/textures/o_eco_contourEcole.jpg\" />\
    <file name=\"materials/textures/o_eco_marches01.jpg\" />\
    <file name=\"materials/textures/o_eco_marches02.jpg\" />\
    <file name=\"materials/textures/o_eco_solbatecole01.jpg\" />\
    <file name=\"materials/textures/O_sou_04_bateauPhare.jpg\" />\
    <file name=\"materials/textures/O_sou_04_Phare.jpg\" />\
    <file name=\"materials/textures/O_sou_04_pontonPhare01.jpg\" />\
    <file name=\"materials/textures/O_sou_04_pontonPhare02.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersmer01.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersmer02.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersmer03.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersmer04.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersmer05.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersmer06.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersmer07.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersmer08.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersmer09.jpg\" />\
    <file name=\"materials/textures/O_sou_04_rochersPhare.jpg\" />"
"   </files>\
  </lod>\
 </content>\
</entity>\
";
        else
            xmlSiteStr = "\
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
    avatarNode->freeze(true);
    mDestroyedNodeIds.insert(nodeId);

    int connection;
    sscanf(nodeId.c_str(), "%x", &connection);
    if (connection < 1)
        return false;
    if (!Peer::getSingleton().releaseConnection(connection - 1))
        return false;

    return true;
}

//-------------------------------------------------------------------------------------
bool NodeManager::update()
{
    NodeIdSet::iterator i;
    for (i = mDestroyedNodeIds.begin(); i != mDestroyedNodeIds.end(); i++)
    {
        AvatarNode* avatarNode = (AvatarNode*)mNodes[*i];
        // Remove this avatar from avatars aware of
        for (NodeMap::iterator node = mNodes.begin(); node != mNodes.end(); ++node)
            if (node->first != avatarNode->getNodeId())
            {
                if (node->second->getType().compare("avatar") == 0)
                {
                    AvatarNode* an = (AvatarNode*)node->second;
                    avatarNode->removeAwareEntity(&an->getEntity());
                    an->removeAwareEntity(&avatarNode->getEntity());
                }
                else if (node->second->getType().compare("site") == 0)
                {
                    SiteNode* sn = (SiteNode*)node->second;
                    avatarNode->removeAwareEntity(&sn->getEntity());
                }
            }
        mNodes.erase(*i);
        delete avatarNode;
        /////////////////////////////////////
        if (Peer::getSingleton().getConnectionsCount() == 0)
        {
            NodeId siteNodeId = "00000010";
            delete mNodes[siteNodeId];
            mNodes.erase(siteNodeId);
            mNodes.clear();
        }
        /////////////////////////////////////
    }
    mDestroyedNodeIds.clear();

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
#ifdef POOL
RefCntPoolPtr<XmlEvt> NodeManager::getNextEvtToHandle(const NodeId& nodeId)
#else
XmlEvt* NodeManager::getNextEvtToHandle(const NodeId& nodeId)
#endif
{
    Node* node = mNodes[nodeId];
    if (node == 0)
#ifdef POOL
        return RefCntPoolPtr<XmlEvt>::nullPtr;
#else
        return 0;
#endif

    return node->getNextEvtToHandle();
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool NodeManager::freeEvt(const NodeId& nodeId, RefCntPoolPtr<XmlEvt>& evt)
#else
bool NodeManager::freeEvt(const NodeId& nodeId, XmlEvt* evt)
#endif
{
    Node* node = mNodes[nodeId];
    if (node == 0)
        return false;

    return node->freeEvt(evt);
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
