#include "bircd.hpp"

void env::handle_commands(int cs, const std::string& cmd_line)
{
	if (cmd_line.find("CAP") == 0)
	{
		if (fds[cs].type != FD_CLIENT)
		{
			if (fds[cs].buf_read.find('\n') != std::string::npos)
			{
				if (fds[cs].buf_read.find("PASS") == std::string::npos)
				{
					std::cout << "Client #" << cs << " irssi connect failed: Missing PASS command." << std::endl;
					fds[cs].buf_write += ":ircserv 461 * PASS :Not enough parameters\r\n";
					fds[cs].close_after_write = true;
					return;
				}
			}
		}
		return;
	}

	if (fds[cs].type != FD_CLIENT)
	{
		if (cmd_line.find("PASS") == 0)
		{
			handle_command_pass(cs, cmd_line);
			return;
		}
		else
		{
			std::cout << "Security Alert: Unauthenticated Client #" << cs << " bypassed PASS. Command ignored: " << cmd_line << std::endl;
			fds[cs].buf_write += ":ircserv 451 * :You have not registered\r\n";
			fds[cs].close_after_write = true;
			return;
		}
	}

	if (cmd_line.find("PASS") == 0)
	{
		std::cout << "Client #" << cs << " tried to send PASS again after authorization." << std::endl;
		fds[cs].buf_write += ":ircserv 462 * :Unauthorized command (Already registered)\r\n";
		return;
	}
	else if (cmd_line.find("NICK") == 0)
	{
		handle_command_nick(cs, cmd_line);
	}
	else if (cmd_line.find("USER") == 0)
	{
		handle_command_user(cs, cmd_line);
	}
	else if (cmd_line.find("PRIVMSG") == 0)
	{
		handle_command_privmsg(cs, cmd_line);
	}
	else if (cmd_line.find("JOIN") == 0)
	{
		handle_command_join(cs, cmd_line);
	}
	else if (cmd_line.find("PART") == 0)
	{
		handle_command_part(cs, cmd_line);
	}
	else if (cmd_line.find("KICK") == 0)
	{
		handle_command_kick(cs, cmd_line);
	}
	else if (cmd_line.find("INVITE") == 0)
	{
		handle_command_invite(cs, cmd_line);
	}
	else if (cmd_line.find("TOPIC") == 0)
	{
		handle_command_topic(cs, cmd_line);
	}
}
/**
 * @brief PASS 명령어를 처리하여 클라이언트의 비밀번호 인증을 수행합니다.
 * @param cs 클라이언트 소켓의 파일 디스크립터(FD) 번호
 * @param cmd_line 클라이언트가 전송한 명령어 전체 문자열
 */
void env::handle_command_pass(int cs, const std::string& cmd_line)
{
	// 1. 이미 인증 및 등록(NICK/USER 완료)된 클라이언트가 PASS를 다시 보낸 경우 예외 처리
	if (fds[cs].type == FD_CLIENT && !fds[cs].nickname.empty())
	{
		std::cout << "Client #" << cs << " tried to send PASS again. Sending 462 Already registered." << std::endl;
		fds[cs].buf_write += ":ircserv 462 " + fds[cs].nickname + " :You may not reregister\r\n";
		return;
	}

	// 2. "PASS" 명령어 이후의 공백 문자 스킵 (인자 시작 위치 찾기)
	size_t idx = 4;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 3. 비밀번호 인자가 누락된 경우 (461 ERR_NEEDMOREPARAMS)
	if (idx >= cmd_line.length())
	{
		std::cout << "Client #" << cs << " auth failed: Not enough parameters." << std::endl;
		fds[cs].buf_write += ":ircserv 461 * PASS :Not enough parameters\r\n";
		fds[cs].close_after_write = true;
		return;
	}

	// 4. 입력된 비밀번호 문자열 추출 및 후행 공백 제거
	std::string input_pw = cmd_line.substr(idx);
	while (!input_pw.empty() && std::isspace(static_cast<unsigned char>(input_pw[input_pw.size() - 1])))
	{
		input_pw.erase(input_pw.size() - 1);
	}

	// 공백 제거 후 비밀번호가 비어있는 경우 처리
	if (input_pw.empty())
	{
		fds[cs].buf_write += ":ircserv 461 * PASS :Not enough parameters\r\n";
		fds[cs].close_after_write = true;
		return;
	}

	// 5. 서버 비밀번호와 일치하는지 검증
	if (input_pw == password)
	{
		// 인증 성공: 파일 디스크립터 타입을 일반 클라이언트로 승격
		fds[cs].type = FD_CLIENT;
		std::cout << "Client #" << cs << " password successfully authorized." << std::endl;
	}
	else
	{
		// 인증 실패: 464 ERR_PASSWDMISMATCH 전송 후 연결 종료 설정
		std::cout << "Client #" << cs << " auth failed: Incorrect password." << std::endl;
		fds[cs].buf_write += ":ircserv 464 * :Password incorrect\r\n";
		fds[cs].close_after_write = true;
	}
}

