#include <stdio.h>
#include <stdint.h> //uint8_t, etc...

#define sebSizeOfArray1( array ) ( sizeof array / sizeof *array )

long unsigned sebSizeOfArray2( char * array ) {
	return sizeof array / sizeof *array;
}

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

	char a[25]; //byte array

	printf("\n=> sizeof(a) = %lu\n", sizeof a / sizeof(char));

	printf("\n=> sizeof(a) = %lu\n", sebSizeOfArray1(a));

	printf("\n=> sizeof(a) = %lu\n", sebSizeOfArray2(a));

//  printf("Hit enter to exit.\n");
//  char *scannedText;
//  scanf("%s", scannedText);

	return 0;

}
