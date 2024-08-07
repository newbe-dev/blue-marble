#include <stdio.h> // 헤더 
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <windows.h>

// 색상코드 
#define COLOR_RED      "\x1b[31m"
#define COLOR_GREEN    "\x1b[32m"
#define COLOR_YELLOW   "\x1b[33m"
#define COLOR_BLUE     "\x1b[34m"
#define COLOR_MAGENTA  "\x1b[35m"
#define COLOR_CYAN     "\x1b[36m"
#define COLOR_NONE    "\x1b[0m"

struct Player { // 플레이어 구조체 
	char name[50]; // 이름 
	int num; // 플레이어 번호(0, 1, 2, 3) 
	int assets; // 보유 자산 
	int doubleCount; // 더블이 나온 횟수 또는 무인도 탈출 못한 횟수 
	int location; // 플레이어 위치 
	int attractionCount; // 무인도 보유 개수 
};

struct Zone { // 지역 구조체 
	char name[20]; // 지역 이름 
	struct Player* owner = NULL; // 지역 주인 포인터 
	int built; // 지역 상태 ( 0: 미개발, 1: 토지만구입, 2~4 별장 빌딩 호텔) 
	int toll[4]; // 통행료 배열 ( 0: 기본, 1~3:  별장빌딩호텔) 
	int cost[4]; // 건설비용 배열 ( 0: 땅값, 1~3 별장빌딩호텔) 
};

void zone_init(struct Zone* map); // 지역 초기화하는 함수
void player_init(struct Player* players, int playerCount);
void insertZone(struct Zone* map, int num, char name[], int t0, int t1, int t2, int t3, int c0, int c1, int c2, int c3); // 지역을 map 배열에 넣어주는 함수 
void insertZone(struct Zone* map, int num, char name[]); // 지역을 map 배열에 넣어주는 함수 
void insertZone(struct Zone* map, int num, char name[], int t0, int c0); // 지역을 map 배열에 넣어주는 함수 
int pay_toll(struct Zone* curZone, struct Player* p, struct Zone* map); // 상대땅에서 비용지불 함수 
void print_map(struct Player* players, struct Zone* map, int playerCount); // 전체 맵 출력 함수 
void interact(struct Zone* curZone, struct Player* p); // 건설 상호작용 함수 
int total_toll(struct Zone* zone); // 전체 통행료 계산 함수
// curZone : 현재 지역 구조체 포인터, map: Zone 구조체 담고있는 배열 