/**
 * @brief NICK 명령어를 처리하여 클라이언트의 닉네임을 설정하거나 변경합니다.
 * @param cs 클라이언트 소켓의 파일 디스크립터(FD) 번호
 * @param cmd_line 클라이언트가 전송한 명령어 전체 문자열
 */
void env::handle_command_nick(int cs, const std::string& cmd_line)
{
	if (cmd_line.size() <= 4)
		return;

	// 1. "NICK" 이후의 공백 문자 스킵
	size_t start = 4;
	while (start < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[start])))
		start++;
	if (start >= cmd_line.length())
		return;

	// 2. 닉네임 문자열의 끝(공백)을 찾아 단어 추출
	size_t end = start;
	while (end < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[end])))
		end++;

	// 3. 닉네임 최대 길이 제한 (9글자 자르기)
	std::string new_nick = cmd_line.substr(start, end - start);
	if (new_nick.length() > 9)
		new_nick = new_nick.substr(0, 9);

	// 4. 닉네임 유효성 검사 (숫자로 시작 금지, 특정 특수문자 금지)
	bool is_valid = true;
	if (new_nick.empty())
	{
		is_valid = false;
	}
	else
	{
		if (new_nick[0] >= '0' && new_nick[0] <= '9')
		{
			is_valid = false;
		}
		for (size_t i = 0; i < new_nick.length(); ++i)
		{
			char ch = new_nick[i];
			if (ch == ',' || ch == '*' || ch == '?' || ch == '!' || ch == '@' || ch == '.')
			{
				is_valid = false;
			}
		}
	}

	// 5. 서버 내 다른 클라이언트와의 닉네임 중복 검사
	for (size_t i = 0; i < fds.size(); ++i)
	{
		if (fds[i].type == FD_CLIENT && static_cast<int>(i) != cs && fds[i].nickname == new_nick)
		{
			// 중복 발견: 433 ERR_NICKNAMEINUSE 전송 후 중단
			std::cout << "Conflict: Nickname '" << new_nick << "' is already in use by FD #" << i << std::endl;
			fds[cs].buf_write += ":ircserv 433 " + new_nick + " :Nickname is already in use.\r\n";
			return;
		}
	}

	// 6. 기존에 이미 환영 메시지를 받은(등록 완료된) 사용자가 닉네임을 변경하는 경우
	if (fds[cs].is_welcomed)
	{
		// 닉네임 변경 알림 패킷 생성
		std::string nick_change_msg = ":" + fds[cs].nickname + "!" + fds[cs].username + "@127.0.0.1 NICK :" + new_nick + "\r\n";
		fds[cs].buf_write += nick_change_msg; // 본인에게 전송
		
		// 서버 내의 다른 모든 클라이언트에게도 변경 사항 브로드캐스트
		for (size_t i = 0; i < fds.size(); ++i)
		{
			if (fds[i].type == FD_CLIENT && static_cast<int>(i) != cs)
			{
				fds[i].buf_write += nick_change_msg;
			}
		}
	}

	// 7. 클라이언트 구조체에 새로운 닉네임 적용
	fds[cs].nickname = new_nick;
	std::cout << "Client #" << cs << " set nickname to: " << fds[cs].nickname << std::endl;

	// 8. 신규 접속자 조건 충족 시 환영 메시지(001 RPL_WELCOME) 전송
	// PASS 인증 완료 상태에서 NICK과 USER가 모두 등록되었을 때 최초 1회만 수행
	if (!fds[cs].nickname.empty() && !fds[cs].username.empty() && fds[cs].is_welcomed == false)
	{
		fds[cs].is_welcomed = true;
		fds[cs].buf_write += ":ircserv 001 " + fds[cs].nickname + " :Welcome to the Internet Relay Network " + fds[cs].nickname + "\r\n";
	}
}

