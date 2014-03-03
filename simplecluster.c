#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int maxMergeThreshold = 50;

typedef struct linkedlist {
    int n1; //note 1
    int n2; //note 2
    int x;
    struct linkedlist *next;
} ll;

ll* new_ll(int n1, int n2, int x)
{
    ll* new = calloc(1, sizeof(ll));
    if (new == NULL)
    {
        printf("failure to create a new linked list.\n");
        exit(1);
    }
    new->n1 = n1;
    new->n2 = n2;
    new->x = x;
    new->next = NULL;
    return new;
}

ll * insert_ll(ll* curr, int n1, int n2, int y)
{
    ll* new = calloc(1, sizeof(ll));
    if (new == NULL)
    {
        printf("failure to create a new linked list for insertion");
        exit(1);
    }
    new->n1 = n1;
    new->n2 = n2;
    new->x = y;
    new->next = curr;
    return new;
}

ll* delete_ll(ll* curr, int z, int n1, int n2)
{
    ll* temp;
    if (curr == NULL)
        return;
    if (curr->x == z && curr->n1 == n1 && curr->n2 == n2)
    {
        temp = curr->next;
        free(curr);
        return temp;
    }
    ll* head = curr;
    temp = curr->next;
    while (temp != NULL && temp->x != z)
    {
        curr = temp;
        temp = temp->next;
    }
    if (temp == NULL)
    {
        return curr;
    }
    curr->next = temp->next;
    free(temp);
    return head;
}

void free_ll(ll* curr)
{
    if (curr == NULL)
        return;
    ll* temp = curr;
    while(temp != NULL)
    {
        curr = temp;
        temp = temp->next;
        free(curr);
    }
    return;
}

ll* merge_ll(ll* first, ll* second)
{
    fprintf(stderr, "merging LL!");

    if (first == NULL)
        return second;
    if (second == NULL)
        return first;
    ll* temp = first;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = second;
    fprintf(stderr, "done merging ll!\n");
    fflush(stderr);
    return first;
}

typedef struct funcluster
{
    double mean;
    int numOfItems;
    ll* list;
} cluster;

cluster* create_cluster(int n1, int n2, int y)
{
    ll* new = new_ll(n1, n2, y);
    cluster* newCluster = calloc(1, sizeof(cluster));
    if (newCluster == NULL)
    {
        printf("couldn't create a cluster!\n");
        exit(1);
    }
    newCluster->mean = (double)y;
    newCluster->numOfItems = 1;
    newCluster->list = new;
    return newCluster;
}

cluster* merge_clusters(cluster* a, cluster* b)
{
    if (a == NULL)
        return b;
    if (b == NULL)
        return a;
    cluster* newCluster = calloc(1, sizeof(cluster));
    if (newCluster == NULL)
    {
        printf("couldn't create a cluster!\n");
        exit(1);
    }

    fprintf(stderr, "merging clusters!");

    newCluster->numOfItems = a->numOfItems + b->numOfItems;
    newCluster->mean = (((a->mean) * (double)a->numOfItems) + ((b->mean) * (double)b->numOfItems)) / (double)newCluster->numOfItems;
    newCluster->list = merge_ll(a->list, b->list);
    fprintf(stderr, "freeing a in merge clusters\n");

    free(a);
    fprintf(stderr, "freeing b in merge clusters!\n");

    free(b);
    fprintf(stderr, "finished merging clusters\n");
    fflush(stderr);
    return newCluster;
}

void free_cluster(cluster* a)
{
    if (a->list != NULL)
        free_ll(a->list);
    free(a);
    return;
}