int main(void) {
	// 변수 선언 
	srand(time(NULL)); // 랜덤 시드값 초기화 
	int input; // 사용자 입력 변수 
	char c; // 사용자 입력 변수 
	int playerCount, LDice, RDice, turn = -1, bankruptcy = 0; // 왼쪽부터 순서대로 플레이어 수, 왼쪽/오른쪽 주사위(L, R), 자신 차례인 플레이어의 번호, 파산한 플레이어 수(게임종료 조건 검사에 쓰임) 
	struct Player* players; // 플레이어 구조체 배열 
	struct Player* p; // 현재 차례인 플레이어 가리키는 포인터 
	struct Zone map[32];
	// 땅 정보 담고있는 2차원배열임 
	// 무인도: 8, 올림픽개최: 16, 세계여행: 24, 출발지: 0 
	
	printf("플레이어 인원 수를 입력해주세요 (2~4): ");
	scanf("%d", &playerCount);
	players = (Player*)malloc(sizeof(Player) * playerCount);
	if(players == NULL) {
		printf("포인터 할당 실패");
		exit(1);
	}
	// 금액분배 & 플레이어 초기화 
	player_init(players, playerCount);
	//이름설정
	for(int i = 0; i < playerCount; i++) {
		printf("플레이어 %d님의 이름을 입력해주세요: ", i+1);
		scanf("%s", &(players+i)->name);
	}
	//지역 초기화 
	zone_init(map);
	while(bankruptcy < playerCount - 1) { // 게임종료 조건검사 ex) 플레이어 수: 4, 파산: 3 -> 안의 조건 false, while문 빠져나옴 
		turn++; // 차례 넘어감 
		turn %= playerCount; // 차례 넘어감 
		system("cls");
		print_map(players, map, playerCount);
		p = (players+turn);
		if(p->assets < 0) continue;
		if(p->location == 24) { // 세계여행 칸에 있다면 (특수이동 예외처리) 
			while(true) {
				int input = -1;
				while (getchar() != '\n');
				printf("%s님의 차례입니다. 이동하기 원하는 지역의 번호를 입력해주세요. (0~31): ", p->name);
				scanf("%d", &input);
				if(input >= 0 && input <= 31 && input != 24) {
					p->location = input;
					if(p->location <= 23) {
						printf("출발지를 경유하여 월급 100만원을 획득합니다.\n");
						p->assets += 100;
					}
					break;
				}
				else {
					printf("%s잘못된 입력입니다.%s\n", COLOR_RED, COLOR_NONE);
				}
			}	
		}
		else { // 세계여행 칸이 아니라면 
			printf("\n%s님의 차례입니다. 아무 키나 눌러 주사위를 굴려주세요.\n", p->name);
			int key = getch();
			if(key == 27) { // ESC 키 누르면 강제종료(DEBUG) 
				free(players);
				exit(0);
			}
			LDice = rand()%6+1;
			RDice = rand()%6+1;
			if(p->location == 8) { // 무인도 
				if(LDice == RDice) {
					printf("더블! [%d] [%d]\n", LDice, RDice);
					printf("%s님이 무인도 탈출에 성공했습니다!\n", p->name);
					p->doubleCount = 0;
					RDice += LDice; // 더블 방지용
					LDice = 0; // 더블 방지용 
				}
				else {
					 printf("[%d] [%d]\n", LDice, RDice);
					if(p->doubleCount < 2) {
						p->doubleCount++;
						printf("무인도 탈출에 실패했습니다.. (경과한 턴: %d)\n", p->doubleCount);
						Sleep(1000);
						continue;
					}
					printf("무인도를 탈출합니다.\n");
					p->doubleCount = 0;
				}
				
			}
			else { // 무인도 아니라면 
				if(LDice == RDice) {
					printf("더블! [%d] [%d]\n", LDice, RDice);
				
					p->doubleCount++;
					if(p->doubleCount >= 3) { // 더블세번 
						printf("%s님이 주사위를 너무 많이 굴려 무인도로 납치됩니다. 머리좀 식히세요\n", p->name);
						p->location = 8;
						p->doubleCount = 0;
						Sleep(1000);
						continue;
					}
				}
				else{
					printf("[%d] [%d]\n", LDice, RDice);
					p->doubleCount = 0;	
				}
			}
			printf("%s님이 %d칸 앞으로 전진합니다..\n", p->name, LDice+RDice);
			p->location += (LDice+RDice); // 플레이어 이동 
		}
		
		if(p->location >= 32) {
			printf("출발지를 경유하여 월급 100만원을 획득합니다.\n");
			p->assets += 100;
			p->location %= 32;
		}
		if(p->location == 0) { // 출발지 도착 
			printf("출발지에 도착했습니다. 보너스로 100만원을 더 획득합니다.\n");
			p->assets += 100;
		}
		else if(p->location == 8) { // 무인도 도착 
			printf("무인도에 도착했습니다..\n");
			Sleep(1000);
			continue;
		}
		else if(p->location == 16) { // 올림픽 도착 
			printf("올림픽은 만들지 않았다.\n");
		}
		else if(p->location == 24) { // 세계여행 도착
			printf("세계여행에 도착했습니다! 다음 턴에 원하는 지역으로 갈 수 있어요.\n");
		}
		else if(p->location == 30) { // 국세청 도착 
			printf("국세청에 도착했습니다. 자산의 10%%인 세금 %d만원을 헌납합니다.\n", (int)(p->assets * 0.1f));
			p->assets = (int)(p->assets * 0.9f);
		}
		else { // 나머지 (일반지역, 관광지) 도착 
			Zone* curZone = (map+p->location);
			if(curZone->owner != NULL && curZone->owner != (players+turn)) { // 상대 땅이라면 
				if(pay_toll(curZone, p, map) == 1) { // 통행료 지불, 파산하면 안의 조건 TRUE 
					bankruptcy++; // 파산 
					Sleep(1000);
					continue;
				}
			}
			else {
				printf("%s에 도착했습니다.\n", curZone->name);				
			}
			if(curZone->owner == NULL || curZone->owner == (players+turn)) { // 주인없는 땅 or 내땅이라면 
				int totalToll = total_toll(curZone);
				if(curZone->cost[1] == 0) { //  관광지라면
					printf("%s(현재 지역 정보: %s관광지%s ::  땅값: %d만원  ::  통행료: %d만원 )%s\n", COLOR_CYAN, COLOR_MAGENTA, COLOR_CYAN, curZone->cost[0], totalToll, COLOR_NONE);
				}
				else { // 관광지가 아니라면 
					char temp[30];
					switch(curZone->built) {
						case 0:
							strcpy(temp, "미개발지대");
							break;
						case 1:
							strcpy(temp, "부지");
							break;
						case 2:
							strcpy(temp, "별장");
							break;
						case 3:
							strcpy(temp, "빌딩");
							break;
						case 4:
							strcpy(temp, "호텔");
							break;
						default:
							break;
					}
					printf("%s(현재 지역 정보: %s%s%s :: 땅값: %d만원  ::  별장 건설비: %d만원  ::  빌딩 건설비: %d만원  ::  호텔 건설비: %d만원  ::  %s통행료: %d만원%s %s)%s\n", COLOR_CYAN, COLOR_MAGENTA, temp, COLOR_CYAN, curZone->cost[0], curZone->cost[1], curZone->cost[2], curZone->cost[3], COLOR_GREEN, totalToll, COLOR_NONE, COLOR_CYAN, COLOR_NONE);
				}
				interact(curZone, p); // 건설 상호작용 
			}
		}
		Sleep(1000);
		if(LDice == RDice) turn--;
	}
	// 우승자 검사 
	int i;
	for(i = 0; i < playerCount; i++) {
		if(players[i].assets > 0) break;
	}
	printf("\n___________________\n게임 종료! %s님의 승리!", players[i]);
	free(players);
}

