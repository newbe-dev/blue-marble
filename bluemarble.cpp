// 21�� ~ 28�� (�÷��̾� ����ü) , 114 ~ 131��(���ε� ����ó��) , 464�� ~ 500��(���� �ʱ�ȭ �Լ�) : ������
// 30 ~ 36�� ( ���� ����ü ),  530 ~ 543�� (�÷��̾� ����ü �ʱ�ȭ) : ����� 
// ������: �輺�� 

#include <stdio.h> // ��� 
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <windows.h>

// �����ڵ� 
#define COLOR_RED      "\x1b[31m"
#define COLOR_GREEN    "\x1b[32m"
#define COLOR_YELLOW   "\x1b[33m"
#define COLOR_BLUE     "\x1b[34m"
#define COLOR_MAGENTA  "\x1b[35m"
#define COLOR_CYAN     "\x1b[36m"
#define COLOR_NONE    "\x1b[0m"

struct Player { // �÷��̾� ����ü 
	char name[50]; // �̸� 
	int num; // �÷��̾� ��ȣ(0, 1, 2, 3) 
	int assets; // ���� �ڻ� 
	int doubleCount; // ������ ���� Ƚ�� �Ǵ� ���ε� Ż�� ���� Ƚ�� 
	int location; // �÷��̾� ��ġ 
	int attractionCount; // ���ε� ���� ���� 
};

struct Zone { // ���� ����ü 
	char name[20]; // ���� �̸� 
	struct Player* owner = NULL; // ���� ���� ������ 
	int built; // ���� ���� ( 0: �̰���, 1: ����������, 2~4 ���� ���� ȣ��) 
	int toll[4]; // ����� �迭 ( 0: �⺻, 1~3:  �������ȣ��) 
	int cost[4]; // �Ǽ���� �迭 ( 0: ����, 1~3 �������ȣ��) 
};

void zone_init(struct Zone* map); // ���� �ʱ�ȭ�ϴ� �Լ�
void player_init(struct Player* players, int playerCount);
void insertZone(struct Zone* map, int num, char name[], int t0, int t1, int t2, int t3, int c0, int c1, int c2, int c3); // ������ map �迭�� �־��ִ� �Լ� 
void insertZone(struct Zone* map, int num, char name[]); // ������ map �迭�� �־��ִ� �Լ� 
void insertZone(struct Zone* map, int num, char name[], int t0, int c0); // ������ map �迭�� �־��ִ� �Լ� 
int pay_toll(struct Zone* curZone, struct Player* p, struct Zone* map); // ��붥���� ������� �Լ� 
void print_map(struct Player* players, struct Zone* map, int playerCount); // ��ü �� ��� �Լ� 
void interact(struct Zone* curZone, struct Player* p); // �Ǽ� ��ȣ�ۿ� �Լ� 
int total_toll(struct Zone* zone); // ��ü ����� ��� �Լ�
// curZone : ���� ���� ����ü ������, map: Zone ����ü ����ִ� �迭 


