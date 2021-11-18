#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

//Differentiating Bits Structure
struct db{
    int r;
    int* b_bits;
};

//Masking Bits Structure
struct mb{
    int m;
    int* m_bits;
};

//Structure of Fusion Tree Node
struct FusionTree{
    int n;//number of keys
    int *key;//Pointer to array of keys
    bool leaf;// Whether node is leaf or not
    struct db* difbit;// Pointer to structure of differentiation bits
    struct mb* masbit;// Pointer to structure of masking bits
    int gap;
    int node_sketch;
    int mask_sketch;
    int mask_q;
    int mask_b;
    int mask_bm;
    struct FusionTree** next;// Pointer to array of pointers to next nodes
};

//Structure of root
struct root{
    struct FusionTree* r;// Pointer to root node
    int wor_len;
    float c;
    float w;
    int keys_max;
    int keys_min;
};

int main(){
    return 0;
}