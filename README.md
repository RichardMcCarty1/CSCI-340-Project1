# Running

Run 

    make

in this folder to compile the program

From there, the program can be run by entering 

    ./p1

# Limits

There is a hard limit set on the number of available processes to be scanned in this program, that limit being 500.

Additionally, it only supports a maximum of 200 children per process.

Both of these can be modified if need be.

<br>

## Modifying Limits


<br>

### Children Limit 
______
The limit for the supported number of children nodes is set in the Nodes typedef, by changing this to some value greater than 200 and increasing the ```MAXIMUM_CHILDREN``` value on line 28 to match , the program will support more children.

### Process Limit
______
The limit of available processes is bound by the ```MAXIMUM_PROCESSES``` value on line 26, increasing this will permit the program to allocate more memory for the requisite node array.