int main(void) {
	// ���� ���� 
	srand(time(NULL)); // ���� �õ尪 �ʱ�ȭ 
	int input; // ����� �Է� ���� 
	char c; // ����� �Է� ���� 
	int playerCount, LDice, RDice, turn = -1, bankruptcy = 0; // ���ʺ��� ������� �÷��̾� ��, ����/������ �ֻ���(L, R), �ڽ� ������ �÷��̾��� ��ȣ, �Ļ��� �÷��̾� ��(�������� ���� �˻翡 ����) 
	struct Player* players; // �÷��̾� ����ü �迭 
	struct Player* p; // ���� ������ �÷��̾� ����Ű�� ������ 
	struct Zone map[32];
	// �� ���� ����ִ� 2�����迭�� 
	// ���ε�: 8, �ø��Ȱ���: 16, ���迩��: 24, �����: 0 
	
	printf("�÷��̾� �ο� ���� �Է����ּ��� (2~4): ");
	scanf("%d", &playerCount);
	players = (Player*)malloc(sizeof(Player) * playerCount);
	if(players == NULL) {
		printf("������ �Ҵ� ����");
		exit(1);
	}
	// �ݾ׺й� & �÷��̾� �ʱ�ȭ 
	player_init(players, playerCount);
	//�̸�����
	for(int i = 0; i < playerCount; i++) {
		printf("�÷��̾� %d���� �̸��� �Է����ּ���: ", i+1);
		scanf("%s", &(players+i)->name);
	}
	//���� �ʱ�ȭ 
	zone_init(map);
	while(bankruptcy < playerCount - 1) { // �������� ���ǰ˻� ex) �÷��̾� ��: 4, �Ļ�: 3 -> ���� ���� false, while�� �������� 
		turn++; // ���� �Ѿ 
		turn %= playerCount; // ���� �Ѿ 
		system("cls");
		print_map(players, map, playerCount);
		p = (players+turn);
		if(p->assets < 0) continue;
		if(p->location == 24) { // ���迩�� ĭ�� �ִٸ� (Ư���̵� ����ó��) 
			while(true) {
				int input = -1;
				while (getchar() != '\n');
				printf("%s���� �����Դϴ�. �̵��ϱ� ���ϴ� ������ ��ȣ�� �Է����ּ���. (0~31): ", p->name);
				scanf("%d", &input);
				if(input >= 0 && input <= 31 && input != 24) {
					p->location = input;
					if(p->location <= 23) {
						printf("������� �����Ͽ� ���� 100������ ȹ���մϴ�.\n");
						p->assets += 100;
					}
					break;
				}
				else {
					printf("%s�߸��� �Է��Դϴ�.%s\n", COLOR_RED, COLOR_NONE);
				}
			}	
		}
		else { // ���迩�� ĭ�� �ƴ϶�� 
			printf("\n%s���� �����Դϴ�. �ƹ� Ű�� ���� �ֻ����� �����ּ���.\n", p->name);
			int key = getch();
			if(key == 27) { // ESC Ű ������ ��������(DEBUG) 
				free(players);
				exit(0);
			}
			LDice = rand()%6+1;
			RDice = rand()%6+1;
			if(p->location == 8) { // ���ε� 
				if(LDice == RDice) {
					printf("����! [%d] [%d]\n", LDice, RDice);
					printf("%s���� ���ε� Ż�⿡ �����߽��ϴ�!\n", p->name);
					p->doubleCount = 0;
					RDice += LDice; // ���� ������
					LDice = 0; // ���� ������ 
				}
				else {
					 printf("[%d] [%d]\n", LDice, RDice);
					if(p->doubleCount < 2) {
						p->doubleCount++;
						printf("���ε� Ż�⿡ �����߽��ϴ�.. (����� ��: %d)\n", p->doubleCount);
						Sleep(1000);
						continue;
					}
					printf("���ε��� Ż���մϴ�.\n");
					p->doubleCount = 0;
				}
				
			}
			else { // ���ε� �ƴ϶�� 
				if(LDice == RDice) {
					printf("����! [%d] [%d]\n", LDice, RDice);
				
					p->doubleCount++;
					if(p->doubleCount >= 3) { // ������ 
						printf("%s���� �ֻ����� �ʹ� ���� ���� ���ε��� ��ġ�˴ϴ�. �Ӹ��� ��������\n", p->name);
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
			printf("%s���� %dĭ ������ �����մϴ�..\n", p->name, LDice+RDice);
			p->location += (LDice+RDice); // �÷��̾� �̵� 
		}
		
		if(p->location >= 32) {
			printf("������� �����Ͽ� ���� 100������ ȹ���մϴ�.\n");
			p->assets += 100;
			p->location %= 32;
		}
		if(p->location == 0) { // ����� ���� 
			printf("������� �����߽��ϴ�. ���ʽ��� 100������ �� ȹ���մϴ�.\n");
			p->assets += 100;
		}
		else if(p->location == 8) { // ���ε� ���� 
			printf("���ε��� �����߽��ϴ�..\n");
			Sleep(1000);
			continue;
		}
		else if(p->location == 16) { // �ø��� ���� 
			printf("�ø����� ������ �ʾҴ�.\n");
		}
		else if(p->location == 24) { // ���迩�� ����
			printf("���迩�࿡ �����߽��ϴ�! ���� �Ͽ� ���ϴ� �������� �� �� �־��.\n");
		}
		else if(p->location == 30) { // ����û ���� 
			printf("����û�� �����߽��ϴ�. �ڻ��� 10%%�� ���� %d������ �峳�մϴ�.\n", (int)(p->assets * 0.1f));
			p->assets = (int)(p->assets * 0.9f);
		}
		else { // ������ (�Ϲ�����, ������) ���� 
			Zone* curZone = (map+p->location);
			if(curZone->owner != NULL && curZone->owner != (players+turn)) { // ��� ���̶�� 
				if(pay_toll(curZone, p, map) == 1) { // ����� ����, �Ļ��ϸ� ���� ���� TRUE 
					bankruptcy++; // �Ļ� 
					Sleep(1000);
					continue;
				}
			}
			else {
				printf("%s�� �����߽��ϴ�.\n", curZone->name);				
			}
			if(curZone->owner == NULL || curZone->owner == (players+turn)) { // ���ξ��� �� or �����̶�� 
				int totalToll = total_toll(curZone);
				if(curZone->cost[1] == 0) { //  ���������
					printf("%s(���� ���� ����: %s������%s ::  ����: %d����  ::  �����: %d���� )%s\n", COLOR_CYAN, COLOR_MAGENTA, COLOR_CYAN, curZone->cost[0], totalToll, COLOR_NONE);
				}
				else { // �������� �ƴ϶�� 
					char temp[30];
					switch(curZone->built) {
						case 0:
							strcpy(temp, "�̰�������");
							break;
						case 1:
							strcpy(temp, "����");
							break;
						case 2:
							strcpy(temp, "����");
							break;
						case 3:
							strcpy(temp, "����");
							break;
						case 4:
							strcpy(temp, "ȣ��");
							break;
						default:
							break;
					}
					printf("%s(���� ���� ����: %s%s%s :: ����: %d����  ::  ���� �Ǽ���: %d����  ::  ���� �Ǽ���: %d����  ::  ȣ�� �Ǽ���: %d����  ::  %s�����: %d����%s %s)%s\n", COLOR_CYAN, COLOR_MAGENTA, temp, COLOR_CYAN, curZone->cost[0], curZone->cost[1], curZone->cost[2], curZone->cost[3], COLOR_GREEN, totalToll, COLOR_NONE, COLOR_CYAN, COLOR_NONE);
				}
				interact(curZone, p); // �Ǽ� ��ȣ�ۿ� 
			}
		}
		Sleep(1000);
		if(LDice == RDice) turn--;
	}
	// ����� �˻� 
	int i;
	for(i = 0; i < playerCount; i++) {
		if(players[i].assets > 0) break;
	}
	printf("\n___________________\n���� ����! %s���� �¸�!", players[i]);
	free(players);
}

void print_map(struct Player* players, struct Zone* map, int playerCount) { //��ü �� ��� �Լ� (���������� ���ʴ�� ���) 
	// 16 ~ 24 ��� 
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������	%s�÷��̾�1(%s)#%s ���� �ڻ�: %d����, ���� ��ġ: %s\n", COLOR_RED, players[0].name, COLOR_NONE, players[0].assets, map[players[0].location].name);
	printf("��        ��        ��        ��        ��        ��        ��        ��        ��        ��	%s�÷��̾�2(%s)#%s ���� �ڻ�: %d����, ���� ��ġ: %s\n", COLOR_BLUE, players[1].name, COLOR_NONE, players[1].assets, map[players[1].location].name);
	//printf("�� �ø��� �� ������ �� Ǫ��   �� ������ ����Ʈ���æ���ũ�٦� ���׹� ��  �θ�  �����迩�ঢ\n");
	for(int i = 16; i <= 24; i++) {
		printf("��%8s", map[i].name);
	}
	if(playerCount >= 3)
		printf("��	%s�÷��̾�3(%s)#%s ���� �ڻ�: %d����, ���� ��ġ: %s\n", COLOR_YELLOW, players[2].name, COLOR_NONE, players[2].assets, map[players[2].location].name);
	else
		printf("��\n");
		
	for(int i = 16; i <= 24; i++) {
		printf("��");
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
		printf("��	%s�÷��̾�4(%s)#%s ���� �ڻ�: %d����, ���� ��ġ: %s\n", COLOR_GREEN, players[3].name, COLOR_NONE, players[3].assets, map[players[3].location].name);
	else
		printf("��\n");
	
	// 15, 25 ��� 
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������\n");
	printf("��        ��%62s��        ��\n", "");
	printf("��%8s��%62s��%8s��\n", map[15].name, "", map[25].name);
	
	printf("��");
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
	printf("  [%d]%s   ��%62s��  ", map[15].built, COLOR_NONE, "");
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
	printf("[%d]%s   ��\n", map[25].built, COLOR_NONE);	
	// 9 ~ 14, 25 ~ 31 ���
	for(int i = 14; i >= 9; i--) {
		printf("��������������������%62s��������������������\n", "");
		printf("��        ��%62s��        ��\n", "");
		printf("��%8s��%62s��%8s��\n", map[i].name, "", map[40-i].name);
		
		printf("��");
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
		printf("  [%d]%s   ��%62s��  ", map[i].built, COLOR_NONE, "");
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
		printf("[%d]%s   ��\n", map[40-i].built, COLOR_NONE);	
	}
	
	 // 0 ~ 8 ��� 
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������\n");
	printf("��        ��        ��        ��        ��        ��        ��        ��        ��        ��\n");
	//printf("�� �ø��� �� ������ �� Ǫ��   �� ������ ����Ʈ���æ���ũ�٦� ���׹� ��  �θ�  �����迩�ঢ\n");
	for(int i = 8; i >= 0; i--) {
		printf("��%8s", map[i].name);
	}
	printf("��\n");
	for(int i = 8; i >= 0; i--) {
		printf("��");
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
	printf("��\n");
	printf("��������������������������������������������������������������������������������������������������������������������������������������������������������������������\n\n");
	
}

int total_toll(struct Zone* zone) { // ��ü ����� ��ȯ�ϴ� �Լ� 
	int totalToll = 0;
	for(int i = 0; i < zone->built; i++) {
		totalToll += zone->toll[i];
	}
	if(zone->cost[1] == 0 && zone->owner != NULL) { // ������ ������ ���� ����� 2�辿 ��� 
		for(int i = 1; i < zone->owner->attractionCount; i++) { 
			totalToll *= 2;
		}
	}
	return totalToll;
}

int pay_toll(struct Zone* curZone, struct Player* p, struct Zone* map) { // ����� �����ϴ� �Լ� (�Ļ��ϸ� 1 ��ȯ �ƴϸ� 0 ��ȯ) 
	char c = ' ';
	int totalToll = total_toll(curZone);
	printf("%s���� ������ %s�� �����߽��ϴ�!\n", curZone->owner->name, curZone->name);
	curZone->owner->assets += totalToll;
	p->assets -= totalToll;
	printf("%s����� %d������ �����մϴ�.. (���� ���� �ڻ�: %d����) %s\n", COLOR_YELLOW, totalToll, p->assets, COLOR_NONE);
	
	if(p->assets < 0) { // �����ڻ� ���̳ʽ��� �Ļ� 
		printf("%s%s���� �Ļ��߽��ϴ�.%s\n", COLOR_RED, p->name, COLOR_NONE);
		for(int i = 0; i <= 31; i++) {
			if(map[i].owner == p) {
				map[i].built = 0;
				map[i].owner = NULL;
			}
		}
		return 1;
	}
	if(curZone->cost[1] == 0) return 0; // �������� �μ� �Ұ��� 
	
	int takeOver = 0; // �μ���� 
	for(int i = 0; i < curZone->built; i++) {
		takeOver += curZone->cost[i]*2;
	}
	if(p->assets >= takeOver) {
		while (getchar() != '\n');
		printf("%s��(��) �μ��Ͻðڽ��ϱ�? (�μ����: %d����) (�μ��Ϸ��� y / Y): ", curZone->name, takeOver);
		scanf("%c", &c);				
		if(c == 'Y' || c == 'y') {
			p->assets -= takeOver;
			curZone->owner = p;
			printf("%s�μ��� �����߽��ϴ�! (���� ���� �ڻ�: %d����) %s\n", COLOR_GREEN, p->assets, COLOR_NONE);
		}
	}
	return 0;
}

void interact(struct Zone* curZone, struct Player* p) {
	int input;
	while(true) {
		input = -2;
		if(curZone->cost[1] == 0) { // ��������� 
			while (getchar() != '\n');
			printf("������ �ൿ�� �������ּ���. (�� �����ϱ�: -1, ������ ����: 0): ");
			scanf("%d", &input);
			if(input == -1) break;
			if(input == 0) {
				if(input < curZone->built) {
					printf("%s�������� �̹� ���ŵǾ����ϴ�. %s\n", COLOR_RED, COLOR_NONE);
				}
				else {
					if(curZone->cost[0] > p->assets) {
						printf("%s���� �ڻ��� �����մϴ�. (�Ǽ� ���: %d����, ���� ���� �ڻ�: %d����) %s\n", COLOR_RED, curZone->cost[0], p->assets, COLOR_NONE);
					}
					else {
						p->assets -= curZone->cost[0];
						curZone->built = 1;
						curZone->owner = p;
						p->attractionCount += 1;
						printf("%s�Ǽ��� �����߽��ϴ�! (�Ǽ� ���: %d����, ���� ���� �ڻ�: %d����, ���� �����: %d����(������ ���ʽ� - �� ������ ��: %d��)%s\n", COLOR_GREEN, curZone->cost[0], p->assets, 40*p->attractionCount, p->attractionCount, COLOR_NONE);
					}
				}	
			}
			else {
				printf("%s�߸��� �Է��Դϴ�.%s\n", COLOR_RED, COLOR_NONE);
			}
		}
		else { // �Ϲ� �����̶��  
			while (getchar() != '\n');
			printf("������ �ൿ�� �������ּ���. (�� �����ϱ�: -1, �� ����: 0, ���� �Ǽ�: 1, ���� �Ǽ�: 2, ȣ�� �Ǽ�: 3): ");
			scanf("%d", &input);
			if(input == -1) break;
			if(input >= 0 && input <= 3) {
				if(input < curZone->built) {
					printf("%s�̹� �Ǽ��� �ܰ��Դϴ�. %s\n", COLOR_RED, COLOR_NONE);
				}
				else {
					int totalCost = 0;
					
					for(int i = curZone->built; i<=input; i++) {
						totalCost += curZone->cost[i];
					}
					if(totalCost > p->assets) {
						printf("%s���� �ڻ��� �����մϴ�. (�Ǽ� ���: %d����, ���� ���� �ڻ�: %d����) %s\n", COLOR_RED, totalCost, p->assets, COLOR_NONE);
					}
					else {
						p->assets -= totalCost;
						curZone->built = input+1;
						curZone->owner = p;
						printf("%s�Ǽ��� �����߽��ϴ�! (�Ǽ� ���: %d����, ���� ���� �ڻ�: %d����, ���� �����: %d����)%s\n", COLOR_GREEN, totalCost, p->assets, total_toll(curZone), COLOR_NONE);
					}
				}	
			}
			else {
				printf("%s�߸��� �Է��Դϴ�.%s\n", COLOR_RED, COLOR_NONE);
			}
		}
	}
}

void zone_init(struct Zone* map) { // ���� �ʱ�ȭ �Լ� 
	insertZone(map, 0, "�����");
	insertZone(map, 1, "����", 1, 2, 9, 25, 5, 5, 15, 25); // �⺻����� ���� ���� ȣ�� ���� ����Ǽ� �����Ǽ� ȣ�ڰǼ�
	insertZone(map, 2, "Ÿ�̺���", 1, 2, 9, 25, 5, 5, 15, 25);
	insertZone(map, 3, "����¡", 2, 2, 18, 35, 8, 5, 15, 25);
	insertZone(map, 4, "���Ҷ�", 2, 2, 18, 35, 8, 5, 15, 25);
	insertZone(map, 5, "����", 40, 20);
	insertZone(map, 6, "�ι���", 3, 3, 27, 45, 10, 5, 15, 25);
	insertZone(map, 7,"ī�̷�", 3, 3, 27, 45, 10, 5, 15, 25);
	insertZone(map, 8, "���ε�");
	//
	insertZone(map, 9, "�߸�", 4, 6, 45, 75, 16, 10, 30, 50);
	insertZone(map, 10, "����", 4, 6, 45, 75, 16, 10, 30, 50);
	insertZone(map, 11, "�õ��", 5, 7, 50, 85, 18, 10, 30, 50);
	insertZone(map, 12, "�����ϰ�", 5, 7, 50, 85, 18, 10, 30, 50);
	insertZone(map, 13, "�Ͽ���", 40, 20);
	insertZone(map, 14, "���Ŀ��", 6, 8, 55, 100, 20, 10, 30, 50);
	insertZone(map, 15, "����", 6, 8, 55, 100, 20, 10, 30, 50);
	insertZone(map, 16, "�ø���");
	//
	insertZone(map, 17, "������", 40, 20);
	insertZone(map, 18, "Ǫ��", 7, 10, 70, 100, 22, 15, 45, 75);
	insertZone(map, 19, "������", 7, 10, 70, 100, 22, 15, 45, 75);
	insertZone(map, 20, "��Ʈ����", 8, 12, 80, 110, 24, 15, 45, 75);
	insertZone(map, 21, "��ũ��", 8, 12, 80, 110, 24, 15, 45, 75);
	insertZone(map, 22, "���׹�", 9, 14, 90, 120, 26, 15, 45, 75);
	insertZone(map, 23, "�θ�", 9, 14, 90, 120, 26, 15, 45, 75);
	insertZone(map, 24, "���迩��");
	//
	insertZone(map, 25, "Ÿ��Ƽ", 40, 20);
	insertZone(map, 26, "����", 10, 15, 100, 130, 30, 20, 60, 100);
	insertZone(map, 27, "�ĸ�", 10, 15, 100, 130, 30, 20, 60, 100);
	insertZone(map, 28, "���帮��", 11, 18, 110, 140, 35, 20, 60, 100);
	insertZone(map, 29, "����", 11, 18, 110, 140, 35, 20, 60, 100);
	insertZone(map, 30, "����û");
	insertZone(map, 31, "����", 15, 20, 130, 200, 50, 30, 80, 120);
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

