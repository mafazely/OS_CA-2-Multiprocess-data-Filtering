# OS_CA-2-Multiprocess-data-Filtering
An OS course project to filtering stores data and print the max or min value of them.

## How to run ?
In project directory just run `make` command to make project by g++.
them run executable file in this form :
```
‫‪./StoreCalculator.out‬‬ ‬ <commands file path> <‫‪assets‬‬ path>
```

like this :
```
/StoreCalculator.out‬‬ ‬ commands.txt assets
```

### command format (2 types):
```
‫‪MAX‬‬ <product_id> <starting_date> <ending_date>
```
or
```
‫‪MIN‬‬ <product_id> <starting_date> <ending_date>
```


### store data format :
in csv file for each Store :
[csv files must have header.]
```
<date>,<product_id>,<price>
```
for example :
```
MAX 4 2018/01/01 2019/01/01
```
date format is : ```YYYY/MM/DD‬‬```




Created by Amin Fazeli on 23 Nov 2020.

OS Course - 
Fall 1399 - 
University of Tehran

