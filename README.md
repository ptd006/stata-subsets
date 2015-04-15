# stata-subsets
Generate all subsets in Stata (for combinatorial tests, for example)

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

See the demo.do file for an example.

This is a preliminary version and does not support non-trivial "if ..." conditions, or ranges for the input variable.  (The latter should work, but not tested).