cluster** runClustering(cluster* clusterList[], int numOfClusters, int* newLength)
{
    cluster* a;
    cluster* b;
    cluster* temp;
    int i, j;
    int aIndex, bIndex;
    int currDifference = 0;
    int tempDiff;
    int newNum = numOfClusters;
    while (currDifference < maxMergeThreshold)
    {
        aIndex = -1;
        bIndex = -1;
        a = NULL;
        b = NULL;
        currDifference = ~(1 << 31);

        for (i = 0; i < numOfClusters; i++)
        {
            for (j = i + 1; j < numOfClusters; j++)
            {
                if (clusterList[i] != NULL && clusterList[j] != NULL)
                {
                    tempDiff = ((clusterList[i]->mean - clusterList[j]->mean > 0) ? (clusterList[i]->mean - clusterList[j]->mean) : (clusterList[j]->mean - clusterList[i]->mean));
                    if (tempDiff < currDifference)
                    {
                        aIndex = i;
                        bIndex = j;
                        a = clusterList[i];
                        b = clusterList[j];
                        currDifference = tempDiff;
                    }
                }
            }
        }

        if (currDifference < maxMergeThreshold)
        {

            clusterList[aIndex] = merge_clusters(a, b);
            clusterList[bIndex] = NULL;
            newNum = newNum - 1;
        }
    }

    cluster** newClusterList = calloc(newNum, sizeof(cluster *));
    if (newClusterList == NULL)
    {
        printf("couldn't resize to new cluster list!\n");
        exit(1);
    }

    j = 0;
    for (i = 0; i < numOfClusters; i++)
    {
        if (clusterList[i] != NULL)
        {
            newClusterList[j] = clusterList[i];
            j++;
        }
    }

    if (j != newNum)
    {
        printf("error: registed %d numbers but array shifting only got %d\n", newNum, j);
        exit(1);
    }

    *newLength = newNum;

//    fprintf(stderr, "freeing old list!");
    fflush(stderr);
    free(clusterList);
    return newClusterList;
}

void printList(ll* list)
{
    while (list != NULL)
    {
        printf("Interval - Harmonics %d - %d, Difference: %d cents\n", list->n1, list->n2, list->x);
        list = list->next;
    }
    fflush(stdout);
    return;
}

void printClusters(cluster** clusterArray, int len)
{
    int i = 0;
    while (i < len && clusterArray[i] != NULL)
    {
        printf("Mean: %f cents with %d intervals:\n", clusterArray[i]->mean, clusterArray[i]->numOfItems);
        printList(clusterArray[i]->list);
        printf("\n");
        i++;
    }
    fflush(stdout);
    return;
}

void sortClusters(cluster** clusterArray, int len)
{
    int i = 0;
    int j = 1;
    double currMin;
    int currMinIndex;
    cluster* temp;
    while (i < len - 1)
    {
        currMin = clusterArray[i]->mean; //kinda hacky large double
        currMinIndex = i;
        for (j = i + 1; j < len; j++)
        {
            if (clusterArray[j]->mean < currMin)
            {
                currMin = clusterArray[j]->mean;
                currMinIndex = j;
            }
        }

        temp = clusterArray[currMinIndex];
        clusterArray[currMinIndex] = clusterArray[i];
        clusterArray[i] = temp;
        i++;
    }

}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("needs just a file to cluster\n");
        exit(1);
    }
    FILE* inputFile = fopen(argv[1], "r");
    char temp[768];
    char name[128];
    int n1;
    int n2;
    int i = 0;
    int j;
    int centsDiff;
    int numOfTones;
    fgets(temp, 768, inputFile);
    sscanf(temp, "%127[^ \n]s", name);
    fgets(temp, 768, inputFile);
    sscanf(temp, "%d", &(numOfTones));
    int numOfIntervals = numOfTones * (numOfTones - 1) / 2;
    int * toneArray = calloc(numOfTones, sizeof(int));
    while(fgets(temp, 768, inputFile) != NULL)
    {
        sscanf(temp, "%d", &(toneArray[i]));
        i = i + 1;
    }

    cluster** clusterArray = calloc(numOfIntervals, sizeof(cluster*));
    int pos = 0;
    for (i = 0; i < numOfTones; i++)
    {
        for (j = i + 1; j < numOfTones; j++)
        {
                clusterArray[pos] = create_cluster(i, j, ((toneArray[j] - toneArray[i] > 0) ? toneArray[j] - toneArray[i] : toneArray[i] - toneArray[j]));
                pos++;
        }
    }

    if (pos != numOfIntervals)
    {
        printf("didn't insert as many clusters as numOfIntervals at start\n");
        exit(1);
    }

    fprintf(stderr, "about to run clustering... \n");
    fflush(stdout);
    int newLength;
    clusterArray = runClustering(clusterArray, pos, &newLength);

    printf("\nScale: %s with %d tones - %d intervals\n", name, numOfTones, numOfIntervals);
    printf("Threshold for combining clusters: %d cents\n\n", maxMergeThreshold);
    sortClusters(clusterArray, newLength);
    printClusters(clusterArray, newLength);

    fclose(inputFile);
    free(toneArray);
    for (i = 0; i < newLength; i++)
    {
        if (clusterArray[i] != NULL)
        {
//            fprintf(stderr, "freeing %d cluster\n", i);
            fflush(stderr);
            free_cluster(clusterArray[i]);
        }
    }
    free(clusterArray);
    return 0;
}

