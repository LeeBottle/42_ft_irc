# ft_irc RFC 구현 범위 정리

이 문서는 ft_irc mandatory에서 실제로 구현해야 하는 RFC 내용만 추린 것이다.

주요 기준:

- RFC 2812: IRC Client Protocol
- RFC 2811: IRC Channel Management
- RFC 1459: IRC Protocol, 구형이지만 42 과제 설명과 명령 개념을 맞춰 볼 때 참고

서버 간 통신은 과제에서 금지되어 있으므로 RFC 2813 서버 프로토콜은 구현 대상에서 제외한다.

## 참고 링크

- RFC 2812: https://www.rfc-editor.org/rfc/rfc2812
- RFC 2811: https://www.rfc-editor.org/rfc/rfc2811
- RFC 1459: https://www.rfc-editor.org/rfc/rfc1459

## 1. IRC 메시지 기본 형식

RFC 2812 기준으로 IRC 메시지는 TCP stream에서 CR-LF, 즉 `\r\n`을 기준으로 분리한다. 빈 메시지는 무시한다.

구현해야 할 핵심:

- 클라이언트에서 받은 바이트를 바로 명령으로 처리하지 말고 client별 read buffer에 누적한다.
- `\r\n` 또는 실전 호환을 위해 `\n`이 나올 때까지 기다린다.
- 완성된 line만 parser에 넘긴다.
- 한 번의 `recv`에 여러 명령이 들어올 수 있으므로 line을 반복해서 꺼낸다.
- 명령 한 줄은 CR-LF 포함 최대 512 bytes, 실제 내용은 최대 510 bytes로 제한한다.

형식:

```text
[:prefix] COMMAND [params...] [:trailing text]\r\n
```

클라이언트가 서버로 보내는 메시지는 보통 prefix 없이 온다. 서버가 클라이언트에게 전달할 때는 보통 아래 prefix를 붙인다.

```text
:nick!user@host COMMAND ...
```

파서에서 중요한 점:

- command는 대소문자를 구분하지 않는 방식으로 처리하는 것이 안전하다.
- `:`로 시작하는 trailing parameter는 공백을 포함할 수 있다.
- `PRIVMSG #chan :hello world`의 메시지 본문은 `hello world` 하나의 parameter다.
- parameter는 일반적으로 최대 15개 구조를 따른다.

## 2. 등록 흐름: PASS, NICK, USER

RFC 2812의 권장 등록 순서:

```text
PASS <password>
NICK <nickname>
USER <user> <mode> <unused> <realname>
```

과제에서는 실행 인자로 password를 받으므로 `PASS` 검사를 필수로 두는 것이 맞다.

등록 완료 조건:

- `PASS`가 서버 비밀번호와 일치
- 유효하고 중복되지 않는 `NICK` 설정
- `USER` 수신

등록이 완료되면 최소한 `001 RPL_WELCOME`을 보내야 실제 IRC 클라이언트가 정상 접속으로 인식하기 쉽다. 가능하면 `002`, `003`, `004`도 함께 보낸다.

예시 응답:

```text
:ircserv 001 nick :Welcome to the Internet Relay Network nick!user@host\r\n
:ircserv 002 nick :Your host is ircserv, running version ft_irc-1.0\r\n
:ircserv 003 nick :This server was created <date>\r\n
:ircserv 004 nick ircserv ft_irc-1.0 o itklo\r\n
```

### PASS

문법:

```text
PASS <password>
```

구현:

- 등록 전에만 허용한다.
- 인자가 없으면 `461 ERR_NEEDMOREPARAMS`.
- 이미 등록된 뒤 다시 오면 `462 ERR_ALREADYREGISTRED`.
- 비밀번호가 틀리면 `464 ERR_PASSWDMISMATCH` 후 연결 종료 예약.
- 비밀번호 오류 응답도 즉시 `send`하지 말고 write buffer에 넣은 뒤 `EPOLLOUT`에서 전송한다.

### NICK

문법:

```text
NICK <nickname>
```

구현:

- 인자가 없으면 `431 ERR_NONICKNAMEGIVEN`.
- 형식이 잘못되면 `432 ERR_ERRONEUSNICKNAME`.
- 이미 사용 중이면 `433 ERR_NICKNAMEINUSE`.
- 등록 후 닉 변경도 허용할 수 있다.
- 닉 변경 시 사용자가 들어가 있는 채널 멤버들에게 `:old!user@host NICK new`를 전파한다.

