#include <unistd.h>

int main(void){
	char* argv[11] = {NULL};

	execve("./testExecve", argv, NULL);
}