void print_map(struct Player* players, struct Zone* map, int playerCount) { //전체 맵 출력 함수 (위에서부터 차례대로 출력) 
	// 16 ~ 24 출력 
	printf("┌────────┬────────┬────────┬────────┬────────┬────────┬────────┬────────┬────────┐	%s플레이어1(%s)#%s 보유 자산: %d만원, 현재 위치: %s\n", COLOR_RED, players[0].name, COLOR_NONE, players[0].assets, map[players[0].location].name);
	printf("│        │        │        │        │        │        │        │        │        │	%s플레이어2(%s)#%s 보유 자산: %d만원, 현재 위치: %s\n", COLOR_BLUE, players[1].name, COLOR_NONE, players[1].assets, map[players[1].location].name);
	//printf("│ 올림픽 │ 프라하 │ 푸켓   │ 베를린 │몬트리올│모스크바│ 제네바 │  로마  │세계여행│\n");
	for(int i = 16; i <= 24; i++) {
		printf("│%8s", map[i].name);
	}
	if(playerCount >= 3)
		printf("│	%s플레이어3(%s)#%s 보유 자산: %d만원, 현재 위치: %s\n", COLOR_YELLOW, players[2].name, COLOR_NONE, players[2].assets, map[players[2].location].name);
	else
		printf("│\n");
		
	for(int i = 16; i <= 24; i++) {
		printf("│");
		if(map[i].owner == NULL) {
			printf("%s", COLOR_NONE);
		}
		else if (map[i].owner->num == 0)
			printf("%s", COLOR_RED);
		else if (map[i].owner->num == 1)
			printf("%s", COLOR_BLUE);
		else if (map[i].owner->num == 2)
			printf("%s", COLOR_YELLOW);
		else if (map[i].owner->num == 3)
			printf("%s", COLOR_GREEN);
		printf("  [%d]%s   ", map[i].built, COLOR_NONE);
	}
	if(playerCount >= 4)
		printf("│	%s플레이어4(%s)#%s 보유 자산: %d만원, 현재 위치: %s\n", COLOR_GREEN, players[3].name, COLOR_NONE, players[3].assets, map[players[3].location].name);
	else
		printf("│\n");
	
	// 15, 25 출력 
	printf("├────────┼────────┴────────┴────────┴────────┴────────┴────────┴────────┼────────┤\n");
	printf("│        │%62s│        │\n", "");
	printf("│%8s│%62s│%8s│\n", map[15].name, "", map[25].name);
	
	printf("│");
	if(map[15].owner == NULL) {
		printf("%s", COLOR_NONE);
	}
	else if (map[15].owner->num == 0)
		printf("%s", COLOR_RED);
	else if (map[15].owner->num == 1)
		printf("%s", COLOR_BLUE);
	else if (map[15].owner->num == 2)
		printf("%s", COLOR_YELLOW);
	else if (map[15].owner->num == 3)
		printf("%s", COLOR_GREEN);
	printf("  [%d]%s   │%62s│  ", map[15].built, COLOR_NONE, "");
	if(map[25].owner == NULL) {
		printf("%s", COLOR_NONE);
	}
	else if (map[25].owner->num == 0)
		printf("%s", COLOR_RED);
	else if (map[25].owner->num == 1)
		printf("%s", COLOR_BLUE);
	else if (map[25].owner->num == 2)
		printf("%s", COLOR_YELLOW);
	else if (map[25].owner->num == 3)
		printf("%s", COLOR_GREEN);
	printf("[%d]%s   │\n", map[25].built, COLOR_NONE);	
	// 9 ~ 14, 25 ~ 31 출력
	for(int i = 14; i >= 9; i--) {
		printf("├────────┤%62s├────────┤\n", "");
		printf("│        │%62s│        │\n", "");
		printf("│%8s│%62s│%8s│\n", map[i].name, "", map[40-i].name);
		
		printf("│");
		if(map[i].owner == NULL) {
			printf("%s", COLOR_NONE);
		}
		else if (map[i].owner->num == 0)
			printf("%s", COLOR_RED);
		else if (map[i].owner->num == 1)
			printf("%s", COLOR_BLUE);
		else if (map[i].owner->num == 2)
			printf("%s", COLOR_YELLOW);
		else if (map[i].owner->num == 3)
			printf("%s", COLOR_GREEN);
		printf("  [%d]%s   │%62s│  ", map[i].built, COLOR_NONE, "");
		if(map[40-i].owner == NULL) {
			printf("%s", COLOR_NONE);
		}
		else if (map[40-i].owner->num == 0)
			printf("%s", COLOR_RED);
		else if (map[40-i].owner->num == 1)
			printf("%s", COLOR_BLUE);
		else if (map[40-i].owner->num == 2)
			printf("%s", COLOR_YELLOW);
		else if (map[40-i].owner->num == 3)
			printf("%s", COLOR_GREEN);
		printf("[%d]%s   │\n", map[40-i].built, COLOR_NONE);	
	}
	
	 // 0 ~ 8 출력 
	printf("├────────┼────────┬────────┬────────┬────────┬────────┬────────┬────────┼────────┤\n");
	printf("│        │        │        │        │        │        │        │        │        │\n");
	//printf("│ 올림픽 │ 프라하 │ 푸켓   │ 베를린 │몬트리올│모스크바│ 제네바 │  로마  │세계여행│\n");
	for(int i = 8; i >= 0; i--) {
		printf("│%8s", map[i].name);
	}
	printf("│\n");
	for(int i = 8; i >= 0; i--) {
		printf("│");
		if(map[i].owner == NULL) {
			printf("%s", COLOR_NONE);
		}
		else if (map[i].owner->num == 0)
			printf("%s", COLOR_RED);
		else if (map[i].owner->num == 1)
			printf("%s", COLOR_BLUE);
		else if (map[i].owner->num == 2)
			printf("%s", COLOR_YELLOW);
		else if (map[i].owner->num == 3)
			printf("%s", COLOR_GREEN);
		printf("  [%d]%s   ", map[i].built, COLOR_NONE);
	}
	printf("│\n");
	printf("└────────┴────────┴────────┴────────┴────────┴────────┴────────┴────────┴────────┘\n\n");
	
}

