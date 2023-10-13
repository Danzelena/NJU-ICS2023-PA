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

#include <isa.h>
#include<memory/paddr.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

#define NUM_LEN 256
enum
{
  TK_NOTYPE = 256,
  TK_EQ,
  DEC,
  HEX,
  REG,
  TK_NEQ,
  AND,
  OR,
  NEG,
  DEREF,
  /* TODO: Add more token types */

};

static struct rule
{
  const char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE}, // spaces

    {"\\+", '+'}, // plus
    {"\\-", '-'}, // sub
    {"\\*", '*'}, // mul
    {"\\/", '/'}, // divide

    {"==", TK_EQ},  // equal
    {"!=", TK_NEQ}, // not equal

    {"0x[0-9A-Fa-f]+", HEX}, // HEX number
    {"[0-9]+", DEC},         // DEC number

    {"\\$[0-9a-z]+", REG}, // register

    {"\\(", '('}, // left (
    {"\\)", ')'}, // right )

    {"&&", AND},    // logical and
    {"\\|\\|", OR}, // logical or

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;
// TODO: determine to change 32
static Token tokens[25565] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        Token tok;
        tok.type = rules[i].token_type;

        switch (rules[i].token_type)
        {
        case DEC:
        {

          memset(tok.str, '\0', sizeof(tok.str));
          strncpy(tok.str, substr_start, substr_len);
          // printf("Debug:tok.str:%s\n",tok.str);
          break;
        }
          // TODO:add a function to tell hex or dec
        case HEX:
        {
          memset(tok.str, '\0', sizeof(tok.str));
          // convert hex to dec
          char hexstr[NUM_LEN];
          memset(hexstr, '\0', sizeof(hexstr));
          strncpy(hexstr, substr_start + 2, substr_len - 2);
          assert(hexstr[substr_len + 1] == '\0');
          int dec = strtol(hexstr, NULL, 16);
          // assert(dec < 10000);
          sprintf(tok.str, "%d", dec);
          printf("Debug:hex=%s,dex=%d\n", hexstr, dec);
          // strncpy(tok.str, substr_start + 2, substr_len - 2);
          break;
        }
          // TODO:add function to handle REG flag
        case REG:
        {
          bool suc = true;
          char name[5];
          // get name of register
          memset(name,'\0',sizeof(name));
          strncpy(name,substr_start + 1,substr_len-1);
          assert(name[substr_len + 1] == '\0');
          //get value of register
          word_t val = isa_reg_str2val(name,&suc);
          assert(suc);
          sprintf(tok.str,"%d",val);
          break;
        }
        // TODO:DEREF case
        case DEREF:
        {
          break;
        }
        case NEG:{
          break;
        }
        default:
        {
          tok.str[0] = '\0';
        }
        }
        if (tok.type != TK_NOTYPE)
        {
          tokens[nr_token] = tok;
          nr_token++;

          break;
        }
      }
    }
    // printf("Debug:nr_token=%d\n",nr_token);
    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_paretheses_matched(Token *tokens, int begin, int end)
{
  int stack[20];
  int top = 0;
  for (int i = begin; i <= end; i++)
  {
    if (tokens[i].type == '(')
    {

      stack[top] = '(';
      top++;
    }
    else if (tokens[i].type == ')' && top > 0 && stack[top - 1] == '(')
    {
      top--;
    }
  }
  if (top != 0)
  {
    return false;
  }
  return true;
}
bool check_paretheses(Token *tokens, int begin, int end)
{
  // if first is not '(' and last is ')',return false
  if (tokens[begin].type != '(' || tokens[end].type != ')')
  {
    return false;
  }
  if (check_paretheses_matched(tokens, begin + 1, end - 1))
  {
    return true;
  }
  return false;
}
bool isCertainType();
static int getMainop(Token *tokens, int begin, int end)
{
  // printf("Debug:begin=%d,end=%d\n",begin,end);
  // get the main operation of the expression
  int isInparetheses = 0;
  // char opset[10] = {'+','-','*','/'};
  struct myset
  {
    int type;
    int pos;
  } okset[40];
  int okindex = 0;
  // linear search
  for (int i = begin; i <= end; i++)
  {
    int type = tokens[i].type;
    if (type == '(')
    {
      isInparetheses++;
    }
    if (type == ')')
    {
      isInparetheses--;
    }
    if (isCertainType(type)||type == DEREF || type == NEG)
    {
      if (isInparetheses == 0)
      {
        struct myset ok;
        ok.type = type;
        ok.pos = i;
        okset[okindex] = ok;
        okindex++;
      }
    }
  }
  // select the best op from okset
  // TODO:modift old rules
  for (int k = okindex - 1; k >= 0; k--)
  {
    if (okset[k].type == AND || okset[k].type == OR)
    {
      return okset[k].pos;
    }
  }
  for (int k = okindex - 1; k >= 0; k--)
  {
    if (okset[k].type == TK_EQ || okset[k].type == TK_NEQ)
    {
      return okset[k].pos;
    }
  }
  for (int k = okindex - 1; k >= 0; k--)
  {
    if (okset[k].type == '+' || okset[k].type == '-')
    {
      return okset[k].pos;
    }
  }
  for (int k = okindex - 1; k >= 0; k--)
  {
    if (okset[k].type == '*' || okset[k].type == '/')
    {
      return okset[k].pos;
    }
  }
    for (int k = okindex - 1; k >= 0; k--)
  {
    if (okset[k].type == NEG)
    {
      return okset[k].pos;
    }
  }
  for (int k = okindex - 1; k >= 0; k--)
  {
    if (okset[k].type == DEREF)
    {
      return okset[k].pos;
    }
  }
  // printf("Debug:okindex=%d\n",okindex);
  assert(0);
}
u_int32_t eval(Token *tokens, int begin, int end)
{
  if (begin > end)
  {
    // Bad expression
    assert(0);
  }
  else if (begin == end)
  {
    // return the value of the DECber
    char *num = tokens[begin].str;
    int number = atoi(num);
    return number;
  }
  else if (check_paretheses(tokens, begin, end) == true)
  {
    return eval(tokens, begin + 1, end - 1);
  }
  else
  {
    // printf("D")
    int op = getMainop(tokens, begin, end);
    int op_type = tokens[op].type;
    u_int32_t val1,val2;
    if (op_type == DEREF||op_type == NEG){
      printf("Debug:op=%d,begin=%d,end=%d\n",op,begin,end);
      val1 = -1;
      val2 = eval(tokens,op+1,end);
    }else{
      val1 = eval(tokens, begin, op - 1);
      val2 = eval(tokens, op + 1, end);
    }
    
    
    int ret;
    switch (op_type)
    {
    case '+':
    {
      ret = val1 + val2;
      break;
    }
    case '-':
    {
      ret = val1 - val2;
      break;
    }
    case '*':
    {
      ret = val1 * val2;
      break;
    }
    case '/':
    {
      ret = val1 / val2;
      break;
    }
      // TODO:add more cases
    case TK_EQ:
      /*if equal returns 1,else return 0*/
      {
        if (val1 == val2)
        {
          ret = 1;
        }
        else
        {
          ret = 0;
        }
        break;
      }
    case TK_NEQ:
    {
      if (val1 == val2)
      {
        ret = 0;
      }
      else
      {
        ret = 1;
      }
      break;
    }
    case AND:
    {
      if (val1 && val2)
      {
        ret = 1;
      }
      else
      {
        ret = 0;
      }
      break;
    }
    case OR:
    {
      if (val1 || val2)
      {
        ret = 1;
      }
      else
      {
        ret = 0;
      }
      break;
    }
    case DEREF:{
      uint8_t *pos = guest_to_host(val2);
      ret = *pos;
      break;
    }
    case NEG:{
      ret = (-1) * val2;
      
      break;
    }
    default:
      assert(0);
    }
    printf("Debug: %d  %d = %d\n",val1,val2,ret);
    return ret;
  }
}
bool isCertainType(int type)
{
  if (type == '+' || type == '-' || type == '*' || type == '/' 
  ||type == '('||type == ')'|| type == NEG||
   type == TK_EQ || type == TK_NEQ || type == AND || type == OR)
  {
    return true;
  }
  return false;
}
word_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  // TODO:handle pointer case
  for (int j = 0; j < nr_token; j++)
  {
    if (tokens[j].type == '*' && (j == 0 || isCertainType(tokens[j - 1].type)))
    {
      tokens[j].type = DEREF;
    }else if(tokens[j].type == '-' && (j==0||isCertainType(tokens[j-1].type))){
      tokens[j].type = NEG;
    }
  }
  int len = nr_token;
  return eval(tokens, 0, len - 1);
  // return 0;
}
