#include "user_allocate.h"

Linear_allocate Allocate;
int main(void)
{
	setlocale(LC_ALL, "Russian");

	Tester tester;

	tester.check_create_block(&Allocate);
	tester.check_create_over_block(&Allocate);
	tester.check_deallocate(&Allocate);

	
	return 0;
}