int total_toll(struct Zone* zone) { // 전체 통행료 반환하는 함수 
	int totalToll = 0;
	for(int i = 0; i < zone->built; i++) {
		totalToll += zone->toll[i];
	}
	if(zone->cost[1] == 0 && zone->owner != NULL) { // 관광지 개수에 따라 통행료 2배씩 상승 
		for(int i = 1; i < zone->owner->attractionCount; i++) { 
			totalToll *= 2;
		}
	}
	return totalToll;
}

int pay_toll(struct Zone* curZone, struct Player* p, struct Zone* map) { // 통행료 지불하는 함수 (파산하면 1 반환 아니면 0 반환) 
	char c = ' ';
	int totalToll = total_toll(curZone);
	printf("%s님의 소유인 %s에 도착했습니다!\n", curZone->owner->name, curZone->name);
	curZone->owner->assets += totalToll;
	p->assets -= totalToll;
	printf("%s통행료 %d만원을 지불합니다.. (현재 보유 자산: %d만원) %s\n", COLOR_YELLOW, totalToll, p->assets, COLOR_NONE);
	
	if(p->assets < 0) { // 보유자산 마이너스면 파산 
		printf("%s%s님이 파산했습니다.%s\n", COLOR_RED, p->name, COLOR_NONE);
		for(int i = 0; i <= 31; i++) {
			if(map[i].owner == p) {
				map[i].built = 0;
				map[i].owner = NULL;
			}
		}
		return 1;
	}
	if(curZone->cost[1] == 0) return 0; // 관광지는 인수 불가능 
	
	int takeOver = 0; // 인수비용 
	for(int i = 0; i < curZone->built; i++) {
		takeOver += curZone->cost[i]*2;
	}
	if(p->assets >= takeOver) {
		while (getchar() != '\n');
		printf("%s을(를) 인수하시겠습니까? (인수비용: %d만원) (인수하려면 y / Y): ", curZone->name, takeOver);
		scanf("%c", &c);				
		if(c == 'Y' || c == 'y') {
			p->assets -= takeOver;
			curZone->owner = p;
			printf("%s인수에 성공했습니다! (현재 보유 자산: %d만원) %s\n", COLOR_GREEN, p->assets, COLOR_NONE);
		}
	}
	return 0;
}