/**
 * @brief USER 명령어를 처리하여 클라이언트의 사용자 이름을 설정합니다.
 * @param cs 클라이언트 소켓의 파일 디스크립터(FD) 번호
 * @param cmd_line 클라이언트가 전송한 명령어 전체 문자열
 */
void env::handle_command_user(int cs, const std::string& cmd_line)
{
	if (cmd_line.size() <= 4)
		return;

	// 1. "USER" 이후의 공백 문자 스킵
	size_t start = 4;
	while (start < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[start])))
		start++;
	if (start >= cmd_line.length())
		return;

	// 2. 사용자 이름 문자열의 끝(공백)을 찾아 단어 추출
	size_t end = start;
	while (end < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[end])))
		end++;

	// 3. 사용자 이름 최대 길이 제한 (9글자 자르기)
	std::string parsed_user = cmd_line.substr(start, end - start);
	if (parsed_user.length() > 9)
		parsed_user = parsed_user.substr(0, 9);
	
	// 4. 클라이언트 구조체에 사용자 이름 저장
	fds[cs].username = parsed_user;
	std::cout << "Client #" << cs << " set username to: " << fds[cs].username << std::endl;

	// 5. 신규 접속자 조건 충족 시 환영 메시지(001 RPL_WELCOME) 전송
	// PASS 인증 완료 상태에서 NICK과 USER가 모두 등록되었을 때 최초 1회만 수행
	if (!fds[cs].nickname.empty() && !fds[cs].username.empty() && fds[cs].is_welcomed == false)
	{
		fds[cs].is_welcomed = true;
		fds[cs].buf_write += ":ircserv 001 " + fds[cs].nickname + " :Welcome to the Internet Relay Network " + fds[cs].nickname + "\r\n";
	}
}

/**
 * @brief PRIVMSG 명령어를 처리하여 특정 대상(채널 혹은 개인)에게 메시지를 전송합니다.
 * @param cs 클라이언트 소켓의 파일 디스크립터(FD) 번호
 * @param cmd_line 클라이언트가 전송한 명령어 전체 문자열
 */
void env::handle_command_privmsg(int cs, const std::string& cmd_line)
{
	// 1. PASS 비밀번호 인증을 거치지 않은 클라이언트 차단 (451 ERR_NOTREGISTERED)
	if (fds[cs].type != FD_CLIENT)
	{
		fds[cs].buf_write += ":ircserv 451 * :You must specify a password first (PASS).\r\n";
		return;
	}

	// 2. "PRIVMSG" 이후의 공백 문자 스킵
	size_t idx = 7;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 메시지 수신 대상(Target)이 없는 경우 (411 ERR_NORECIPIENT)
	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 411 " + (fds[cs].nickname.empty() ? "*" : fds[cs].nickname) + " :No recipient given (PRIVMSG)\r\n";
		return;
	}

	// 3. 수신 대상(채널명 혹은 닉네임) 문자열 추출
	size_t target_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])) && cmd_line[idx] != ':')
	{
		idx++;
	}
	std::string target = cmd_line.substr(target_start, idx - target_start);

	// 4. 대상과 메시지 본문 사이의 공백 문자 스킵
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 메시지 본문 내용이 없는 경우 (412 ERR_NOTEXTTOSEND)
	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 412 " + fds[cs].nickname + " :No text to send\r\n";
		return;
	}

	// 5. 전송할 본문 메시지 파싱 (IRC 프로토콜에 따른 ':' 접두사 처리)
	std::string pure_message;
	if (cmd_line[idx] == ':')
	{
		pure_message = cmd_line.substr(idx + 1); // ':' 이후의 모든 문자열을 메시지로 취급
	}
	else
	{
		pure_message = cmd_line.substr(idx);
	}

	// 6. 파싱된 대상과 메시지를 라우팅 및 라디오 방송하기 위해 내부 브로드캐스트 함수 호출
	broadcast_message(cs, target, pure_message);
}

