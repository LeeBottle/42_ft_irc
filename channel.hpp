#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

class Channel
{
private:
	std::string			_name;
	std::string			_topic;
	std::vector<int>	_client_fds;
	std::vector<int>	_operator_fds;

	//mode value
	bool				_mode_invite_only;	//초대모드 
	bool				_mode_topic_op_only;//토픽변경권한
	std::string			_key;				//비밀번호
	long long			_max_users;			//유저수 제한
	std::vector<int>	_invited_fds;		//초대받은 유저 fd

public:
	Channel();
	Channel(const std::string& name);
	~Channel();

	//게터세터
	std::string			get_name() const;
	std::string			get_topic() const;
	void				set_topic(const std::string& topic);
	std::vector<int>	get_client_fds() const;
	std::vector<int>	get_operator_fds() const;

	//i
	bool                is_invite_only() const;
    void                set_invite_only(bool on);
	//t
	bool                is_topic_op_only() const;
    void                set_topic_op_only(bool on);
	//k
	std::string         get_key() const;
    void                set_key(const std::string& key);
	//l
	long long           get_max_users() const;
    void                set_max_users(long long limit);
	//초대 목록(join 체크용)
	void                add_invite(int fd);
    void                remove_invite(int fd);
    bool                is_invited(int fd) const;

	//default
	void				add_client(int fd);
	void				remove_client(int fd);
	bool				is_member(int fd) const;

	void				add_operator(int fd);
	void				remove_operator(int fd);
	bool				is_operator(int fd) const;
};

#endif
