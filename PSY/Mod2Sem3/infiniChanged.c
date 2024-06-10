#include <unistd.h>

int main(void){
	alarm(10);
	pause();
	return 0;
}
