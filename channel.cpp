#include "channel.hpp"
#include <algorithm>

Channel::Channel() 
  : _name(""), _topic(""), _mode_invite_only(false), _mode_topic_op_only(false), _key(""), _max_users(-1)
{
}

Channel::Channel(const std::string& name) 
  : _name(name), _topic(""), _mode_invite_only(false), _mode_topic_op_only(false), _key(""), _max_users(-1)
{
}

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

// i 모드 Getter / Setter
bool Channel::is_invite_only() const { return _mode_invite_only; }
void Channel::set_invite_only(bool on) { _mode_invite_only = on; }

// t 모드 Getter / Setter
bool Channel::is_topic_op_only() const { return _mode_topic_op_only; }
void Channel::set_topic_op_only(bool on) { _mode_topic_op_only = on; }

// k 모드 Getter / Setter
std::string Channel::get_key() const { return _key; }
void Channel::set_key(const std::string& key) { _key = key; }

// l 모드 Getter / Setter
long long Channel::get_max_users() const { return _max_users; }
void Channel::set_max_users(long long limit) { _max_users = limit; }


// 초대(Invite) 유저 관리 구현
void Channel::add_invite(int fd)
{
    if (!is_invited(fd))
        _invited_fds.push_back(fd);
}

void Channel::remove_invite(int fd)
{
    std::vector<int>::iterator it = std::find(_invited_fds.begin(), _invited_fds.end(), fd);
    if (it != _invited_fds.end())
        _invited_fds.erase(it);
}

bool Channel::is_invited(int fd) const
{
    std::vector<int>::const_iterator it = std::find(_invited_fds.begin(), _invited_fds.end(), fd);
    return (it != _invited_fds.end());
}
