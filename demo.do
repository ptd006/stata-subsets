* load the plugin
program ksubset, plugin

	
* clear the dataset
clear

* set up initial groups
* The code assumes that the first n - k are in group 0
* and the last k are in group 1.

set obs 24
* gen id = _n
gen groupid = 0
replace groupid = 1 if _n >= 8


local k = 0
while (_rc == 0) {
		local k=`k'+1
		* li
		plugin call ksubset groupid
		}

di `k'
	
* unload the plugin
program drop ksubset

