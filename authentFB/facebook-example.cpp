/**
 * Facebook Class Example
 * from James Custer
 * Last Modified: June 1, 2007
 *
 * This is an example of using the Facebook class.
 */

#include <iostream>
#include "tinyxml.h"
#include "curl/curl.h"
#include "curl/easy.h"
#include "facebook.h"

int main()
{
    const string apiKey  = "8d81e4c64ac0039b209c4a53b21ba220";
    const string secret  = "695a02e3645bed085e1802c7e9952d73";
    const string server  = "api.facebook.com/restserver.php";
    const string login   = "http://api.facebook.com/login.php";

    Facebook fb(apiKey, secret, server);

    // Grab a token from the server.
    if (!fb.authenticate())
    {
        cerr << "Unable to authenticate." << endl;
        exit(1);
    }

    // Let the user login using the token we just received. The user must login
    // before we can continue, so our program will be put on hold by waiting
    // for input. If you're using a GUI application, one might put up a MessageBox.
    cout << "Use the following URL to login: " << fb.getLoginUrl(login) << endl;
    cout << "After logging in, type something and hit enter." << endl;
    int x;
    ShellExecute(0, "open", fb.getLoginUrl(login).c_str(), 0, 0, SW_SHOWNORMAL);
    cin >> x;

    // After the user has logged in, we get the session.
    if (!fb.getSession())
    {
        cerr << "Unable to get session." << endl;
        exit(1);
    }

    cout << "Uid: " << fb.getUid() << endl;
    cout << "SessionKey: " << fb.getSessionKey() << endl;
    cout << "SessionSecret: " << fb.getSessionSecret() << endl;

    // Now we retrieve a list of the user's friends and the result is stored
    // in xml.
    list<string> args;
    string xml;
    if (!fb.request("facebook.friends.get", args, &xml))
    {
        cerr << "Could not get friends list." << endl;
        exit(1);
    }

    // We parse the XML document to get the user's friends' ids.
    TiXmlDocument doc;
    doc.Parse(xml.c_str());
    TiXmlHandle docHandle(&doc);
    TiXmlNode *response = doc.FirstChild("friends_get_response");
    if (!response)
    {
        cerr << "Error getting friends list." << endl;
        exit(1);
    }
    list<string> friendsList;
    TiXmlNode *friends = 0;
    while (friends = response->IterateChildren("uid",friends))
    {
        friendsList.push_back(friends->ToElement()->GetText());
    }

    // Now we take that list of uids and request their name and status.
    args.clear();
    xml.clear();
    string uids;
    list<string>::iterator i;
    for (i = friendsList.begin(); i != friendsList.end(); i++)
    {
        uids.append(*i);
        uids.append(",");
    }
    // Erase the last appended comma.
    uids.erase(uids.size()-1,1);
    args.push_back("uids=" + uids);
    // We're going to obtain the name and status of the friends in uids.
    args.push_back("fields=name,status");
    if (!fb.request("facebook.users.getInfo",args, &xml))
    {
        cerr << "Could not get users info." << endl;
        exit(1);
    }

    doc.Parse(xml.c_str());
    doc.Print();
    response = doc.FirstChild("users_getInfo_response");
    if (!response)
    {
        cerr << "Error getting friends' names." << endl;
        exit(1);
    }

    // Print out each user's name and status.
    TiXmlNode *user = docHandle.FirstChild("users_getInfo_response").FirstChild("user").ToNode();
    TiXmlNode *message;
    for (user; user; user = user->NextSibling())
    {
        cout << user->FirstChild("name")->ToElement()->GetText();
        if (message = user->FirstChild("status")->FirstChild("message"))
            if (message->FirstChild())
                cout << " -> " << message->ToElement()->GetText();
        cout << endl;
    }

    return 0;
}
