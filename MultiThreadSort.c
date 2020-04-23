#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#define SET 100000000
#define T_SIZE 11

char *data;
char *sort;
FILE *readstream, *writestream;

char* createString() {
	char *string = (char*)malloc(sizeof(char) * T_SIZE);
	int ranchar;
	int i;

	for (i = 0; i < 10; i++) {
		ranchar = ((int)time(NULL) + rand()) % 36;
		if (ranchar < 10)
			string[i] = (char)(ranchar + 48);
		else
			string[i] = (char)(ranchar + 55);
	}
	string[10] = ' ';
	return string;
}
void createData() {

	FILE *datastream;
	int i;
	char* string;
	string = (char*)malloc(sizeof(char) * T_SIZE);
	fopen_s(&datastream, "C:\\DATA\\data1000.txt", "w");
	if (datastream == NULL)
		printf("입력파일 오류");

	for (i = 0; i < SET; i++) {
		string = createString();
		fwrite(string, sizeof(char), T_SIZE, datastream);
	}
	fclose(datastream);
	free(string);
}
void swap(char *a, char *b) {
	char ch[11];
	memcpy(ch, a, 10);
	memcpy(a, b, 10);
	memcpy(b, ch, 10);
}

int compare(int a, int b) {
	int i;
	for (i = 0; i < 10; i++) {
		if (data[a + i] > data[b + i])
			return 1;
		else if (data[a + i] < data[b + i])
			return -1;
	}
	return 0;
}
int compare2(int a, int b) {
	int i;
	for (i = 0; i < 10; i++) {
		if (sort[a + i] > sort[b + i])
			return 1;
		else if (sort[a + i] < sort[b + i])
			return -1;
	}
	return 0;
}

int partition(int low, int high, int* lp)
{
	int j = low + T_SIZE;
	int g = high - T_SIZE, k = low + T_SIZE, p = low, q = high;
	if (compare(low, high)==1)
		swap(&data[low], &data[high]);

	while (k <= g) {
		if (compare(k,p)==-1) {
			swap(&data[k], &data[j]);
			j += T_SIZE;
		}
		else if (compare(k, q) != -1) {
			while (compare(g, q) == 1 && k < g)
				g -= T_SIZE;
			swap(&data[k], &data[g]);
			g -= T_SIZE;
			if (compare(k, p)==-1) {
				swap(&data[k], &data[j]);
				j += T_SIZE;
			}
		}
		k += T_SIZE;
	}
	j -= T_SIZE;
	g += T_SIZE;
	swap(&data[low], &data[j]);
	swap(&data[high], &data[g]);
	*lp = j;
	return g;
}
void dualpivotquicksort(int low, int high)
{
	if (low < high) {
		int lp, rp;
		rp = partition(low, high, &lp);
		dualpivotquicksort(low, lp - T_SIZE);
		dualpivotquicksort(lp + T_SIZE, rp - T_SIZE);
		dualpivotquicksort(rp + T_SIZE, high);
	}
}

void merge1(int p, int q, int r) {
	int i = p, j = q + 1, k = p;
	while (i <= q && j <= r) {
		if (compare(i*T_SIZE, j*T_SIZE) == -1) memcpy(sort + (k++) * T_SIZE, data + (i++) * T_SIZE, 11);
		else memcpy(sort + (k++) * T_SIZE, data + (j++) * T_SIZE, 11);
	}
	while (i <= q) memcpy(sort + (k++) * T_SIZE, data + (i++) * T_SIZE, 11);
	while (j <= r) memcpy(sort + (k++) * T_SIZE, data + (j++) * T_SIZE, 11);
}

void merge2(int p, int q, int r) {
	int i = p, j = q + 1, k = p;
	while (i <= q && j <= r) {
		if (compare2(i*T_SIZE, j*T_SIZE) == -1) memcpy(data + (k++) * T_SIZE, sort + (i++) * T_SIZE, 11);
		else memcpy(data + (k++) * T_SIZE, sort + (j++) * T_SIZE, 11);
	}
	while (i <= q) memcpy(data + (k++) * T_SIZE, sort + (i++) * T_SIZE, 11);
	while (j <= r) memcpy(data + (k++) * T_SIZE, sort + (j++) * T_SIZE, 11);
}

