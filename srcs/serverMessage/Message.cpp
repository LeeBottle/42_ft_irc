#include "serverMessage/Message.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"


#ifdef DEBUG_RECV
# include <iostream>
#endif


// Initializes this object with the supplied state.
Message::Message(const std::string &password, ClientManager &clients,
    ChannelManager &channels)
    : _command(password, clients, channels)
{
}


// Destroys this object and releases its owned resources.
Message::~Message()
{
}


// Validates and executes this IRC command.
bool    Message::handle(Client &client, const std::string &line)
{
    Parser  message;

    if (!message.parse(line))
        return (true);

    return (_command.execute(client, message));
}


// Processes complete IRC messages currently buffered for a client.
bool    Message::process(Client &client)
{
    std::string line;

    while (client.receiveBuffer().pop(line))
    {

#ifdef DEBUG_RECV
        std::cout << "received from fd " << client.fd()
            << ": " << line << std::endl;
#endif

        if (!handle(client, line))
            return (false);
    }

    return (true);
}
