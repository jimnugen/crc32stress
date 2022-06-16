#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <locale.h>

struct read_format {
	uint64_t nr;
	struct {
		uint64_t value;
		uint64_t id;
	} values[];
};

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
		int cpu, int group_fd, unsigned long flags)
{
	int ret;
	ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
	return ret;
}

unsigned int crc32_vpmsum(unsigned int crc, unsigned char *p, unsigned long len);

int main(int argc, char *argv[])
{
	unsigned long length, iterations;
	unsigned char *data;
	unsigned long i;
	unsigned int crc = 0;

	//
	// Performs crc32 checksum an [iterations] number of times on a buffer filled with junk data of [length] bytes
	//

	if (argc != 3) {
		fprintf(stderr, "Using defaults - length: 1000000 iterations: 600000\n");
		length=1000000;
		iterations=600000;
	} else {
		length = strtoul(argv[1], NULL, 0);
		iterations = strtoul(argv[2], NULL, 0);
	}
	data = (unsigned char *)memalign(getpagesize(), length);

	srandom(1);
	for (i = 0; i < length; i++)
		data[i] = random() & 0xff;

	struct perf_event_attr pea;
	int fd1;
	uint64_t cycles;
	uint64_t id1;
	char buf[4096];
	struct read_format* rf = (struct read_format*) buf;

	memset(&pea, 0, sizeof(struct perf_event_attr));
	pea.type = PERF_TYPE_HARDWARE;
	pea.size = sizeof(struct perf_event_attr);
	pea.config = PERF_COUNT_HW_CPU_CYCLES;
	pea.disabled = 1;
	pea.exclude_kernel = 1;
	pea.exclude_hv = 1;
	pea.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;
	fd1 = syscall(__NR_perf_event_open, &pea, 0, -1, -1, 0);
	ioctl(fd1, PERF_EVENT_IOC_ID, &id1);


	// Start measuring time
	struct timespec begin, end; 
	clock_gettime(CLOCK_REALTIME, &begin);

	// Start PMU measuring 
	ioctl(fd1, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
	ioctl(fd1, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);


	for (i = 0; i < iterations; i++)
		crc = crc32_vpmsum(crc, data, length);


	// Stop PMU measuring 
	ioctl(fd1, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);

	// Stop measuring time and calculate the elapsed time
	clock_gettime(CLOCK_REALTIME, &end);

	long seconds = end.tv_sec - begin.tv_sec;
	long nanoseconds = end.tv_nsec - begin.tv_nsec;
	double elapsed = seconds + nanoseconds*1e-9;

	read(fd1, buf, sizeof(buf));
	for (i = 0; i < rf->nr; i++) {
		if (rf->values[i].id == id1) {
			cycles = rf->values[i].value;
		}
	}

	printf("CRC value    : %08x\n", crc);
	printf("cpu cycles   : %"PRIu64"\n", cycles);

	double freq =  cycles / (elapsed * 1000000000);

	printf("cpu frequency: %.3f GHz\n", freq);

	printf("Elapsed time : %.3f seconds\n", elapsed);
	unsigned long long total_bytes = length * iterations;
	setlocale(LC_NUMERIC, "");
	printf("Bytes scanned: %'llu\n", total_bytes);
	double GB_sec = total_bytes / ( elapsed * 1000000000);
	printf("GB / sec     : %.2lf\n", GB_sec);
	double bytes_cyc = total_bytes / cycles;
	printf("B / cyc      : %.2lf\n", bytes_cyc);

	return 0;
}
