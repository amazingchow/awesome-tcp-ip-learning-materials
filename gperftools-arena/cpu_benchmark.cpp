#include <gperftools/profiler.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int THREAD_NUM = 1;		// 并发线程数
int TEST_NUM = 1e9;		// 循环测试次数
int ALLOC_SIZE = 128;	// 一次性分配的内存块大小

void* loop_alloc(void* args)
{
	timeval t_start, t_end;
	void* addr;

	gettimeofday(&t_start, 0);

	for (int i = 0; i < TEST_NUM; i++) {
		addr = malloc(ALLOC_SIZE);
		free(addr);
	}

	gettimeofday(&t_end, 0);

	*(long*)args = (1000000LL * (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec));

	return 0;
}

int main(int argc, char** argv)
{
	ProfilerStart("");

	int ch;
	while ((ch = getopt(argc, argv, "t:s:n:")) != -1) {
		switch (ch) {
		case 't':
			THREAD_NUM = atoi(optarg);
			break;
		case 's':
			ALLOC_SIZE = atoi(optarg);
			break;
		case 'n':
			TEST_NUM = atoi(optarg);
			break;
		}
	}

	// 统计每个线程运行所需用时
	long* time_cost = new long[THREAD_NUM];
	long cost_sum = 0;

	if (1 == THREAD_NUM) {
		loop_alloc(&cost_sum);
	} else {
		pthread_t* t_ids = new pthread_t[THREAD_NUM];

		for (int i = 0; i < THREAD_NUM; i++) {
			int ret = pthread_create(&t_ids[i], NULL, loop_alloc, &time_cost[i]);
			if (ret != 0) {
				printf("failed to create pthread!\n");
				exit(1);
			}
		}

		for (int i = 0; i < THREAD_NUM; i++) {
			pthread_join(t_ids[i], NULL);
			cost_sum += time_cost[i];
		}
	}

	printf("%d个线程平均用时: %.2fms\n", THREAD_NUM, (double)cost_sum / THREAD_NUM / 1000);

	ProfilerStop();
}