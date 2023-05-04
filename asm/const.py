# Makefile
MK_template = "build:\n\tgcc -o $ $.s\n\ntest:\n\t./$\n"

# 四元式解析
FOUR = r"(\d+): \((.+), (.+), (.+), (.+)\)"

LC = -1
LFB = -1  # function start
L = -1  # label


def getLC():
    global LC
    LC = LC + 1
    return LC


def getLFB():
    global LFB
    LFB += 1
    return LFB


def getLabel():
    global L
    L += 1
    return L


# 64 位寄存器
RAX = "%rax"  # 函数返回值, 除法运算中, 存放除数、以及运算结果的商, 乘法运算中，存放被乘数、以及运算结果
EAX = "%eax"
RBP = "%rbp"  # 当前函数调用时栈的基地址
RBX = "%rbx"  # 存放数据段基地址
RCX = "%rcx"  # 函数调用第四个参数, 循环计数
RDX = "%rdx"  # 除法运算中, 存放余数, 乘法运算中, 存放溢出的部分, 函数第三个参数
RSP = "%rsp"  # 栈顶
RDI = "%rdi"  # 函数调用第一个参数
EDI = "%edi"
RSI = "%rsi"  # 函数调用第二个参数
ESI = "%esi"
R8D = "%r8d"  # 函数调用第五个参数
R9D = "%r9d"  # 函数调用第六个参数
RIP = "%rip"  # 程序计数器 PC


# 获取当前可用的寄存器
reg = ["%edi", "%esi", "%edx", "%ecx"]
idx = -1
reg_stack = []


def getR():
    global idx
    idx += 1

    if idx >= len(reg):
        # 寄存器已经不够分配了
        return None

    return reg[idx]


def saveR():
    reg_stack.append(idx)


def popR():
    global idx
    idx = reg_stack.pop()


def resetIdx():
    global idx
    idx = -1


# 转移指令
JMP = "jmp"  # 直接跳转
JE = "je"  # 相等/零
JNE = "jne"  # 不相等
JS = "js"  # 负数
JNS = "jns"  # 非负数
JG = "jg"  # 大于
JGE = "jge"  # 大于等于
JL = "jl"  # 小于
JLE = "jle"  # 小于等于
JA = "ja"  # 大于(无符号)
JAE = "jae"  # 大于等于(无符号)
JB = "jb"  # 小于(无符号)
JBE = "jbe"  # 小于等于(无符号)

# 栈操作
PUSHQ = "pushq"
POPQ = "popq"

# 算术运算
IMULQ = "imulq"  # 有符号乘法
MULQ = "mulq"  # 无符号乘法
CLTO = "clto"  # 转换为八字
IDIVQ = "idivq"  # 有符号除法
DIVQ = "divq"  # 无符号除法
ADDL = "addl"
SUBQ = "subq"

# 传送数据
MOVB = "movb"  # 传送字节
MOVW = "movw"  # 传送字
MOVL = "movl"  # 传送双字
MOVQ = "movq"  # 传送四字
MOVABSQ = "movabsq"  # 传送绝对四字
LEAQ = "leaq"  # 四个字

# 比较
CMPB = "cmpb"  # 比较字节
CMPW = "cmpw"  # 比较字
CMPL = "cmpl"  # 比较双字
CMPQ = "cmpq"  # 比较四字