/**
 * @brief JOIN 명령어를 처리하여 클라이언트를 특정 채널에 입장시킵니다. 채널이 없으면 새로 생성합니다.
 * @param cs 클라이언트 소켓의 파일 디스크립터(FD) 번호
 * @param cmd_line 클라이언트가 전송한 명령어 전체 문자열
 */
void env::handle_command_join(int cs, const std::string& cmd_line)
{
	// 1. "JOIN" 이후의 공백 문자 스킵
	size_t idx = 4;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 채널명 인자가 부족한 경우 (461 ERR_NEEDMOREPARAMS)
	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 461 " + fds[cs].nickname + " JOIN :Not enough parameters\r\n";
		return;
	}

	// 2. 채널 이름 추출
	size_t ch_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}
	std::string ch_name = cmd_line.substr(ch_start, idx - ch_start);

	if (ch_name.empty())
	{
		return;
	}
	
	// 3. 채널명 유효성 검사 (IRC 표준에 따라 # 또는 &로 시작해야 함)
	if (ch_name[0] != '#' && ch_name[0] != '&')
	{
		fds[cs].buf_write += ":ircserv 403 " + fds[cs].nickname + " " + ch_name + " :No such channel\r\n";
		return;
	}

	// 4. 서버 내부 채널 맵 목록에서 해당 채널이 존재하는지 조회
	std::map<std::string, Channel>::iterator it = channels.find(ch_name);
	
	if (it == channels.end())
	{
		// [채널 신규 생성 분기] 채널이 존재하지 않으면 새로 생성하고 생성자를 채널 오퍼레이터(방장)로 설정
		std::cout << "Creating new channel: " << ch_name << " by Client #" << cs << std::endl;
		
		Channel new_channel(ch_name);
		new_channel.add_client(cs);
		new_channel.add_operator(cs); // 최초 생성자에게 오퍼레이터 권한 부여
		
		channels.insert(std::make_pair(ch_name, new_channel));
	}
	else
	{
		// [기존 채널 입장 분기] 이미 존재하는 채널이면 구성원 리스트에만 추가
		std::cout << "Client #" << cs << " joining existing channel: " << ch_name << std::endl;
		it->second.add_client(cs);
	}

	// 5. 채널 입장 알림 패킷 생성 및 채널 내 모든 멤버에게 브로드캐스트
	std::string join_notif = ":" + fds[cs].nickname + "!" + fds[cs].username + "@127.0.0.1 JOIN :" + ch_name + "\r\n";
	fds[cs].buf_write += join_notif; // 본인에게 전송

	std::vector<int> members = channels[ch_name].get_client_fds();
	for (size_t i = 0; i < members.size(); ++i)
	{
		int target_fd = members[i];
		if (target_fd != cs)
		{
			fds[target_fd].buf_write += join_notif; // 다른 멤버들에게 전송
		}
	}

	// 6. 입장한 채널의 토픽 상태 안내 (331 RPL_NOTOPIC 또는 332 RPL_TOPIC)
	std::string current_topic = channels[ch_name].get_topic();
	if (current_topic.empty())
	{
		fds[cs].buf_write += ":ircserv 331 " + fds[cs].nickname + " " + ch_name + " :No topic is set\r\n";
	}
	else
	{
		fds[cs].buf_write += ":ircserv 332 " + fds[cs].nickname + " " + ch_name + " :" + current_topic + "\r\n";
	}
}

/**
 * @brief PART 명령어를 처리하여 클라이언트를 특정 채널에서 퇴장시킵니다. 채널이 비게 되면 채널을 제거합니다.
 * @param cs 클라이언트 소켓의 파일 디스크립터(FD) 번호
 * @param cmd_line 클라이언트가 전송한 명령어 전체 문자열
 */
