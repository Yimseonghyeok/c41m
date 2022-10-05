#include <stdio.h> // 표준 입출력
#include <sys/stat.h> // 파일의 크기,권한, 정보 등의 함수
#include <string.h> // C형식 문자열 다루는 함수
#include <sys/socket.h> // socket 함수 정의
#include <unistd.h> // POSIX 운영체제 API에 대한 엑세스 제공
#include <stdlib.h> // C 표준 유틸리티 함수 헤더
#include <arpa/inet.h> // 숫자로 ip 주소 조작 기능 정의
// 엔티티 헤더
// 메세지 바디 컨텐츠를 나타냄
#define ENTITY "HTTP/1.1 %d %s\nContent-Length: %ld\nContent-Type: %s\n\n%s\n"

void setHeader(char* header, char h_buf[]);

int main(int argc, char* argv[]) {
	
	struct sockaddr_in addr; //sockaddr_in 구조체 변수 선언
	char header[1024] = { 0 };// 헤더 배열 선언, 0으로 초기화
	char h_buf[1024]; // HTML 버퍼 선언

	int client; // Client 변수 선언
	int proc; // Process  변수 선언
	int server = socket(AF_INET, SOCK_STREAM, 0); // Server socket 설정
	int addrlen = sizeof(addr); // addr 사이즈 만큼 addrlen 선언


	if (server == -1) { // Server가 설정이 안되 있으면 리턴
		// socket 함수는 실패시 -1, 성공시 0 이상을 반환
		printf("\nServer 설정 실패\n");
		return 0; // 리턴
	} // if-end
	
	
	
	// 구조체 변수 오버라이딩
	// htonl, htnos 네트워크 정렬 방식으로 재배열
	addr.sin_addr.s_addr = htonl(INADDR_ANY);// 32bit IPv4 주소
	//atoi() 문자열을 정수로
	addr.sin_port = htons(atoi(argv[1])); // 사용할 port 번호 argv 입력 받아 지정
	addr.sin_family = AF_INET; //IPv4 Internet protocol
	
	// bind()
	// Server Socket에 필요한 주소 정보 할당
	if (bind(server, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		// bind()
		// 할당 성공시 0, 실패시 -1 반환
		printf("\n서버 소켓에 정보 할당 실패\n");
		return 0;
	} // if-end
	
	
	// listen()
	// listen( 소켓 식별자 , 백로그큐 );
	// 연결 대기 0 , 실패 -1
	if (listen(server, 3) < 0) {
		printf("\n서버 대기하지 않음\n");
		return 0;
	} // if-end
	
	
	printf("서버 가동");
	
	while (1) { // 계속 반복 true
		// Server로 접속이 들어오면 Client 연결에 대한 새로운 소켓 생성
		// 연결이 성공하면 이 구조체를 채워서 되돌려 주게 되고
		// 이 구조체의 정보를 이용해서 연결된 클라이언트의 인터넷 정보 획득
		client = accept(server, (struct sockaddr*)&addr, (socklen_t*) & addrlen);

		if (client == -1) { // accept 함수는 에러시 -1 반환, 성공시 0보다 큰 값을 반환
			continue;
		} // if-end
		
		proc=fork(); // 프로세스 생성
		//fork()는 부모 프로세스에게는 자식 프로세스의 PID를 반환하고
		// 자식 프로세스에게는 0을 반환

		if (proc == 0) {
			close(server);
			setHeader(header, h_buf);
			write(client, header, strlen(header));
		} // if-end

		if (proc != 0) {
			close(client);
		} // if-end

		if (proc == -1) { // fork() 실패시 -1을 반환
			printf("프로세스 생성 실패");
		} // if-end
		
	} // while-end
} // main-end

void setHeader(char* header, char h_buf[]) {
	FILE* fp=fopen("http.html","r"); // 읽기 모드로 열어서 파일 포인터 설정
	fgets(h_buf,1024,fp); // 담을 변수, 담을 수, 파일 포인터
	fclose(fp); // 연 파일 닫기
	// 인자의 값을 출력하는 결과를 header에 저장
	sprintf(header, ENTITY, 200, "OK", strlen(h_buf), "text/html", h_buf);
} //setHeader-end

