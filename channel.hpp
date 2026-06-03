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

public:
	Channel();
	Channel(const std::string& name);
	~Channel();

	// 
	std::string			get_name() const;
	std::string			get_topic() const;
	void				set_topic(const std::string& topic);
	std::vector<int>	get_client_fds() const;
	std::vector<int>	get_operator_fds() const;

	// 
	void				add_client(int fd);
	void				remove_client(int fd);
	bool				is_member(int fd) const;

	// 
	void				add_operator(int fd);
	void				remove_operator(int fd);
	bool				is_operator(int fd) const;
};

#endif