void env::handle_command_part(int cs, const std::string& cmd_line)
{
	// 1. "PART" 이후의 공백 문자 스킵
	size_t idx = 4;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 퇴장할 채널 인자가 부족한 경우 (461 ERR_NEEDMOREPARAMS)
	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 461 " + fds[cs].nickname + " PART :Not enough parameters\r\n";
		return;
	}

	// 2. 채널 이름 추출
	size_t ch_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])) && cmd_line[idx] != ':')
	{
		idx++;
	}
	std::string ch_name = cmd_line.substr(ch_start, idx - ch_start);

	// 3. 퇴장 사유(Reason) 유효성 파싱 준비을 위한 공백 스킵
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 퇴장 사유가 존재한다면 추출
	std::string reason = "";
	if (idx < cmd_line.length())
	{
		if (cmd_line[idx] == ':')
		{
			reason = cmd_line.substr(idx + 1);
		}
		else
		{
			reason = cmd_line.substr(idx);
		}
	}

	// 4. 채널 존재 여부 검사 (403 ERR_NOSUCHCHANNEL)
	std::map<std::string, Channel>::iterator it = channels.find(ch_name);
	if (it == channels.end())
	{
		fds[cs].buf_write += ":ircserv 403 " + fds[cs].nickname + " " + ch_name + " :No such channel\r\n";
		return;
	}

	Channel& channel = it->second;

	// 5. 요청한 클라이언트가 해당 채널의 일원인지 검사 (442 ERR_NOTONCHANNEL)
	if (channel.is_member(cs) == false)
	{
		fds[cs].buf_write += ":ircserv 442 " + fds[cs].nickname + " " + ch_name + " :You're not on that channel\r\n";
		return;
	}

	// 6. 채널 퇴장 알림 패킷 생성 및 채널 내 모든 멤버(본인 포함)에게 전송
	std::string part_packet = ":" + fds[cs].nickname + "!" + fds[cs].username + "@127.0.0.1 PART " + ch_name;
	if (reason.empty() == false)
	{
		part_packet += " :" + reason;
	}
	part_packet += "\r\n";

	std::vector<int> members = channel.get_client_fds();
	for (size_t i = 0; i < members.size(); ++i)
	{
		int target_fd = members[i];
		fds[target_fd].buf_write += part_packet;
	}

	// 7. 채널 내부 오브젝트에서 클라이언트 완전히 제거
	channel.remove_client(cs);
	std::cout << "Client #" << cs << " left channel: " << ch_name << std::endl;

	// 8. 채널에 남은 인원이 없는 경우, 자원 낭비를 방지하기 위해 채널 리소스 소멸
	std::vector<int> remaining_members = channel.get_client_fds();
	if (remaining_members.empty() == true)
	{
		std::cout << "Channel " << ch_name << " is now empty. Dissolving channel resource." << std::endl;
		channels.erase(it);
	}
}

/**
 * @brief KICK 명령어를 처리하여 채널 관리자가 특정 유저를 채널에서 강제로 추방합니다.
 * @param cs 명령을 내린 클라이언트 소켓의 파일 디스크립터(FD) 번호
 * @param cmd_line 클라이언트가 전송한 명령어 전체 문자열
 */