void interact(struct Zone* curZone, struct Player* p) {
	int input;
	while(true) {
		input = -2;
		if(curZone->cost[1] == 0) { // 관광지라면 
			while (getchar() != '\n');
			printf("진행할 행동을 선택해주세요. (턴 종료하기: -1, 관광지 구입: 0): ");
			scanf("%d", &input);
			if(input == -1) break;
			if(input == 0) {
				if(input < curZone->built) {
					printf("%s관광지가 이미 구매되었습니다. %s\n", COLOR_RED, COLOR_NONE);
				}
				else {
					if(curZone->cost[0] > p->assets) {
						printf("%s보유 자산이 부족합니다. (건설 비용: %d만원, 현재 보유 자산: %d만원) %s\n", COLOR_RED, curZone->cost[0], p->assets, COLOR_NONE);
					}
					else {
						p->assets -= curZone->cost[0];
						curZone->built = 1;
						curZone->owner = p;
						p->attractionCount += 1;
						printf("%s건설에 성공했습니다! (건설 비용: %d만원, 현재 보유 자산: %d만원, 현재 통행료: %d만원(관광지 보너스 - 내 관광지 수: %d개)%s\n", COLOR_GREEN, curZone->cost[0], p->assets, 40*p->attractionCount, p->attractionCount, COLOR_NONE);
					}
				}	
			}
			else {
				printf("%s잘못된 입력입니다.%s\n", COLOR_RED, COLOR_NONE);
			}
		}
		else { // 일반 지역이라면  
			while (getchar() != '\n');
			printf("진행할 행동을 선택해주세요. (턴 종료하기: -1, 땅 구입: 0, 별장 건설: 1, 빌딩 건설: 2, 호텔 건설: 3): ");
			scanf("%d", &input);
			if(input == -1) break;
			if(input >= 0 && input <= 3) {
				if(input < curZone->built) {
					printf("%s이미 건설된 단계입니다. %s\n", COLOR_RED, COLOR_NONE);
				}
				else {
					int totalCost = 0;
					
					for(int i = curZone->built; i<=input; i++) {
						totalCost += curZone->cost[i];
					}
					if(totalCost > p->assets) {
						printf("%s보유 자산이 부족합니다. (건설 비용: %d만원, 현재 보유 자산: %d만원) %s\n", COLOR_RED, totalCost, p->assets, COLOR_NONE);
					}
					else {
						p->assets -= totalCost;
						curZone->built = input+1;
						curZone->owner = p;
						printf("%s건설에 성공했습니다! (건설 비용: %d만원, 현재 보유 자산: %d만원, 현재 통행료: %d만원)%s\n", COLOR_GREEN, totalCost, p->assets, total_toll(curZone), COLOR_NONE);
					}
				}	
			}
			else {
				printf("%s잘못된 입력입니다.%s\n", COLOR_RED, COLOR_NONE);
			}
		}
	}
}

