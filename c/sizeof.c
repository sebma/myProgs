#include <stdio.h>
#include <stdint.h> //uint8_t, etc...

int main (int argc, char **argv) {

	printf("\nsizeof(char):  %lu bits\n", 8 * sizeof(char) );

	printf("sizeof(short): %lu bits\n", 8 * sizeof(short));

	printf("sizeof(int):   %lu bits\n", 8 * sizeof(int));

	printf("sizeof(long):  %lu bits\n", 8 * sizeof(long));

	printf("sizeof(long long): %lu bits\n", 8 * sizeof(long long));

	printf("sizeof(void *):%lu bits\n", 8 * sizeof(void *));

	printf("sizeof(float):  %lu bits\n", 8 * sizeof(float));

	printf("sizeof(double):  %lu bits\n", 8 * sizeof(double));

	printf("sizeof(long double):  %lu bits\n", 8 * sizeof(long double));

	printf("\n=> Standard types from stdint.h :\n");

	printf("sizeof(int8_t): %lu bits\n", 8 * sizeof(int8_t));

	printf("sizeof(int16_t): %lu bits\n", 8 * sizeof(int16_t));

	printf("sizeof(int32_t): %lu bits\n", 8 * sizeof(int32_t));

	printf("sizeof(int64_t): %lu bits\n", 8 * sizeof(int64_t));

	uint8_t a[15]; //byte array

	printf("\n=> sizeof(a) = %lu\n", sizeof(a));

	printf("=> sizeof(a)/sizeof(uint8_t) = %lu\n", sizeof(a)/sizeof(uint8_t));

//  printf("Hit enter to exit.\n");
//  char *scannedText;
//  scanf("%s", scannedText);

	return 0;

}