현실적인 ft_irc 닉네임 검증:

- 첫 글자: 영문자 또는 특수문자 `[]\`_^{|}`
- 이후: 영문자, 숫자, 위 특수문자, `-`
- 공백, `,`, `*`, `?`, `!`, `@`, `.` 등은 거부하는 편이 안전하다.

### USER

문법:

```text
USER <user> <mode> <unused> <realname>
```

구현:

- 등록 전에만 허용한다.
- parameter 4개가 필요하다.
- `<realname>`은 `:` trailing parameter로 올 수 있으므로 공백 포함 가능해야 한다.
- 이미 등록된 뒤 다시 오면 `462 ERR_ALREADYREGISTRED`.
- `mode`는 mandatory에서 실질적으로 무시해도 된다.

## 3. JOIN

문법:

```text
JOIN <channel>[,<channel>...] [<key>[,<key>...]]
JOIN 0
```

ft_irc mandatory에서는 여러 채널 목록을 전부 지원하지 않아도, parser가 최소한 단일 채널을 안정적으로 처리해야 한다. 다만 RFC는 comma list를 허용하므로 구현 여유가 있으면 분리 처리한다.

구현:

- 등록되지 않은 사용자는 거부한다.
- 인자가 없으면 `461 ERR_NEEDMOREPARAMS`.
- 채널이 없으면 생성한다.
- 채널을 생성한 첫 사용자는 channel operator가 된다.
- 성공하면 해당 채널 멤버들에게 JOIN 메시지를 보낸다.
- JOIN 성공 후 접속자에게 topic과 names list를 보낸다.

성공 시 기본 응답 흐름:

```text
:nick!user@host JOIN #channel\r\n
:ircserv 332 nick #channel :topic text\r\n
:ircserv 353 nick = #channel :@nick other\r\n
:ircserv 366 nick #channel :End of NAMES list\r\n
```

topic이 없으면 `331 RPL_NOTOPIC`을 보낸다.

JOIN에서 확인할 channel mode:

- `+i`: invite-only이면 invited 목록에 있는 사용자만 입장 가능
- `+k`: key가 설정되어 있으면 JOIN key가 일치해야 함
- `+l`: user limit에 도달하면 입장 거부

관련 에러:

- `473 ERR_INVITEONLYCHAN`
- `475 ERR_BADCHANNELKEY`
- `471 ERR_CHANNELISFULL`
- `403 ERR_NOSUCHCHANNEL`

## 4. PRIVMSG

문법:

```text
PRIVMSG <target> :<text>
```

target은 nickname 또는 channel name이다.

구현:

- 인자가 없으면 `411 ERR_NORECIPIENT`.
- text가 없으면 `412 ERR_NOTEXTTOSEND`.
- target nick/channel이 없으면 `401 ERR_NOSUCHNICK` 또는 `403 ERR_NOSUCHCHANNEL`.
- target이 nick이면 해당 클라이언트에게만 전달한다.
- target이 channel이면 같은 채널의 다른 멤버들에게 전달한다.
- 보낸 사람 자신에게는 channel PRIVMSG를 다시 보내지 않는 것이 일반적이다.

전달 형식:

```text
:nick!user@host PRIVMSG target :message\r\n
```

주의:

- 과제 요구사항상 채널에 보낸 메시지는 같은 채널의 다른 클라이언트에게 모두 전달되어야 한다.
- RFC에는 host mask/server mask target도 있지만 mandatory에서는 구현하지 않아도 된다.

## 5. Channel Operator 개념

RFC 1459와 ft_irc subject에서 channel operator 전용 명령으로 보는 것:

- `KICK`
- `INVITE`
- `TOPIC`, 단 `+t`일 때
- `MODE`

구현해야 할 상태:

```cpp
members
operators
invited
topic
inviteOnly       // +i
topicOpOnly      // +t
key              // +k
userLimit        // +l
```

첫 JOIN으로 채널을 만든 사용자를 operator로 등록한다.

권한 부족 시:

```text
482 ERR_CHANOPRIVSNEEDED
```

## 6. KICK

문법:

```text
KICK <channel> <user> [:comment]
```

구현:

