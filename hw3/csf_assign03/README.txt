Team Members:
Ruoxi Liu rliu79
Michael Ye sye10

Best cache configuration:
The follwing tests were run, (number of sets, number of blocks, number of bytes in each block):
To keep everything consistent, the total cache size will remain the same for all of the configurations.

1. Fully associative, (1, 1024, 16), write-allocate, write-back, fifo, gcc.trace
This test was run as a baseline, as you can't really get better performance than a fully associative cache,
but fully associative caches are unrealistic in practice.
Result:
Total loads: 318197
Total stores: 197486
Load hits: 314311
Load misses: 3886
Store hits: 188117
Store misses: 9369
Total cycles: 9919114

The following tests all vary the number of sets and blocks in each cache to find the ideal combination:
2. (2, 512, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314317
Load misses: 3880
Store hits: 188115
Store misses: 9371
Total cycles: 9915918

3. (4, 256, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314314
Load misses: 3883
Store hits: 188115
Store misses: 9371
Total cycles: 9916715

4. (8, 128, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314309
Load misses: 3888
Store hits: 188103
Store misses: 9383
Total cycles: 9929098

5. (16, 64, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314315
Load misses: 3882
Store hits: 188089
Store misses: 9397
Total cycles: 9931890

6. (32, 32, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314293
Load misses: 3904
Store hits: 188081
Store misses: 9405
Total cycles: 9949860

7. (64, 16, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314283
Load misses: 3914
Store hits: 188075
Store misses: 9411
Total cycles: 9962644

8. (128, 8, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314249
Load misses: 3948
Store hits: 188090
Store misses: 9396
Total cycles: 9965825

9. (256, 4, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 10029304

10. (512, 2, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 313943
Load misses: 4254
Store hits: 187935
Store misses: 9551
Total cycles: 10206564

11. (1024, 1, 16), write-allocate, write-back, fifo, gcc.trace
Total loads: 318197
Total stores: 197486
Load hits: 312238
Load misses: 5959
Store hits: 187502
Store misses: 9984
Total cycles: 11308826

The results of these series of tests were quite similar, with x-way set associative caches performing better as x
increases. We decided to go with (128, 8, 16), a 4-way set associative configuration as it is realistic while maintaining
solid performance.

12. (128, 8, 16), write-allocate, write-back, lru, gcc.trace
This test was run to see if lru would improve the performance of this cache configuration. 
Total loads: 318197
Total stores: 197486
Load hits: 314908
Load misses: 3289
Store hits: 188278
Store misses: 9208
Total cycles: 9462672
There were some improvements for the number of load misses, which improved the total performance of the cache.
We decided to stick with LRU as the replace strategy.


13. (128, 8, 16), no-write-allocate, write-through, lru, gcc.trace
This test was run to see if changing to no-write-allocate + write-through would increase the performance.
Total loads: 318197
Total stores: 197486
Load hits: 311693
Load misses: 6504
Store hits: 164933
Store misses: 32553
Total cycles: 22826826
This was much worse, this configuration shouldn't be used.

14. (128, 8, 16), write-allocate, write-through, lru, gcc.trace
This test was run to try out another alternate configuration.
Total loads: 318197
Total stores: 197486
Load hits: 314908
Load misses: 3289
Store hits: 188278
Store misses: 9208
Total cycles: 25250586
The cache hit rate is the same as the write-allocate, write-back configuration, but the cache miss penalty is much higher,
resulting in a much higher total cycles count. Hence, we decided that the best configuration is write-allocate + write-back

After all these tests, we decided that the best cache configuration was:
- 128 sets in the cache
- 8 way set associative
- 16 bytes in each block
- write-allocate
- write-back
- lru


Contributions:
We met in person and worked on all of the functions together.

Michael:
-main.cpp and input parsing
-cases of load() function, including:
-handle_load_hit()
-handle_load_miss_LRU() and handle_load_miss_FIFO()
-error handling
-debug

Ruoxi:
-Cache Initialization
-get_tag(), get_index() and find() methods
-the load() function that decides which sub-case we are dealing with
-store() function
-debug