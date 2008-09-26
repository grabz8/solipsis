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

#ifndef __Facebook_h__
#define __Facebook_h__

#include <iostream>
#include <list>
#include <string>
#include <sstream>

using namespace std;

typedef void CURL;
class md5wrapper;

namespace Solipsis {

/** Class managing authentication of 1 user on Facebook
    This Facebook class is a modification of James Custer & Joel Seligstein's.
    It uses the TinyXML, curl, and md5wrapper libraries.
 */
class Facebook
{
public:
    /// Constructor
    Facebook(string apiKey, string secret, string server);
    // Destructor
    ~Facebook();

    /** This function must be called first. It initializes curl and
      returns whether loadToken() has succeded or not.
    */
    bool authenticate();
    /** You should call this function after the user has logged in. */
    bool getSession();
    /** You can pass a Facebook method and its arguments to this function
      and get the result from res.
    */
    bool request(string method, list<string> args, string *res);

    /// Returns the URL the user must use to login
    string getLoginUrl(const string& url);
    /// Returns the API key
    const string& getApiKey() { return mApiKey; }
    /// Returns the secret
    const string& getSecret() { return mSecret; }
    /// Returns the token
    const string& getToken() { return mToken; }
    /// Returns the server URL
    const string& getServer() { return mServer; }
    /// Returns the session key
    const string& getSessionKey() { return mSessionKey; }
    /// Returns the session secret
    const string& getSessionSecret() { return mSessionSecret; }
    /// Returns the user UID
    const string& getUid() { return mUid; }
    /// Returns the version
    const string& getVersion() { return mVersion; }
    /// Returns true if the user has a session
    bool hasSession() { return mHasSession; }

private:
    string mApiKey;
    string mSecret;
    string mServer;
    string mToken;
    string mSessionKey;
    string mSessionSecret;
    string mUid;
    string mVersion;
    int mCallId;
    bool mHasSession;
    CURL *mCurl;
    md5wrapper *mMd5;

    /* Implements the facebook.auth.createToken method to create a token. */
    bool   loadToken();
    /* Given a list of arguments, this function returns a signature using the
     algorithm given in the authenticating guide.
    */
    string getSignature(list<string> args);
    /* Cleans up curl. */
    void   cleanup();
    /* Given a list of arguments, returns a string of the arguments to be used
     when creating a URL. */
    static string getArgsString(list<string> args, bool separate);
    /* Callback function for curl. */
    static size_t writeCallback(void *ptr, size_t size, size_t nmemb, void *userp);
};

} // namespace Solipsis

#endif // #ifndef __Facebook_h__