- 인자가 부족하면 `461 ERR_NEEDMOREPARAMS`.
- 채널이 없으면 `403 ERR_NOSUCHCHANNEL`.
- 요청자가 채널에 없으면 `442 ERR_NOTONCHANNEL`.
- 대상 사용자가 채널에 없으면 `441 ERR_USERNOTINCHANNEL`.
- 요청자가 channel operator가 아니면 `482 ERR_CHANOPRIVSNEEDED`.
- 성공하면 채널 멤버와 kick 대상에게 KICK 메시지를 전파한 뒤 대상자를 채널에서 제거한다.
- comment가 없으면 요청자의 nickname을 기본 reason으로 써도 된다.

전파 형식:

```text
:opnick!user@host KICK #channel target :reason\r\n
```

RFC는 여러 channel/user 조합도 허용하지만, ft_irc에서는 단일 channel + 단일 user부터 정확히 구현하는 것이 우선이다. 서버가 클라이언트에게 보낼 때는 여러 channel/user를 한 메시지로 보내지 않는다.

## 7. INVITE

문법:

```text
INVITE <nickname> <channel>
```

구현:

- 인자가 부족하면 `461 ERR_NEEDMOREPARAMS`.
- 초대 대상 nick이 없으면 `401 ERR_NOSUCHNICK`.
- 채널이 존재하고 요청자가 그 채널 멤버가 아니면 `442 ERR_NOTONCHANNEL`.
- 대상이 이미 채널에 있으면 `443 ERR_USERONCHANNEL`.
- 채널이 `+i`인 경우 요청자가 channel operator가 아니면 `482 ERR_CHANOPRIVSNEEDED`.
- 성공하면 초대 대상자를 channel의 invited 목록에 추가한다.
- 요청자에게 `341 RPL_INVITING`을 보낸다.
- 초대 대상자에게 INVITE 메시지를 보낸다.

전달 형식:

```text
:nick!user@host INVITE target #channel\r\n
```

## 8. TOPIC

문법:

```text
TOPIC <channel> [:topic]
```

구현:

- channel만 있으면 topic 조회.
- topic parameter가 있으면 topic 변경.
- `TOPIC #chan :`처럼 빈 trailing이면 topic 제거.
- 채널이 없으면 `403 ERR_NOSUCHCHANNEL`.
- 요청자가 채널에 없으면 `442 ERR_NOTONCHANNEL`.
- topic이 없으면 `331 RPL_NOTOPIC`.
- topic이 있으면 `332 RPL_TOPIC`.
- 채널이 `+t`이고 요청자가 channel operator가 아니면 `482 ERR_CHANOPRIVSNEEDED`.
- 성공적으로 topic을 바꾸면 채널 멤버들에게 TOPIC 메시지를 전파한다.

전파 형식:

```text
:nick!user@host TOPIC #channel :new topic\r\n
```

## 9. MODE: channel modes

문법:

```text
MODE <channel> [<modes> [<mode params>...]]
```

조회:

```text
MODE #channel
```

변경:

```text
MODE #channel +i
MODE #channel -i
MODE #channel +t
MODE #channel -t
MODE #channel +k key
MODE #channel -k key
MODE #channel +o nick
MODE #channel -o nick
MODE #channel +l limit
MODE #channel -l
```

공통 구현:

- 채널이 없으면 `403 ERR_NOSUCHCHANNEL`.
- mode 조회는 일반 멤버에게도 허용 가능.
- mode 변경은 channel operator만 허용한다.
- 권한 부족이면 `482 ERR_CHANOPRIVSNEEDED`.
- 알 수 없는 mode면 `472 ERR_UNKNOWNMODE`.
- 성공한 mode 변경은 채널 멤버들에게 전파한다.

전파 형식:

```text
:nick!user@host MODE #channel +i\r\n
:nick!user@host MODE #channel +k secret\r\n
:nick!user@host MODE #channel +o target\r\n
```

### +i / -i: invite-only

의미:

- `+i`: 초대받은 사용자만 JOIN 가능
- `-i`: 제한 해제

관련:

- `INVITE`로 invited 목록에 추가
- invite-only 채널에서 초대 없이 JOIN하면 `473 ERR_INVITEONLYCHAN`

### +t / -t: topic operator only

의미:

- `+t`: channel operator만 TOPIC 변경 가능
- `-t`: 일반 멤버도 TOPIC 변경 가능

### +k / -k: channel key

의미:

- `+k key`: 채널 password 설정
- `-k key`: 채널 password 제거

JOIN 시 key가 틀리면:

```text
475 ERR_BADCHANNELKEY
```

주의:

- key는 MODE 조회 응답에서 채널 멤버에게만 보이도록 하는 것이 RFC 의미에 가깝다.
- ft_irc에서는 최소한 JOIN 검증이 정확해야 한다.

