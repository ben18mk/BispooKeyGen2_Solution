#pragma warning(disable : 4996)

#include <stdio.h>
#include <string.h>

#define MAX_PATH 255

unsigned movToLow(unsigned num, unsigned byte)
{
	return num & 0xffffff00 | byte & 0xff;
}

unsigned getLow(unsigned num)
{
	return num & 0xff;
}

unsigned dword(char* ptr)
{
	return *(ptr) & 0xff |
		(*(ptr + 1) << 8) & 0xff00 |
		(*(ptr + 2) << 16) & 0xff0000 |
		(*(ptr + 3) << 24) & 0xff000000;
}

void __stdcall xortographer(char* param)
{
	int i = 0;

	while (*(param + i))
		*(param + i++) ^= 0xbe;
}

void __stdcall passwordFinalTouch(char* param)
{
	int i = 0;

	while (*(param + i))
	{
		if (*(param + i) <= (char) 0x8f)
			i++;
		else if (*(param + i) == (char) 0x93)
			*(param + i++) = 0x8d;
		else
			*(param + i++) = 0x1;
	}

	*(param + i++) = 0x93;
	*(param + i++) = 0xf2;
	*(param + i++) = 0xfb;
	*(param + i++) = 0xfb;
	*(param + i++) = 0xea;
	*(param + i) = 0x0;
}

int main()
{
	unsigned char* username = (unsigned char*)malloc(MAX_PATH);
	unsigned char* password = (unsigned char*)malloc(MAX_PATH);

	printf("Username: ");
	scanf("%s", username);

	printf("Password: ");
	scanf("%s", password);

	int usernameLen = strlen(username);
	if (usernameLen < 4)
	{
		printf("Username must have 4 letters minimum\n");
		return -1;
	}
	else if (usernameLen > 15)
	{
		printf("Username cannot exceed 15 letters\n");
		return -1;
	}
	else if (*username == ' ')
	{
		printf("No spaces in username first character\n");
		return -1;
	}

	unsigned eax = 0, i = 0, edx = 0, j = 1;

	unsigned char* keygen = (unsigned char*)malloc(usernameLen + 6);
	for (int i = 0; i < usernameLen + 1; i++)
		*(keygen + i) = 0;

	do
	{
		eax = movToLow(eax, *(username + i));
		eax *= j;
		eax += j;
		eax <<= 0xde;
		eax >>= 0xad; // Can be combined to a single line of eax << 0x31
		eax ^= j;
		eax = movToLow(eax, getLow(eax) + 0x35);
		*(keygen + i) = getLow(eax);
		edx += eax;
		i++;
		j++;
	} while (getLow(i) != getLow(usernameLen));

	edx *= dword(keygen);
	edx = ~edx;

	sprintf(keygen, "%i", edx);

	xortographer(keygen);
	passwordFinalTouch(keygen);
	
	i = 0;
	while (*(password + i) == (char)(*(keygen + i) ^ 0xbe))
	{
		if (*(password + ++i) || *(keygen + i))
			continue;
 
		if (i > 5)
		{
			printf("GOOD\n");
			return 0;
		}
	}

	printf("BAD\n");

	return 0;
}
