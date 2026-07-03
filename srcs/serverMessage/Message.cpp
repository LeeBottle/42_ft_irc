#include "serverMessage/Message.hpp"
#include "client/Client.hpp"
#include "parser/Parser.hpp"

#include <iostream>

Message::Message(const std::string &password,
    ClientManager &clients, ChannelManager &channels)
    : _command(password, clients, channels)
{
}

Message::~Message()
{
}

bool    Message::handle(Client &client,
    const std::string &line)
{
    Parser  message;

    if (!message.parse(line))
        return (true);
    return (_command.execute(client, message));
}

bool    Message::process(Client &client)
{
    std::string line;

    while (client.receiveBuffer().pop(line))
    {
        std::cout << "received from fd " << client.fd()
            << ": " << line << std::endl;
        if (!handle(client, line))
            return (false);
    }
    return (true);
}
