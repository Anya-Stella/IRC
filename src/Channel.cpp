#include "../include/Channel.hpp"


//TODO:temp()
void Channel::addInvite(int fd)
{
    _invited.insert(fd);
}

bool Channel::isInvited(int fd) const
{
    return _invited.count(fd) > 0;
}
