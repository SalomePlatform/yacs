import yacsdecorator

@yacsdecorator.leaf
def while_func(context):
    context -= 1
    print("while ", context)
    cond = context > 0
    return cond, context

@yacsdecorator.leaf
def extra_init():
    a = 5
    return a

@yacsdecorator.leaf
def post(x):
    v = x
    return v

@yacsdecorator.leaf
def f1(x):
    y = x + 2
    return y

@yacsdecorator.leaf
def f2(x):
    y = x * 2
    return y

@yacsdecorator.leaf
def g(a, b):
    r = a + b
    print("g(", r, ")")
    cond = r < 100
    return cond, r

@yacsdecorator.bloc
def while_bloc(context):
    a = f1(context)
    b = f2(context)
    cond, r = g(a,b)
    return cond, r

@yacsdecorator.bloc
def main():
    v = extra_init()
    x = yacsdecorator.whileloop(while_func, v)
    post(x)
    yacsdecorator.whileloop(while_func, 3)
    r = yacsdecorator.whileloop(while_bloc, 1)
    print("r final:", r)

if __name__ == "__main__" :
    main()
