#include "SimpleVoiceEngineProtocol.h"
#include <assert.h>
#include "Socket.h"
#include "VoiceHeader.h"

int SimpleVoiceEngineProtocol::receiveVoiceUid(Socket & socket, Solipsis::EntityUID & voiceUid)
{
	voiceUid.clear();

	const unsigned int maxVoiceIdSize = 128;
	char voiceIdBuffer[maxVoiceIdSize];

    int numBytesReceived = 0;
	// receive the size of the voice id
	unsigned int idSize; 
	{
		int received = 0;
		while (received < sizeof(unsigned int))
			received += socket.receive(&((char*)&idSize)[received], sizeof(unsigned int)-received);
		numBytesReceived += received;
	}
	assert(idSize < maxVoiceIdSize);

	// receive the voice id
	{
		unsigned int received = 0;
		while (received < idSize)
			received += socket.receive(&((char*)voiceIdBuffer)[received], idSize-received);
		assert(received == idSize);
		numBytesReceived += received;
	}

	voiceIdBuffer[idSize] = 0;
	voiceUid = voiceIdBuffer;

    return numBytesReceived;
}

int SimpleVoiceEngineProtocol::receiveLogin(Socket & socket, Solipsis::EntityUID& voiceId, int* supportedFormats)
{
    int received = 0;
	received = receiveVoiceUid(socket, voiceId);
    received += receiveSupportedFormats(socket, supportedFormats);

    return received;
}

int SimpleVoiceEngineProtocol::receiveSupportedFormats(Socket & socket, int* supportedFormats)
{
    int received = 0;
    while (received < sizeof(int))
		received += socket.receive(&((char*)supportedFormats)[received], sizeof(int)-received);

    return received;
}

int SimpleVoiceEngineProtocol::sendPacketHeader(Socket & socket, VoicePacketType type, unsigned int size)
{
    char packet[sizeof(char)+sizeof(unsigned int)];
	char voicePacketTypeAsChar = type;
    memcpy(packet, &voicePacketTypeAsChar, sizeof(char));
    memcpy(&packet[sizeof(char)], &size, sizeof(unsigned int));
    return socket.send( packet, sizeof(packet) );
}

int SimpleVoiceEngineProtocol::receivePacketHeader(Socket & socket, VoicePacketType* type, unsigned int* size)
{
    int numBytesReceived = 0;
	char typeAsChar;
	numBytesReceived = socket.receive(&typeAsChar, sizeof(char));
	*type = (VoicePacketType)typeAsChar;
    if (numBytesReceived <= 0) return numBytesReceived;
    numBytesReceived = 0;
    while (numBytesReceived < sizeof(unsigned int))
        numBytesReceived += socket.receive( &((char*)size)[numBytesReceived], sizeof(unsigned int)-numBytesReceived);
    return numBytesReceived + sizeof(char);
}
