How to run these benchmarks:

First, some prerequisites are:

* make
* clang 20 (because of `std::flat_map`)
* Boost

Now, run:

```bash
make
python3 bench.py
python3 make_chart_data.py < output | python3 make_html.py
```

Your charts are now in charts.html.

You can tweak some of the values in bench.py to make it run faster at the
expense of less granular data, and you might need to tweak some of the tickSize
settings in charts-template.html.

To run the benchmark at the highest priority possible, do this:

```bash
sudo nice -n-20 ionice -c1 -n0 sudo -u $USER python bench.py
```

You might also want to disable any swap files/partitions so that swapping
doesn't influence performance.  (The programs will just die if they try to
allocate too much memory.)

Copyright Information
=====================

Written by Nick Welch in 2010.
Forked by Tessil in 2016.
No copyright.  This work is dedicated to the public domain.
For full details, see http://creativecommons.org/publicdomain/zero/1.0/
