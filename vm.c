/*
 * EE 468 Final Exam Problem 
 *
 * This program simulates a virtual memory manager with
 * a TLB with 4 entries.  Total number of memory bytes is 2^16 bytes.
 * Page and frame sizes are 256 bytes.  Number of pages and
 * frames is 256.  It uses the least recently used replacement
 * policy.
 *
 * For the simulation it gets virtual addresses from a file
 * named "address.txt"  
 *
 * However this version doesn't work.  It simulates a
 * memory system with a TLB of size one.  Fix it so
 * that it works according to Final Exam Problem 6.
 */

#include <stdlib.h>
#include <stdio.h>

#define NUMPAGES 256
#define TLBSIZE 4
#define FAIL -1

struct tlb_entry {
   char valid;  /* Valid bit */ 
   int page;    /* Page number */
   int frame;   /* Frame number */
   int age;  // Update
};

struct ptable_entry {
   char valid;  /* Valid bit */ 
   int frame;   /* Frame number */
};

struct tlb {
   struct tlb_entry * entry;
   int size;
};

struct ptable {
   struct ptable_entry * entry;
   int freeframe;  /* Points to location of the next free frame */
   int size;
};

struct ptable * createPTable(int size)
{
struct ptable * p=(struct ptable*) malloc(sizeof(struct ptable));
p->entry = (struct ptable_entry *)malloc(sizeof(struct ptable_entry)*size);
for (int i=0; i<size; i++) {
   p->entry[i].valid=0;
}
p->size=size;
p->freeframe=0;
return p;
}

void destroyPTable(struct ptable *p)
{
free(p->entry);
free(p);
return;
}

int accessPTable(struct ptable *p, int page)
{
if (p->entry[page].valid == 0) { /* No page fault */
    p->entry[page].frame = p->freeframe++;
    p->entry[page].valid = 1;
}
return p->entry[page].frame;
}

struct tlb * createTLB(int size)
{
struct tlb * t=(struct tlb*) malloc(sizeof(struct tlb));
t->entry = (struct tlb_entry *)malloc(sizeof(struct tlb_entry)*size);
for (int i=0; i<size; i++) {
   t->entry[i].valid=0;
}
t->size=size;
return t;
}

void destroyTLB(struct tlb *t)
{
free(t->entry);
free(t);
return;
}

/* 
 * Access the TLB:  returns the frame number given the
 * page number.  Otherwise it returns FAIL
 */
int accessTLB(struct tlb *t, int page, int time)
{
for (int i=0; i<t->size; i++) {
    if (t->entry[i].valid==1 && t->entry[i].page==page) {
	    t->entry[i].age=time;
	    return(t->entry[i].frame);
    }
}
return FAIL;
}	

void replaceTLB(struct tlb *t, int page, int frame, int time)
{
return;
}

/* The next two data structures is the TLB (tlb) and page table (ptable). 
 * TLB is usually an array but in this (bad) implementation there is only
 * one entry. 
 */

void main()
{

struct ptable * p =createPTable(NUMPAGES);
struct tlb * t =createTLB(TLBSIZE);
int tlbHitTotal=0;
int totalReferences=0;
/* Open the file which has virtual addresses */ 
FILE *fp = fopen("address.txt", "r");
if (fp ==NULL) {
   printf("Cannot open file\n");
   exit(1);
}

/* Read virtual addresses */ 
int vaddr;  /* virtual address */  
for (int time=0; ; time++) { /* Read a virtual address */
   if (fscanf(fp, "%d", &vaddr)!= 1) break;

   totalReferences++;
   int page = vaddr >> 8; /* Page # of virtual address */

   /* Access TLB */
   int tlbhit=0;     /* indicates a TLB hit */
   int frame=accessTLB(t,page,time); /* Frame # of page */
   if (frame==FAIL) { 
       frame=accessPTable(p,page);
       replaceTLB(t,page,frame,time);
   }
   else {
       tlbhit=1;
       tlbHitTotal++;
   }

   int paddr = (frame<<8)+ (0xff & vaddr);  /* Physical address */
   printf("VirtAddr=%6d PhysAddr=%6d PageNum=%4d FrameNum=%3d ",vaddr,paddr,page,frame);  
   if (tlbhit == 1) printf("TLB-HIT   ");
   else printf("TLB-MISS  ");

   printf("\n");
}
printf("Total References = %d, Total TLB Hits=%d\n",totalReferences,tlbHitTotal);
fclose(fp);
return;
}