void env::handle_command_kick(int cs, const std::string& cmd_line)
{
	// 1. "KICK" 이후의 공백 문자 스킵
	size_t idx = 4;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	std::string sender_nick = fds[cs].nickname;
	// 인자가 부족한 경우 예외 처리
	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 461 " + sender_nick + " KICK :Not enough parameters\r\n";
		return;
	}

	// 2. 대상 채널 이름 추출
	size_t ch_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}
	std::string ch_name = cmd_line.substr(ch_start, idx - ch_start);

	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 대상 유저 인자가 부족한 경우 예외 처리
	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 461 " + sender_nick + " KICK :Not enough parameters\r\n";
		return;
	}

	// 3. 강제 추방 대상 유저(Target/Victim)의 닉네임 추출
	size_t usr_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])) && cmd_line[idx] != ':')
	{
		idx++;
	}
	std::string target_user = cmd_line.substr(usr_start, idx - usr_start);

	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 4. 강제 추방 사유(Comment) 파싱 (지정되지 않았다면 기본값 설정)
	std::string comment = "Kicked by operator";
	if (idx < cmd_line.length())
	{
		if (cmd_line[idx] == ':')
		{
			comment = cmd_line.substr(idx + 1);
		}
		else
		{
			comment = cmd_line.substr(idx);
		}
	}

	// 5. 권한 및 상태 유효성 검사 시작
	// 5-1. 채널 존재 여부 검사 (403 ERR_NOSUCHCHANNEL)
	std::map<std::string, Channel>::iterator it = channels.find(ch_name);
	if (it == channels.end())
	{
		fds[cs].buf_write += ":ircserv 403 " + sender_nick + " " + ch_name + " :No such channel\r\n";
		return;
	}

	Channel& channel = it->second;

	// 5-2. 명령을 내린 유저가 해당 채널 소속인지 검사 (442 ERR_NOTONCHANNEL)
	if (channel.is_member(cs) == false)
	{
		fds[cs].buf_write += ":ircserv 442 " + sender_nick + " " + ch_name + " :You're not on that channel\r\n";
		return;
	}

	// 5-3. 명령을 내린 유저가 채널 오퍼레이터 권한을 가졌는지 검사 (482 ERR_CHANOPRIVSNEEDED)
	if (channel.is_operator(cs) == false)
	{
		fds[cs].buf_write += ":ircserv 482 " + sender_nick + " " + ch_name + " :You're not channel operator\r\n";
		return;
	}

	// 6. 강제 추방 대상(Victim) 유저의 파일 디스크립터(FD) 조회 및 검증
	int victim_fd = -1;
	for (size_t i = 0; i < fds.size(); ++i)
	{
		if (fds[i].type == FD_CLIENT && fds[i].nickname == target_user)
		{
			victim_fd = static_cast<int>(i);
			break;
		}
	}

	// 추방 대상 유저가 서버에 없거나 채널의 일원이 아닌 경우 (441 ERR_USERNOTINCHANNEL)
	if (victim_fd == -1 || channel.is_member(victim_fd) == false)
	{
		fds[cs].buf_write += ":ircserv 441 " + sender_nick + " " + target_user + " " + ch_name + " :They aren't on that channel\r\n";
		return;
	}

	// 7. KICK 패킷 및 강제 퇴장 알림(PART) 패킷 생성
	std::string kick_packet = ":" + sender_nick + "!" + fds[cs].username + "@127.0.0.1 KICK " + ch_name + " " + target_user + " :" + comment + "\r\n";
	std::string forced_part_packet = ":" + target_user + "!" + fds[victim_fd].username + "@127.0.0.1 PART " + ch_name + " :Kicked by " + sender_nick + "\r\n";

	// 8. 채널 내 모든 구성원에게 KICK 상태 전달 및 강제 플러시
	std::vector<int> members = channel.get_client_fds();
	for (size_t i = 0; i < members.size(); ++i)
	{
		int member_fd = members[i];
		
		// 당사자(Victim)에게는 방에서 쫓겨났음을 알리는 강제 PART 패킷도 함께 삽입
		if (member_fd == victim_fd)
		{
			fds[member_fd].buf_write += forced_part_packet;
		}
		fds[member_fd].buf_write += kick_packet;
		client_write(member_fd); // 중요 패킷이므로 소켓에 즉시 write 시도
	}

	// 9. 채널 객체 내부에서 추방된 유저 완전 제거
	channel.remove_client(victim_fd);
	std::cout << "Client #" << victim_fd << " was kicked and forced to part from " << ch_name << std::endl;

	// 10. KICK 집행 이후 채널 멤버가 전멸했다면 채널 리소스 소멸
	std::vector<int> remaining_members = channel.get_client_fds();
	if (remaining_members.empty() == true)
	{
		std::cout << "Channel " << ch_name << " became empty after KICK. Dissolving channel." << std::endl;
		channels.erase(it);
	}
}

/**
 * @brief INVITE 명령어를 처리하여 특정 유저를 기존 채널에 초대합니다.
 * @param cs 초대를 보낸 클라이언트 소켓의 파일 디스크립터(FD) 번호
 * @param cmd_line 클라이언트가 전송한 명령어 전체 문자열
 */
