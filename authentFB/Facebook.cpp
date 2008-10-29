/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory, original code by James Custer & Joel Seligstein's

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "facebook.h"
#include <curl/curl.h>
#include <md5wrapper/md5wrapper.h>
#include <tinyxml.h>

namespace Solipsis {

//-------------------------------------------------------------------------------------
Facebook::Facebook(string apiKey, string secret, string server) :
    mApiKey(apiKey),
    mSecret(secret),
    mServer(server),
    mToken(""),
    mVersion("1.0"),
    mHasSession(false),
    mCallId(0),
    mCurl(0)
{
    mMd5 = new md5wrapper();
}

//-------------------------------------------------------------------------------------
Facebook::~Facebook()
{
    cleanup();
    if (mMd5 != 0)
        delete mMd5;
}

//-------------------------------------------------------------------------------------
bool Facebook::authenticate()
{
    mCurl = curl_easy_init();
    if (!mCurl)
    {
        cerr << "Could not initialize curl." << endl;
        return false;
    }
    return loadToken();
}

//-------------------------------------------------------------------------------------
string Facebook::getLoginUrl(const string& url)
{
    return string(url + "?api_key=" + mApiKey + "&v=" + mVersion + "&auth_token=" + mToken);
}

//-------------------------------------------------------------------------------------
bool Facebook::getSession()
{
    list<string> args;
    string xml;

    args.push_back("auth_token=" + mToken);
    if (!request("facebook.auth.getSession", args, &xml))
        return false;
    if (xml.length() == 0)
        return false;

    TiXmlDocument doc;
    doc.Parse(xml.c_str());
    TiXmlHandle docHandle(&doc);
    TiXmlHandle response        = docHandle.FirstChild("auth_getSession_response");
    TiXmlElement* sessionKey    = response.FirstChild("session_key").ToElement();
    TiXmlElement* uid           = response.FirstChild("uid").ToElement();
    TiXmlElement* sessionSecret = response.FirstChild("secret").ToElement();
    if (!sessionKey || !sessionSecret || !uid)
        return false;
    mSessionKey    = sessionKey->GetText();
    mSessionSecret = sessionSecret->GetText();
    mUid           = uid->GetText();
    mHasSession    = true;

    return true;
}

//-------------------------------------------------------------------------------------
bool Facebook::request(string method, list<string> args, string *result)
{
    stringstream cid;
    cid << mCallId;

    args.push_back("api_key=" + mApiKey);
    args.push_back("call_id=" + cid.str());
    args.push_back("method="  + method);
    if (mHasSession)
        args.push_back("session_key=" + mSessionKey);
    args.push_back("v=" + mVersion);
    args.push_back("sig=" + getSignature(args));

    string query = Facebook::getArgsString(args, true);

    curl_easy_reset(mCurl);
    curl_easy_setopt(mCurl, CURLOPT_URL, mServer.c_str());
    curl_easy_setopt(mCurl, CURLOPT_POST, true);
    curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, query.c_str());
    curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, Facebook::writeCallback);
    curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, result);

    CURLcode res = curl_easy_perform(mCurl);

    mCallId++;

    return (res == 0);
}

//-------------------------------------------------------------------------------------
bool Facebook::loadToken()
{
    list<string> args;
    string xml;

    if (!request("facebook.auth.createToken", args, &xml))
        return false;
    if (xml.length() == 0)
        return false;

    TiXmlDocument doc;
    doc.Parse(xml.c_str());
    TiXmlHandle docHandle(&doc);
    TiXmlElement *token = docHandle.FirstChild("auth_createToken_response").ToElement();
    if (!token)
        return false;
    mToken = token->GetText();

    return true;
}

//-------------------------------------------------------------------------------------
string Facebook::getSignature(list<string> args)
{
    string ret = getArgsString(args, false);
    if (mHasSession)
        ret.append(mSessionSecret);
    else
        ret.append(mSecret);

    return mMd5->getHashFromString(ret);
}

//-------------------------------------------------------------------------------------
void Facebook::cleanup()
{
    if (mCurl == 0) return;
    curl_easy_cleanup(mCurl);
    mCurl = 0;
}

//-------------------------------------------------------------------------------------
string Facebook::getArgsString(list<string> args, bool separate)
{
    list<string>::iterator i;
    string ret;

    args.sort();
    for (i = args.begin(); i != args.end(); i++)
    {
        ret.append(*i);
        if (separate)
            ret.append("&");
    }
    if (separate)
        ret.erase(ret.size() - 1, 1);

    return ret;
}

//-------------------------------------------------------------------------------------
size_t Facebook::writeCallback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    string *str = (string *) userp;
    str->append((char*)ptr, size*nmemb);
    return size*nmemb;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
