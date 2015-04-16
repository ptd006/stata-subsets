/*
 Plugin to generate all subsets for Stata.

 The idea is that you have an indicator variable taking values 0 or 1.

 Value 1 indicates the row belongs to a set.

 It is assumed that initially all included rows are at the end, i.e. you have a dataset like:

    +---------+
    | groupid |
    |---------|
 1. |       0 |
 2. |       0 |
 3. |       0 |
 4. |       1 |
 5. |       1 |
    +---------+

You call the plugin on the specified variable and it removes a row from the set and adds another, 
in a standard "bit shifting" manner.  So, after calling the plugin on the data above you get

   +---------+
   | groupid |
   |---------|
1. |       0 |
2. |       0 |
3. |       1 |
4. |       0 |
5. |       1 |
   +---------+

Once all included rows are at the beginning of the dataset the plugin will return code 103.

The motivation is for exact permutation testing.

You can compile it with something like
cl ksubset.cpp stplugin.c /LD /MD /link /out:../ksubset.plugin

*/

#include "stplugin.h"
#include <iostream>

using namespace std;

// negative return value indicates error!
ST_int findnext(ST_int start, ST_int lastrow, int needle) {
	ST_double v;
	int bit;
	ST_int j = start;
	ST_retcode rc;

	while (j <= lastrow) {
		if (SF_ifobs(j)) {

			if (rc = SF_vdata(1, j, &v)) return(rc);

			bit = int(v);

			if (bit != 0 && bit != 1) {
				SF_error("Group number should be 0 or 1!\n");
				return(-420);
			}

			if (bit == needle) {
				return j;
			}
		}
		else {
			SF_error("Sorry but non-trivial 'if ...' conditions are not supported.\n");
			return(-197);
		}
		j++;
	}

	return j;
}

STDLL stata_call(int argc, char *argv[])
{
	if (SF_nvars() == 0) {
		SF_error("You need to specify a variable of 0s and 1s for the group number.\n");

		return(102);
	}


	ST_retcode rc;

	ST_int firstrow = SF_in1(), lastrow = SF_in2();

	if (firstrow > 1 || lastrow < SF_nobs() ) {
		SF_error("Sorry but input ranges are experimental and disabled.\n");
		return(197);
	}
	
	// find the first 0 (possibly it is the first)
	ST_int first0 = findnext(firstrow, lastrow, 0);
	
	// negative return value indicates error!
	if (first0 < 0) return (-first0);

	// there is at most one zero, and it is already at the end of the dataset
	if (first0 >= lastrow) {
		SF_error("No more subsets.\n");
		return(103); // this is a bad error code!
	}
	
	// first0 < lastrow now
	
	// find the first occurence of 1 AFTER that.
	ST_int l = findnext(first0+1, lastrow, 1);

	// negative return value indicates error!
	if (l < 0) return (-l);

	if (l > lastrow) { // there are no 1s to right of first 0
		SF_error("No more subsets.\n");

		// TODO: This is a bad choice of error code -- it is supposed to mean
		// "too many variables specified"!
		return(103);
	}

	// l should be at least firstrow+1!
	if (l <= firstrow) {
		SF_error("My math is broken :(\n");
		return(666);
	}


	// shift left
	if (rc = SF_vstore(1, l - 1, 1.0)) return(rc);
	if (rc = SF_vstore(1, l, 0.0)) return(rc);

	// if there was a block of 1s to the left then shift them up
	if (first0 > firstrow) {
		int no0s = l - first0 - 1;

		for (ST_int m = firstrow; m < firstrow + no0s; m++) {
			if (rc = SF_vstore(1, m, 0.0)) return(rc);
		}
		for (ST_int m = firstrow + no0s; m <= l - 2; m++) {
			if (rc = SF_vstore(1, m, 1.0)) return(rc);
		}
	}

	return(0) ;
}