void env::handle_command_invite(int cs, const std::string& cmd_line)
{
	std::string sender_nick = fds[cs].nickname;

	// 1. "INVITE" 이후의 공백 문자 스킵
	size_t idx = 6;
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 461 " + sender_nick + " INVITE :Not enough parameters\r\n";
		return;
	}

	// 2. 초대할 대상 유저의 닉네임 추출
	size_t nick_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}
	std::string target_nick = cmd_line.substr(nick_start, idx - nick_start);

	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 461 " + sender_nick + " INVITE :Not enough parameters\r\n";
		return;
	}

	// 3. 초대 대상 채널 이름 추출
	size_t ch_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}
	std::string raw_ch_name = cmd_line.substr(ch_start, idx - ch_start);

	// 개행 문(\r, \n)으로 인한 패킷 훼손 방지를 위해 채널명 필터링 정제
	std::string ch_name = "";
	for (size_t i = 0; i < raw_ch_name.length(); ++i)
	{
		if (raw_ch_name[i] != '\r' && raw_ch_name[i] != '\n')
		{
			ch_name += raw_ch_name[i];
		}
	}

	// 4. 초대 대상 유저(Target)가 서버에 존재하는지 검색 (401 ERR_NOSUCHNICK)
	int target_fd = -1;
	for (size_t i = 0; i < fds.size(); ++i)
	{
		if (fds[i].type == FD_CLIENT && fds[i].nickname == target_nick)
		{
			target_fd = static_cast<int>(i);
			break;
		}
	}

	if (target_fd == -1)
	{
		fds[cs].buf_write += ":ircserv 401 " + sender_nick + " " + target_nick + " :No such nick\r\n";
		return;
	}

	// 5. 채널 소속 상태 및 유효성 검사
	std::map<std::string, Channel>::iterator it = channels.find(ch_name);
	if (it != channels.end())
	{
		Channel& channel = it->second;

		// 5-1. 초대를 보내는 주체가 채널의 일원인지 확인 (442 ERR_NOTONCHANNEL)
		if (channel.is_member(cs) == false)
		{
			fds[cs].buf_write += ":ircserv 442 " + sender_nick + " " + ch_name + " :You're not on that channel\r\n";
			return;
		}

		// 5-2. 초대 대상 유저가 이미 채널에 참여 중인지 확인 (443 ERR_USERONCHANNEL)
		if (channel.is_member(target_fd) == true)
		{
			fds[cs].buf_write += ":ircserv 443 " + sender_nick + " " + target_nick + " " + ch_name + " :is already on channel\r\n";
			return;
		}
	}

	// 6. 안전장치: 유저네임 공백 시 임시 기본값 처리
	std::string safe_username = fds[cs].username;
	if (safe_username.empty() == true)
	{
		safe_username = "unknown";
	}

	// 7. 초대장 수신을 위한 INVITE 패킷 조립
	std::string invite_packet = ":" + sender_nick + "!" + safe_username + "@127.0.0.1 INVITE " 
	                          + target_nick + " :" + ch_name + "\r\n";

	// 8. 패킷 전송 처리
	fds[target_fd].buf_write += invite_packet; // 초대를 받는 당사자에게 전송
	fds[cs].buf_write += ":ircserv 341 " + sender_nick + " " + target_nick + " " + ch_name + "\r\n"; // 발송자에게 성공 응답(341 RPL_INVITING)
	
	// 즉시 네트워크 버퍼 플러시
	client_write(target_fd);
	client_write(cs);
	std::cout << "Client #" << cs << " successfully invited " << target_nick << " to " << ch_name << std::endl;
}

/**
 * @brief TOPIC 명령어를 처리하여 채널의 주제(Topic)를 조회하거나 변경합니다.
 * @param cs 명령을 내린 클라이언트 소켓의 파일 디스크립터(FD) 번호
 * @param cmd_line 클라이언트가 전송한 명령어 전체 문자열
 */