void zone_init(struct Zone* map) { // 지역 초기화 함수 
	insertZone(map, 0, "출발지");
	insertZone(map, 1, "방콕", 1, 2, 9, 25, 5, 5, 15, 25); // 기본통행료 별장 빌딩 호텔 땅값 별장건설 빌딩건설 호텔건설
	insertZone(map, 2, "타이베이", 1, 2, 9, 25, 5, 5, 15, 25);
	insertZone(map, 3, "베이징", 2, 2, 18, 35, 8, 5, 15, 25);
	insertZone(map, 4, "마닐라", 2, 2, 18, 35, 8, 5, 15, 25);
	insertZone(map, 5, "독도", 40, 20);
	insertZone(map, 6, "두바이", 3, 3, 27, 45, 10, 5, 15, 25);
	insertZone(map, 7,"카이로", 3, 3, 27, 45, 10, 5, 15, 25);
	insertZone(map, 8, "무인도");
	//
	insertZone(map, 9, "발리", 4, 6, 45, 75, 16, 10, 30, 50);
	insertZone(map, 10, "도쿄", 4, 6, 45, 75, 16, 10, 30, 50);
	insertZone(map, 11, "시드니", 5, 7, 50, 85, 18, 10, 30, 50);
	insertZone(map, 12, "코펜하겐", 5, 7, 50, 85, 18, 10, 30, 50);
	insertZone(map, 13, "하와이", 40, 20);
	insertZone(map, 14, "상파울로", 6, 8, 55, 100, 20, 10, 30, 50);
	insertZone(map, 15, "퀘백", 6, 8, 55, 100, 20, 10, 30, 50);
	insertZone(map, 16, "올림픽");
	//
	insertZone(map, 17, "프라하", 40, 20);
	insertZone(map, 18, "푸켓", 7, 10, 70, 100, 22, 15, 45, 75);
	insertZone(map, 19, "베를린", 7, 10, 70, 100, 22, 15, 45, 75);
	insertZone(map, 20, "몬트리올", 8, 12, 80, 110, 24, 15, 45, 75);
	insertZone(map, 21, "모스크바", 8, 12, 80, 110, 24, 15, 45, 75);
	insertZone(map, 22, "제네바", 9, 14, 90, 120, 26, 15, 45, 75);
	insertZone(map, 23, "로마", 9, 14, 90, 120, 26, 15, 45, 75);
	insertZone(map, 24, "세계여행");
	//
	insertZone(map, 25, "타이티", 40, 20);
	insertZone(map, 26, "런던", 10, 15, 100, 130, 30, 20, 60, 100);
	insertZone(map, 27, "파리", 10, 15, 100, 130, 30, 20, 60, 100);
	insertZone(map, 28, "마드리드", 11, 18, 110, 140, 35, 20, 60, 100);
	insertZone(map, 29, "뉴욕", 11, 18, 110, 140, 35, 20, 60, 100);
	insertZone(map, 30, "국세청");
	insertZone(map, 31, "서울", 15, 20, 130, 200, 50, 30, 80, 120);
}

void insertZone(struct Zone* map, int num, char name[], int t0, int t1, int t2, int t3, int c0, int c1, int c2, int c3) {
	strcpy(map[num].name, name);
	map[num].toll[0] = t0;
	map[num].toll[1] = t1;
	map[num].toll[2] = t2;
	map[num].toll[3] = t3;
	map[num].cost[0] = c0;
	map[num].cost[1] = c1;
	map[num].cost[2] = c2;
	map[num].cost[3] = c3;
	map[num].built = 0;
}
void insertZone(struct Zone* map, int num, char name[]) {
	strcpy(map[num].name, name);
	map[num].built = 0;
}
void insertZone(struct Zone* map, int num, char name[], int t0, int c0) {
	strcpy(map[num].name, name);
	map[num].toll[0] = t0;
	map[num].toll[1] = 0;
	map[num].toll[2] = 0;
	map[num].toll[3] = 0;
	map[num].cost[0] = c0;
	map[num].cost[1] = 0;
	map[num].cost[2] = 0;
	map[num].cost[3] = 0;
	map[num].built = 0;
}
void player_init(struct Player* players, int playerCount) {
	int foundation = 0;
	if(playerCount >= 3)
		foundation = 300;
	else 
		foundation = 600;
	for(int i = 0; i < playerCount; i++) {
		players[i].assets = foundation;
		players[i].num = i;
		players[i].location = 0;
		players[i].doubleCount = 0;
		players[i].attractionCount = 0;
	}
}