### +o / -o: channel operator

의미:

- `+o nick`: 대상 멤버에게 operator 권한 부여
- `-o nick`: 대상 멤버의 operator 권한 회수

검증:

- 대상 nick이 존재해야 한다.
- 대상이 해당 채널 멤버여야 한다.
- 대상이 채널에 없으면 `441 ERR_USERNOTINCHANNEL`.

### +l / -l: user limit

의미:

- `+l limit`: 채널 최대 인원 설정
- `-l`: 제한 제거

검증:

- limit은 양수 정수여야 한다.
- JOIN 시 현재 멤버 수가 limit 이상이면 `471 ERR_CHANNELISFULL`.

## 10. 필수 numeric reply 목록

ft_irc mandatory에서 우선 구현할 numeric:

```text
001 RPL_WELCOME
002 RPL_YOURHOST
003 RPL_CREATED
004 RPL_MYINFO
```

채널 조회/입장:

```text
331 RPL_NOTOPIC
332 RPL_TOPIC
341 RPL_INVITING
353 RPL_NAMREPLY
366 RPL_ENDOFNAMES
324 RPL_CHANNELMODEIS
```

공통 에러:

```text
401 ERR_NOSUCHNICK
403 ERR_NOSUCHCHANNEL
411 ERR_NORECIPIENT
412 ERR_NOTEXTTOSEND
431 ERR_NONICKNAMEGIVEN
432 ERR_ERRONEUSNICKNAME
433 ERR_NICKNAMEINUSE
441 ERR_USERNOTINCHANNEL
442 ERR_NOTONCHANNEL
443 ERR_USERONCHANNEL
451 ERR_NOTREGISTERED
461 ERR_NEEDMOREPARAMS
462 ERR_ALREADYREGISTRED
464 ERR_PASSWDMISMATCH
471 ERR_CHANNELISFULL
472 ERR_UNKNOWNMODE
473 ERR_INVITEONLYCHAN
475 ERR_BADCHANNELKEY
482 ERR_CHANOPRIVSNEEDED
```

응답 기본 형식:

```text
:ircserv <numeric> <target-nick-or-*> <params> :message\r\n
```

등록 전 nick이 없는 경우 target은 `*`를 사용한다.

## 11. 구현하지 않아도 되는 RFC 범위

과제 mandatory 기준으로 제외해도 되는 것:

- 서버 간 통신
- `SERVICE`, `SQUIT`, `CONNECT`, `LINKS`, `TRACE`, `ADMIN`, `INFO`
- IRC operator `OPER`
- `WHO`, `WHOIS`, `WHOWAS`
- `LIST`, `MOTD`, `VERSION`, `TIME`, `STATS`
- ban mask `+b`, exception `+e`, invitation mask `+I`
- voice `+v`
- moderated `+m`
- secret/private/no-external 메시지 관련 mode
- host mask/server mask 대상 PRIVMSG
- file transfer, bot은 bonus

단, 실제 IRC 클라이언트 호환을 위해 아래는 가볍게 처리하면 좋다.

- `PING` 수신 시 `PONG` 응답
- `CAP` 수신 시 `CAP * LS :` 또는 적절한 무시 응답
- 알 수 없는 명령은 `421 ERR_UNKNOWNCOMMAND`

## 12. ft_irc 구현 우선순위

1. line buffer와 parser
2. `PASS`, `NICK`, `USER`, 등록 완료 응답
3. client map과 nickname map
4. channel map
5. `JOIN` + `353/366`
6. `PRIVMSG` user/channel 전달
7. operator 권한 저장
8. `KICK`
9. `INVITE`
10. `TOPIC`
11. `MODE i/t/k/o/l`
12. irssi 또는 기준 클라이언트로 통합 테스트

## 13. 최소 동작 시나리오

두 클라이언트가 정상 등록:

```text
PASS pass
NICK a
USER a 0 * :A User
```

```text
PASS pass
NICK b
USER b 0 * :B User
```

채널 입장:

```text
JOIN #test
```

채널 메시지:

```text
PRIVMSG #test :hello
```

operator 명령:

```text
MODE #test +i
INVITE b #test
MODE #test +k secret
MODE #test +l 10
MODE #test +o b
TOPIC #test :new topic
KICK #test b :bye
```

이 시나리오가 실제 IRC 클라이언트에서 자연스럽게 동작하면 mandatory 구현의 큰 줄기는 맞다.
