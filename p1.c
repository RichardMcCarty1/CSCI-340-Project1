#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>


typedef struct Node {
    struct Nodes *children;
    int pid;
    int ppid;
    unsigned long int vsize;
    char comm[50];
}Node;

typedef struct Nodes {
    struct Node *grouping[200];
}Nodes;

void dfsPrint(Node *children, int depth, int MAXIMUM_CHILDREN);

int binarySearch(Node *nodeList, int l, int r, int val);

int search(Node *nodeList, int val,int MAXIMUM_PROCESSES);

int main(void) {        
    int MAXIMUM_PROCESSES = 500;

    int MAXIMUM_CHILDREN = 200;

    struct dirent *directoryEntry;

    struct Node* nodeList;
    nodeList = calloc(MAXIMUM_PROCESSES, sizeof(Node));


    DIR *dirPointer = opendir("/proc");

    FILE *processFile;
    int i = 0;
    while((directoryEntry = readdir(dirPointer)) != NULL) {
        //Put name in variable
        char dName[50];
        char buff[100];
        strcpy(dName, directoryEntry->d_name);
        //Length of directory name versus str->int->str conversion, i.e. any non digit characters get filtered such that only pure # names persist
        sprintf(buff, "%d", atoi(dName));
        //also excludes the decimal root folder
        if(strlen(dName) == strlen(buff) && dName[0] != '.') {
            //Create a node struct
            struct Node newNode;
            //Push dir string to buff
            sprintf(buff, "/proc/%s/stat", dName);
            //Open stat file
            processFile = fopen(buff, "r");
            
            //Scan the file data, outputting important values to newNode elements
            fscanf(processFile, "%d %s %c %d %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %lu", &newNode.pid, newNode.comm, 
            buff, &newNode.ppid, buff, buff,buff,buff,buff,buff,buff,buff,buff,buff,buff,buff,buff,buff,buff,buff,buff,&newNode.vsize);   
            //populate the child array with dummy values 
            Nodes *childGroup = calloc(MAXIMUM_CHILDREN, sizeof(Node));
            for(int i = 0; i < MAXIMUM_CHILDREN; i++) {
                Node garbage;
                garbage.pid = 0;garbage.ppid=0;garbage.vsize=0;strcpy(garbage.comm, "");
                (*childGroup).grouping[i] = &garbage;
            }
            //Assign the childarr to childGroup, add to nodeList and increment
            newNode.children = childGroup;
            nodeList[i++] = newNode;
            
            //Close so you dont brick your OS
            fclose(processFile);
        }
    }


    for(int i = 0; i < MAXIMUM_PROCESSES; i++) {
        if(nodeList[i].ppid == 0 || nodeList[i].pid > 4000000 || nodeList[i].pid <= 0) {
            continue;
        }

        int indexOfParent = search(nodeList, nodeList[i].ppid, MAXIMUM_PROCESSES);
        if(indexOfParent < 0) {
            continue;
        }



        //Pointer to beginning of Nodes.grouping array
        Node *groupingArr = nodeList[indexOfParent].children->grouping;

        //Iterate through Nodes.grouping
        for(int k = 0; k < MAXIMUM_CHILDREN; k++) {

            //Current Node we check to see if its a dummy node, if pid > 32-bit OS limit, or if its negative, or if vsize is 0
            //This checks for errant address over-runs or if the node is the pre-generated dummy node that occured during init
            if((groupingArr[k].vsize) == 0 || groupingArr[k].pid <= 0 || groupingArr[k].pid > 4000000 ){
                //The current node is an invalid node, lets replace it with our desired node
                groupingArr[k] = nodeList[i];
                Node *shouldPointToGroupingArr = nodeList[indexOfParent].children->grouping;
                break;
            }
            continue;
        }

    }

    //Iterate through every child list of every node
    //If PPID != 0, skip because itll be pathed to later, if == 0, print value, bring up child array, iterate through
    //DFS-style
    for(int i = 0; i < MAXIMUM_PROCESSES; i++) {
        if(nodeList[i].pid == 0 || nodeList[i].vsize == 0 || nodeList[i].ppid != 0) {
            continue;
        }
        
        printf("%s(%d) %s, %lu kb\n", "", nodeList[i].pid, nodeList[i].comm, nodeList[i].vsize);

        Node *groupingPointer = nodeList[i].children->grouping;
        dfsPrint(groupingPointer, 1, MAXIMUM_CHILDREN);
    }
    closedir(dirPointer);
    return 0;
}
void dfsPrint(Node *children, int depth, int MAXIMUM_CHILDREN) {
    //Generates Tab characters based on depth to offset children from their parent
    char tabDepth[50] = "";
    for(int i = 0; i < depth; i++) {
        strcat(tabDepth, "\t");
    }
    //If the child is a valid node, print it!
    for(int i = 0; i < MAXIMUM_CHILDREN; i++) {
        if(children[i].pid > 0 && children[i].pid < 4000000) {
            printf("%s(%d) %s, %lu kb (Expected PPID:%d)\n", tabDepth, children[i].pid, children[i].comm, children[i].vsize, children[i].ppid);
            Node *childsChildren = children[i].children->grouping;
            dfsPrint(childsChildren, depth+1, MAXIMUM_CHILDREN);
        }
    }
}
int search(Node *nodeList, int val, int MAXIMUM_PROCESSES) {
    for(int i = 0; i < MAXIMUM_PROCESSES; i++) {
        if(nodeList[i].pid == val) {
            return i;
        }
    }
    return -1;
}
int binarySearch(Node *nodeList, int l, int r, int val) {
    if(r >= 1) {
        int midVal = l + (r) / 2;

        if(nodeList[midVal].pid == val)  {
            return midVal;
        }
        if(nodeList[midVal].pid > val) {
            return binarySearch(nodeList, l, midVal - 1, val);
        }

        return binarySearch(nodeList, midVal + 1, r, val);
    }
}
