build:
	g++ -g -Wall heap_benchmark.cpp -o heap_benchmark -std=c++11 -lpthread -ltcmalloc -lprofiler -lunwind
	g++ -g -Wall cpu_benchmark.cpp -o cpu_benchmark -std=c++11 -lpthread -ltcmalloc -lprofiler -lunwind

# https://gperftools.github.io/gperftools/heapprofile.html
run-heapprof:
	env HEAPPROFILE=prof/heap_benchmark env HEAP_PROFILE_ALLOCATION_INTERVAL=104857600  ./heap_benchmark -t 8

check-heapprof:
	pprof --text ./heap_benchmark prof/heap_benchmark.0004.heap

# https://gperftools.github.io/gperftools/cpuprofile.html
run-cpuprof:
	env CPUPROFILE=prof/cpu_benchmark.out env CPUPROFILE_FREQUENCY=2 ./cpu_benchmark -t 8

check-cpuprof:
	pprof --text ./cpu_benchmark prof/cpu_benchmark.out