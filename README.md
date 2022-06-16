# crc32stress

Measuring crc32 implementation throughput and latency.

Single thread measurements based on vpmsum instruction and hand coded assembly. (See https://github.com/antonblanchard/crc32-vpmsum)

| Model | Arch        | Frequency (GHz)          | Thruput (GB/sec) |Thruput (bytes/cycle) |
| ---- | ------------- |-------------:| -----:|-----:|
| E880 | Power8      | 4.259 | 50.8 | 11.9 |
| E980 | Power9      | 3.762   |   31.0  | 8.2 |
| E1080 | Power10 | 3.996      |    66.5 | 16.3 |
