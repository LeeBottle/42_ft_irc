#ifndef COMMANDHELPER_HPP
# define COMMANDHELPER_HPP

# include <string>

class Channel;
class Client;

class CommandHelper
{
public:
    static bool                 validChannel(const std::string &);
    static void                 reply(Client &, const std::string &);
    static void                 namesReply(Client &, Channel &);
    static void                 topicReply(Client &, Channel &);
    static void                 toAll(Channel &,
                                    const std::string &);
    static void                 toOthers(Channel &, Client &,
                                    const std::string &);
    static const std::string    &target(Client &);
    static std::string          memberNames(Channel &);

private:
    CommandHelper();
    CommandHelper(const CommandHelper &);
    CommandHelper &operator=(const CommandHelper &);
    ~CommandHelper();
};

#endif
