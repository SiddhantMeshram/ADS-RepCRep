# ADS-RepCRep

To compile the code, you can run `make`.

After compiling, you can run the code by using the following command and providing input using stdin.
```
./repcrep
```

If you want to provide the input from a file, you can use input redirection.
```
./repcrep < <input_file>
```

If you want to run all the tests under inputs directory, you can use the script
provided:
```
./script.sh
```
We have provided a repro-test.rpz file which is created using reprozip. It
can be unzipped using
```
reprounzip directory setup repro-test.rpz ./unzipped_dir
```

To run the executable:
```
reprounzip directory run ./unzipped_dir
```

To pass input as a file:
```
reprounzip directory run ./unzipped_dir < <input_file>
```