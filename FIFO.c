#define FIFO_SIZE 8



struct FIFOnode
{
  float val; 
  FIFOnode* next; 
};

struct FIFO 
{
  FIFOnode* front; 
  FIFOnode* back; 
  uint_8 size;
};

char pop_back(FIFO* F)
{
  FIFOnode* dummy; 
  if (F->front == NULL) return 1;
  dummy = F->front; 
  F->front = F->front->next; 
  if (F->front == NULL) F->back = NULL;
  free(dummy); 
  return 0;
}
  
  
void new_FIFOnode(float val, FIFO* F)
{
  struct FIFOnode* n = (struct FIFOnode*)malloc(sizeof(struct FIFOnode));
  n->val = val; 
  n->next = NULL; 
  if (F->front == NULL)
  {
    F->front = n; 
    F->back = n; 
  }
  else 
  {
    F->back->next = n; 
    F->back = n; 
  }
  F->size++; 
  if (F->size>FIFO_SIZE) 
  {
    pop_back(F);
    F->size--; 
  }
}

FIFO* new_FIFO(void)
{
  struct FIFO* F = (struct FIFO*)malloc(sizeof(struct FIFO*)); 
  F->size = 0; 
  F->front = NULL; 
  F->back = NULL; 
  return F; 
}

void delete_FIFO(FIFO* F)
{
  while (!pop_back(F)); 
  free(F); 
}

float calc_average(FIFO* F)
{
    float average = 0;
    FIFOnode* node = F->front;
    while(node != F->back){
        average += node->val;
    }
    return (average/F->size);
}
