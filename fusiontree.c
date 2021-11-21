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

//function to calculate the differentiating bits
struct db* DiffBits(struct root* rt,struct FusionTree* p){
    struct db* res;
    res=(struct db*)malloc(sizeof(struct db));
    int bits=0;
    for(int i=0;i<p->n;i++){
        if(p->key[i]==0)break;
        for(int j=0;j<i;j++){
            int w=rt->w;
            while(((p->key[i] & 1 << w)==(p->key[j] & 1 << w)) && w>=0){
                w--;
            }
            if(w>=0)bits |= 1 << w;
        }
    }
    int i=0;
    res->r=0;
    while(i<rt->w){
        if((bits & (1 << i))> 0){
            res->r++;
        }
        i++;
    }
    res->b_bits=(int*)malloc(res->r*sizeof(int));
    int cnt=0;
    i=0;
    while(i<rt->w){
        if((bits & (1 << i))> 0){
            res->b_bits[cnt]=i;
            cnt++;
        }
        i++;
    }
    return res;
}

//function to calculate the masking bits and multiplication constant
struct mb* Const(struct root* rt,struct FusionTree* p){
    struct mb* res;
    res=(struct mb*)malloc(sizeof(struct mb));
    res->m_bits=(int*)malloc(p->difbit->r*sizeof(int));
    for(int i=0;i<p->difbit->r;i++)res->m_bits[i]=0;
    for(int t=0;t<p->difbit->r;t++){
        int mt=0;
        bool flag = true;
        while(flag == true){
            flag = false;
            for(int i=0;i<p->difbit->r;i++){
                if(flag == true)break;
                for(int j=0;j<p->difbit->r;j++){
                    if(flag==true)break;
                    for(int k=0;k<p->difbit->r;k++){
                        if(mt == (p->difbit->b_bits[i] - p->difbit->b_bits[j] + res->m_bits[k])){
                            flag = true;
                            break;
                        }
                    }
                }
            }
            if(flag == true)mt++;
        }
        res->m_bits[t] = mt;
    }
    res->m=0;
    for(int i=0;i<p->difbit->r;i++){
        res->m |= 1 << res->m_bits[i];
    }
    return res;
}

//to calculate mask of our node
int mask(int* temp,struct FusionTree* p){
    int res=0;
    for(int i=0;i<p->difbit->r;i++){
        res |= 1 << temp[i];
    }
    return res;
}

//function to calculate approximate sketch
int sketchap(struct root* rt,struct FusionTree* p, int x){
    int xx = x & p->mask_b;
    int res = xx * p->masbit->m;
    res = res & p->mask_bm;
    return res;
}
struct root *root;

struct FusionTree* allocateNode(int tn){
	struct FusionTree *temp;
	temp=(struct FusionTree*)malloc(sizeof(struct FusionTree));
	temp->key = (int*)malloc((2*tn)*sizeof(int));
	temp->leaf=1;
	temp->next = (struct FusionTree **)malloc((2*tn)*sizeof(struct FusionTree *));
	for(int i=0;i<2*tn;i++){
		temp->next[i]=NULL;
	}
	return temp;
	
}
//Creates a empty fusiontree
void fusionTreeCreate(int ta){
	struct root* temp;
	temp=(struct root*)malloc(sizeof(struct root));
	temp->r = allocateNode(ta);//Allocates space and initialize values for root node
	temp->r->n=0;
	root = temp;
} 
// Performs split child operation
void fusionTreeSplitChild(struct FusionTree *p,int i,int ta){
	struct FusionTree *z= allocateNode(ta);
	struct FusionTree *y= p->next[i];
	z->leaf=y->leaf;
	z->n=ta-1;
	for(int j=0;j<ta-1;j++){
		z->key[j]=y->key[j+ta];
	}
	if(y->leaf==0){
		for(int j=0;j<ta;j++){
			z->next[j]=y->next[j+ta];
		}
	}
	y->n=ta-1;
	for(int j=p->n;j>i;j--){
		p->next[j+1]=p->next[j];
	}
	p->next[i+1]=z;
	for(int j=p->n-1;j>i-1;j--){
		p->key[j+1]=p->key[j];
	}
	p->key[i]=y->key[ta-1];
	p->n++;
}
void fusionTreeInsertNonfull(struct FusionTree *p,int k, int ta){
	int i=p->n-1;
	if(p->leaf==1){
		while(i>=0 && k<p->key[i]){
			p->key[i+1]=p->key[i];
			i--;
		}
		p->key[i+1]=k;
		p->n++;
	}
	else{
		while(i>=0 && k<p->key[i]){
			i--;
		}
		i++;
		if(p->next[i]->n==2*ta-1){
			fusionTreeSplitChild(p,i,ta);
			if(k>p->key[i])
				i++;
		}
		fusionTreeInsertNonfull(p->next[i],k,ta);
	}
}
void fusionTreeInsert(int k,int ta){
	struct root *rty = root;
	if(r->n == 2*ta-1){
		struct FusionTree *s = allocateNode(ta);
		s->leaf=0;
		s->n=0;
		s->next[0]=r;
		root->r=s;
		fusionTreeSplitChild(s,0,ta);
		fusionTreeInsertNonfull(s,k,ta);
	}
	else{
		fusionTreeInsertNonfull(rty->r,k,ta);
	}
}

//function to initiate a node of our fusion tree
void initiatenode(struct root* rt,struct FusionTree* p){
    if(p->n!=0){
        p->difbit = DiffBits(rt,p);//calling function to calculate differentiating bits
        p->masbit = Const(rt,p);//calling function to calculate making bits
        p->mask_b = mask(p->masbit->m_bits,p);

        int * temp;//to store indices of imp bits
        temp=(int*)malloc(p->difbit->r*sizeof(int));

        for(int i=0;i<p->difbit->r;i++){
            temp[i]=(p->difbit->b_bits[i]+p->masbit->m_bits[i]);
        }
        p->mask_bm = mask(temp,p);

        int r3=pow(p->difbit->r,3);
        p->node_sketch=0;
        int sketch_len = r3+1;
        p->mask_sketch=0;
        p->mask_q = 0;
        for(int i=0;i<p->difbit->r;i++){
            int sketch = sketchap(rt,p,p->key[i]);
            temp[i]=1<<r3;
            temp[i] |= sketch;
            p->node_sketch <<= sketch_len;
            p->node_sketch |= temp[i];
            p->mask_q |= 1 << i*(sketch_len);
            p->mask_sketch |= (1 << (sketch_len - 1) << i*(sketch_len));
        }
    }
}
// This function use parallel comparision to compare keys in node
int paracomp(struct root *rt,struct FusionTree *p,int data){
	int sketch = sketchap(rt,p,data);
	int sketch_long = sketch*p->mask_q;
	int res = p->node_sketch-sketch_long;
	res = res&p->mask_sketch;
	int i=0;
	while((1<<i)<res){
		i++;
	}
	i++;
	int sketch_len = p->n*p->n*p->n+1;
	return (p->n-(i/sketch_len));
	}
	int main(){
    return 0;
}
