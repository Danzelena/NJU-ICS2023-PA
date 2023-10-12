/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint
{
  int NO;
  struct watchpoint *next;
  char expression[32];
  int lastval;

  /* TODO: Add more members if necessary */

} WP;

WP *new_wp();
void free_WP();
static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool()
{
  int i;
  for (i = 0; i < NR_WP; i++)
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}
bool check_wp(){
  bool ret = true;
  // if(head == NULL){
  //   return true;
  // }
  for(WP *po = wp_pool;po!=head->next;po = po->next){
    bool su = true;
    int now = expr(po->expression,&su);
    if(now!=po->lastval){
      ret = false;
    }
    po->lastval = now;
  }
  return ret;
}
void print_wp_pool()
{
  printf("range1:\n");
  for (int i = 0; i < NR_WP; i++)
  {
    printf("%d ", wp_pool[i].NO);
  }
  printf("\n");
  WP *po = wp_pool;
  printf("range2:\n");
  while (po != NULL)
  {
    printf("%d ", po->NO);
    po = po->next;
  }
  printf("\n");
  if (head != NULL)
    printf("head=%d\n", head->NO);
  else
    printf("head=NULL\n");
  printf("free_=%d\n", free_->NO);
  printf("=================================\n");
}
/* TODO: Implement the functionality of watchpoint */
int call_new_WP(char *expression,bool*success){
  WP *wp = new_wp(expression,success);
  return wp->NO;
}
// get the wp from free_
WP *new_wp(char *expression,bool*success)
{
  if (free_ == NULL)
  {
    *success = false;
    assert(0);
  }
  WP *ret = free_;
  if (head == NULL)
  {
    head = ret;
  }
  else
  {
    head = head->next;
  }
  free_ = ret->next;
  // check if expression is valid
  expr(expression,success);
  strcpy(ret->expression,expression); 
  // another implement
  // if(head ==  NULL){
  //   head = ret;
  // }else{
  //   ret->next = head->next;
  //   head->next = ret;
  // }
  return ret;
}
void del_WP(int no){
  int cnt = 0;
  for(int i = 0;i <NR_WP;i++){
    if(wp_pool[i].NO == no){
      cnt ++;
      if(cnt == 2){
        Log("Fail:two watchpoiot have same NO!!!\n");
        assert(0);
      }
      free_WP(wp_pool+i);
    }
  }
}
// give back wp to free_
void free_WP(WP *wp)
{
  // get the no. of wq
  // int no = wp->NO;

  // handle the no. case
  // add the free_
  WP *po = wp_pool;
  assert(po != NULL && wp != NULL);
  if (wp == wp_pool)
  {
    free_ = wp;
    head = NULL;
  }
  else
  {
    while (po->next != wp)
    {
      po = po->next;
    }
    po->next = wp->next;
    while (po->next != free_)
    {
      po = po->next;
      po->NO = po->NO - 1;
    }

    head = po;
    po->next = wp;
    wp->NO = free_->NO - 1;
    wp->next = free_;
    free_ = wp;
  }
}