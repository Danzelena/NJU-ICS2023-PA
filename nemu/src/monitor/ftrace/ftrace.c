#include "elfloader.h"
// #include "ftrace.h"

#define STACK_MAX_DEEP 2048

typedef struct Frame
{
    char *name;
    int deep;
    word_t pc;
    // Frame *prevFrame;
} Frame;

typedef struct FuncStack
{
    Frame *func[STACK_MAX_DEEP];
    int top;
} FuncStack;

FuncStack *fstack;

void init()
{
    fstack = (FuncStack *)malloc(sizeof(FuncStack));
    if (fstack == NULL)
    {
        Log("(ftrace)Error: Memory allocation failed!");
        Assert(0, "Memory allocation failed!");
    }

    fstack->top = -1;
}
void push(FuncStack *fs, Frame *f)
{
    if (fs->top == STACK_MAX_DEEP - 1)
    {
        Log("(ftrace)Sorry! FuncStack is filled!");
        Assert(0, "FuncStack is filled!");
    }
    fs->top++;
    f->deep = fs->top;
    fs->func[fs->top] = f;
}

Frame *pop(FuncStack *fs)
{
    if (fs->top == -1)
    {
        Log("(ftrace)Sorry! FuncStack is empty!");
        Assert(0, "FuncStack is empty!");
    }
    return fs->func[fs->top--];
}
void func_print(Frame *func, bool call)
{
    int i;
    printf("%x", func->pc);
    for (i = 0; i < func->deep; i++)
    {
        printf(" ");
    }
    if (call)
    {
        printf("call [%s]\n", func->name);
    }
    else
    {
        printf("ret [%s]\n", func->name);
    }
}
void ftrace_init()
{
    // FuncStack *fstack;

    init();
}

void func_init(Frame *fun, char *name, word_t pc)
{
    fun->name = name;
    fun->deep = 0;
    fun->pc = pc;
}
Frame *function;
void ftrace_call(word_t pc, bool call)
{

    char *func_name = gen_func_name(pc);
    // printf("name:%s\n, pc:%x\n", func_name, pc);

    if (call)
    {
        /* call */
        
        function = (Frame*)malloc(sizeof(Frame));
        // Frame func;
        func_init(function, func_name, pc);
        // function = &func;
        // Frame *function = &func;
        push(fstack, function);
        // func_print(function, true);
        // printf("top = %d\n", fstack->top);

        
    }
    else
    {
        /* ret */
        Frame *pop_func;
        char *name;
        do
        {
            pop_func = pop(fstack);
            name = pop_func->name;
            if(strcmp(name, func_name) != 0){
                free(pop_func);
            }
        } while (strcmp(name, func_name) != 0);
        // func_print(pop_func, false);
        free(pop_func);
    }
    
}
int ftrace_print()
{
    // print(fstack);
    int i;
    for (i = 0; i <= fstack->top; i++)
    {
        // FORWARD:Log in a only file
        Log("(ftrace)%s : deep=%d\n", fstack->func[i]->name, fstack->func[i]->deep);
    }
    return 0;
}
void ftrace_free(){
    // printf("free\n");
    for(int i = 0;i <= fstack->top;i++){
        free((fstack->func)[i]);
    }
    // free(fstack);
    // free(function);
}
// int cnt = 0;
// char *lastfunc;
// tODO:simluate a function call stack to print
// void ftrace_call(word_t pc)
// {
//     // for(int i = 0;i < cnt;i++){
//     //     printf(" ");
//     // }
//     return;
//     char *func = gen_func_name(pc);

//     printf("%s\n", func);
//     // if()
// }