DWORD WINAPI hd_th0(LPVOID lpVoid) {
	dualpivotquicksort(0, (SET / 4 - 1) * T_SIZE);
	return 0;
}
DWORD WINAPI hd_th1(LPVOID lpVoid) {
	dualpivotquicksort((SET / 4 * T_SIZE), (SET / 2 - 1) * T_SIZE);
	return 0;
}
DWORD WINAPI hd_th2(LPVOID lpVoid) {
	dualpivotquicksort((SET / 2 * T_SIZE), (SET / 4 * 3 - 1) * T_SIZE);
	return 0;
}
DWORD WINAPI hd_th3(LPVOID lpVoid) {
	dualpivotquicksort((SET / 4 * 3) * T_SIZE, (SET - 1) * T_SIZE);
	return 0;
}
DWORD WINAPI ms_th0(LPVOID lpVoid) {
	merge1(0, SET / 4 - 1, SET / 2 - 1);
	return 0;
}
DWORD WINAPI ms_th1(LPVOID lpVoid) {
	merge1(SET / 2, SET / 4 * 3 - 1, SET - 1);
	return 0;
}

int main() {

	int i = 0;
	int count = SET / 10;
	clock_t start, end, first;
	DWORD addr;
	HANDLE qs_th[4];
	HANDLE ms_th[2];
	/*
	HANDLE hFile = CreateFile(TEXT("C:\\DATA\\input.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (hFile == NULL) { printf("<ERROR> CreateFile\n"); return; }
	HANDLE fd = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	
	if (fd == NULL) { printf("<ERROR> CreateFileMapping\n"); return; }
	char *mapFile = (char*)MapViewOfFile(fd, FILE_MAP_READ, 0, 0, 0);
	*/
	data = (char*)malloc(sizeof(char) * SET * T_SIZE);
	first = clock();
	
	fopen_s(&readstream, "C:\\DATA\\input.txt", "r");
	if (readstream == NULL)
		printf("입력파일 오류");
	
	fopen_s(&writestream, "C:\\DATA\\input_sort.txt", "w");
	if (writestream == NULL)
		printf("출력파일 오류");

	//memcpy(data, mapFile, 100);
	fread(data, sizeof(char), T_SIZE * SET-1, readstream);
	//fclose(readstream);
	end = clock();
	printf("File Read = %f\n", (double)(end - first) / (CLOCKS_PER_SEC));

	sort = (char*)malloc(sizeof(char) * SET * T_SIZE);
	start = clock();
	qs_th[0] = CreateThread(NULL, 0, hd_th0, NULL, 0, &addr);
	qs_th[1] = CreateThread(NULL, 0, hd_th1, NULL, 0, &addr);
	qs_th[2] = CreateThread(NULL, 0, hd_th2, NULL, 0, &addr);
	qs_th[3] = CreateThread(NULL, 0, hd_th3, NULL, 0, &addr);
	WaitForMultipleObjects(4, qs_th, TRUE, INFINITE);
	CloseHandle(qs_th[0]);
	CloseHandle(qs_th[1]);
	CloseHandle(qs_th[2]);
	CloseHandle(qs_th[3]);
	
	end = clock();
	printf("Quick Sort time = %f\n", (double)(end - start) / (CLOCKS_PER_SEC));
	
	start = clock();
	ms_th[0] = CreateThread(NULL, 0, ms_th0, NULL, 0, &addr);
	ms_th[1] = CreateThread(NULL, 0, ms_th1, NULL, 0, &addr);
	WaitForMultipleObjects(2, ms_th, TRUE, INFINITE);
	CloseHandle(ms_th[0]);
	CloseHandle(ms_th[1]);
	merge2(0, SET / 2 - 1, SET - 1);
	end = clock();
	printf("Merge Sort time = %f\n", (double)(end - start) / (CLOCKS_PER_SEC));

	
	start = clock();
	for (i = 1; i < SET; i++)
		data[i*T_SIZE-1] = ' ';
	for (i = 1; i < count; i++)
		data[i*T_SIZE*10-1] = '\n';
	end = clock();
	
	printf("Data Processing = %f\n", (double)(end - start) / (CLOCKS_PER_SEC));
	
	start = clock();
	fwrite(data, sizeof(char), T_SIZE * SET - 1, writestream);
	end = clock();
	printf("File Write = %f\n", (double)(end - start) / (CLOCKS_PER_SEC));
	printf("Time = %f\n", (double)(end - first) / (CLOCKS_PER_SEC));
	
	
	fclose(writestream);
	free(sort);
	free(data);
	system("pause");
	return 0;
}