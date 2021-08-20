build:
	g++ -g -Wall -o cpu_benchmark cpu_benchmark.cpp -lpthread -ltcmalloc -lprofiler -lunwind
	g++ -g -Wall -o heap_benchmark heap_benchmark.cpp -lpthread -ltcmalloc -lprofiler -lunwind
	
clean:
	rm -rf cpu_benchmark heap_benchmark

run-heapprof:
	env HEAPPROFILE=prof/heap_benchmark env HEAP_PROFILE_ALLOCATION_INTERVAL=104857600  ./heap_benchmark -t 8

check-heapprof:
	pprof --text ./heap_benchmark prof/heap_benchmark.0004.heap

run-cpuprof:
	env CPUPROFILE=prof/cpu_benchmark.out env CPUPROFILE_FREQUENCY=2 ./cpu_benchmark -t 8

check-cpuprof:
	pprof --text ./cpu_benchmark prof/cpu_benchmark.out

.PHONY: build clean run-heapprof check-heapprof run-cpuprof check-cpuprof