void env::handle_command_topic(int cs, const std::string& cmd_line)
{
	std::string sender_nick = fds[cs].nickname;
	
	// irssi 클라이언트 호환성 보장용 안전장치 (username 누락 방지)
	std::string safe_username = fds[cs].username;
	if (safe_username.empty() == true)
	{
		safe_username = "unknown";
	}

	// 1. 첫 번째 인자 (채널명) 파싱 준비
	size_t idx = 5; // "TOPIC" 명령어 본문 스킵 길이
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}

	// 채널명조차 누락된 경우 (461 ERR_NEEDMOREPARAMS)
	if (idx >= cmd_line.length())
	{
		fds[cs].buf_write += ":ircserv 461 " + sender_nick + " TOPIC :Not enough parameters\r\n";
		return;
	}

	// 2. 채널명 추출
	size_t ch_start = idx;
	while (idx < cmd_line.length() && !std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		idx++;
	}
	std::string raw_ch_name = cmd_line.substr(ch_start, idx - ch_start);

	// 채널명에서 개행 문자(\r, \n) 강제 필터링 제거
	std::string ch_name = "";
	for (size_t i = 0; i < raw_ch_name.length(); ++i)
	{
		if (raw_ch_name[i] != '\r' && raw_ch_name[i] != '\n')
		{
			ch_name += raw_ch_name[i];
		}
	}

	// 3. 두 번째 인자(새로운 토픽 내용) 유무 파악을 위한 공백 정밀 파싱
	while (idx < cmd_line.length() && std::isspace(static_cast<unsigned char>(cmd_line[idx])))
	{
		// 개행 문자를 만나면 인자가 끝난 것이므로 스킵을 멈추고 조회(View) 모드로 분기 유도
		if (cmd_line[idx] == '\r' || cmd_line[idx] == '\n')
		{
			break;
		}
		idx++;
	}

	bool is_viewing = true; // 기본값은 조회 모드
	std::string new_topic = "";

	// 뒤에 인자가 더 남아있다면 변경(Change) 모드로 전환
	if (idx < cmd_line.length() && cmd_line[idx] != '\r' && cmd_line[idx] != '\n')
	{
		is_viewing = false;
		if (cmd_line[idx] == ':')
		{
			new_topic = cmd_line.substr(idx + 1); // 콜론 기호 뒷부분 전체를 토픽으로 처리
		}
		else
		{
			new_topic = cmd_line.substr(idx);
		}

		// 새 토픽 내용에서 오염 유발 문자(\r, \n) 필터링 (화면 깨짐 방지)
		std::string clean_topic = "";
		for (size_t i = 0; i < new_topic.length(); ++i)
		{
			if (new_topic[i] != '\r' && new_topic[i] != '\n')
			{
				clean_topic += new_topic[i];
			}
		}
		new_topic = clean_topic;
	}

	// 4. 채널 소속 검사 유효성 체크
	// 4-1. 해당 채널이 실제로 존재하는지 검사 (442 ERR_NOTONCHANNEL)
	std::map<std::string, Channel>::iterator it = channels.find(ch_name);
	if (it == channels.end())
	{
		fds[cs].buf_write += ":ircserv 442 " + sender_nick + " " + ch_name + " :You're not on that channel\r\n";
		return;
	}

	Channel& channel = it->second;

	// 4-2. 명령을 시도한 클라이언트가 해당 채널 멤버인지 검사 (442 ERR_NOTONCHANNEL)
	if (channel.is_member(cs) == false)
	{
		fds[cs].buf_write += ":ircserv 442 " + sender_nick + " " + ch_name + " :You're not on that channel\r\n";
		return;
	}

	// 5. 모드(조회 vs 변경)에 따른 최종 비즈니스 로직 수행
	if (is_viewing == true)
	{
		// [조회 모드 분기] 현재 등록된 방의 토픽 확인 후 응답 패킷 전송
		std::string current_topic = channel.get_topic();
		if (current_topic.empty() == true)
		{
			fds[cs].buf_write += ":ircserv 331 " + sender_nick + " " + ch_name + " :No topic is set\r\n"; // 331 RPL_NOTOPIC
		}
		else
		{
			fds[cs].buf_write += ":ircserv 332 " + sender_nick + " " + ch_name + " :" + current_topic + "\r\n"; // 332 RPL_TOPIC
		}
		client_write(cs); // 데이터 즉시 송신 플러시
	}
	else
	{
		// [변경 모드 분기] 채널 객체의 토픽 정보를 새 값으로 갱신
		channel.set_topic(new_topic);

		// 변경 알림 패킷을 조립하여 실시간 동기화를 위해 방 내부의 모든 멤버에게 전송
		std::string topic_packet = ":" + sender_nick + "!" + safe_username + "@127.0.0.1 TOPIC " 
		                         + ch_name + " :" + new_topic + "\r\n";

		std::vector<int> members = channel.get_client_fds();
		for (size_t i = 0; i < members.size(); ++i)
		{
			int member_fd = members[i];
			fds[member_fd].buf_write += topic_packet;
			client_write(member_fd); // 상단 타이틀바 실시간 갱신 처리를 위해 소켓에 즉시 쓰기
		}
		std::cout << "Client #" << cs << " changed topic of " << ch_name << " to: " << new_topic << std::endl;
	}
}