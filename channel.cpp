#include "channel.hpp"
#include <algorithm>

Channel::Channel() : _name(""), _topic("") {}

Channel::Channel(const std::string& name) : _name(name), _topic("") {}

Channel::~Channel() {}

std::string Channel::get_name() const
{
	return _name;
}

std::string Channel::get_topic() const
{
	return _topic;
}

void Channel::set_topic(const std::string& topic)
{
	_topic = topic;
}

std::vector<int> Channel::get_client_fds() const
{
	return _client_fds;
}

std::vector<int> Channel::get_operator_fds() const
{
	return _operator_fds;
}

// 채널 유저 입장
void Channel::add_client(int fd)
{
	if (is_member(fd) == false)
	{
		_client_fds.push_back(fd);
	}
}

// 채널 유저 퇴장
void Channel::remove_client(int fd)
{
	std::vector<int>::iterator it;
	for (it = _client_fds.begin(); it != _client_fds.end(); ++it)
	{
		if (*it == fd)
		{
			_client_fds.erase(it);
			break;
		}
	}
	// 퇴장 시 오퍼레이터 권한도 함께 가지고 있다면 해제합니다.
	remove_operator(fd);
}

// 채널 유저 존재 여부 검사
bool Channel::is_member(int fd) const
{
	for (size_t i = 0; i < _client_fds.size(); ++i)
	{
		if (_client_fds[i] == fd)
		{
			return true;
		}
	}
	return false;
}

// 채널 오퍼레이터 권한 부여
void Channel::add_operator(int fd)
{
	if (is_operator(fd) == false)
	{
		_operator_fds.push_back(fd);
	}
}

// 채널 오퍼레이터 권한 박탈
void Channel::remove_operator(int fd)
{
	std::vector<int>::iterator it;
	for (it = _operator_fds.begin(); it != _operator_fds.end(); ++it)
	{
		if (*it == fd)
		{
			_operator_fds.erase(it);
			break;
		}
	}
}

// 채널 오퍼레이터 여부 검사
bool Channel::is_operator(int fd) const
{
	for (size_t i = 0; i < _operator_fds.size(); ++i)
	{
		if (_operator_fds[i] == fd)
		{
			return true;
		}
	}
	return false;
}