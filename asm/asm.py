"""
@Author : ARCTURUS
@Time   : 2022-12-16
"""

import os
import re
from const import (
    MK_template,
    FOUR,
    getLFB,
    getLC,
    getLabel,
    getR,
    popR,
    resetIdx,
    saveR,
)

fileName = "case2"

path = os.path.dirname(__file__)


# 处理函数声明
def handleFunction(func: list[str]):
    vars = {}  # 变量与寄存器映射关系
    label = {}  # 保存所有 label 所在行
    string = {}  # 保存静态字符串
    rspIsChange = False  # 是否修改了栈顶指针
    lc = []
    f = []

    ref_temp = []  # 引用临时变量
    ref = []  # 引用变量
    normal = []  # 普通变量

    params = 0  # 参数个数
    callNumber = 0  # 函数调用次数

    # 先遍历一遍进行统计
    for line in func:
        loc, op, arg1, _, result = re.findall(FOUR, line)[0]

        if (
            op == "GOTO"
            or op == "<"
            or op == ">"
            or op == ">="
            or op == "<="
            or op == "=="
            or op == "!="
        ):
            if label.get(result) is None:
                label[result] = f".L{getLabel()}"

        elif op == "PARAM":
            params += 1
            vars[arg1] = ""  # 占位

        elif op == "&":
            ref_temp.append(result)

        elif op == "=":
            if "var" in result and result not in vars:
                vars[result] = ""  # 占位

                if arg1 in ref_temp:
                    ref.append(result)
                else:
                    normal.append(result)

        elif op == "CALL":
            callNumber += 1

    # 先给指针赋偏移值, 差距要 8 个字节, 否则段错误
    for i, v in enumerate(ref):
        vars[v] = f"-{i * 8 + 4}(%rbp)"

    start = 0 if len(ref) == 0 else 4 + 8 * (len(ref) - 1)

    for i, v in enumerate(normal):
        vars[v] = f"-{start + (i + 1) * 4}(%rbp)"

    min = start + len(normal) * 4 + params * 4  # 最小分配量

    resetIdx()

    c = 0 if min % 16 == 0 else 1
    allocate = (min // 16 + c) * 16  # 16 字节对齐

    localVarNumber = len(ref) + len(normal) + params  # 局部变量数

    paramOffset = 0

    if localVarNumber != params:
        # 参数的偏移量, 有局部变量时参数的偏移量靠近上
        paramOffset = allocate - (params - 1) * 3
    else:
        # 没局部变量时参数从 -4 开始
        paramOffset = 4

    args = []
    curArgs = []
    curArg = None
    reg = "%eax"  # 当前可用寄存器, 这个需要优化...o(TヘTo)

    # 生成汇编代码
    for idx, line in enumerate(func):
        loc, op, arg1, arg2, result = re.findall(FOUR, line)[0]

        if label.get(loc) is not None:
            f.append(f"{label[loc]}:\n")

        match op:
            case "#":
                # 函数调用开始...
                curArg = []

                if len(curArgs) != 0:
                    # 如果存在嵌套, 如 printf("..", fib(1, 2))
                    # 需要把之前的参数入栈, 等待 fib 调用完成后恢复
                    args.append(curArgs)
                    curArgs = []
                    saveR()  # 参数寄存器分配状态也要保存

                resetIdx()  # 参数寄存器分配重置

            case "==":
                a1 = vars[arg1]
                a2 = vars[arg2]

                t = None
                if curArg is not None:
                    # 如果当前正在函数调用
                    t = curArg
                else:
                    # 正常的表达式
                    t = f

                if "$" in a2 and "$" in a1:
                    # a1 和 a2 都是常数(如 $1)
                    if a1 == a2:
                        t.append("\tmovl $1, %eax\n")
                    else:
                        t.append("\tmovl $0, %eax\n")

                    vars[result] = "%eax"
                    continue

                if "$" in a2:
                    t.append(f"\tcmpl {a2}, {a1}\n")
                elif "$" in a1:
                    t.append(f"\tcmpl {a1}, {a2}\n")
                else:
                    t.append(f"\tmovl {a1}, %eax\n")
                    t.append(f"\tcmpl {a2}, %eax\n")

                t.append("\tsete %al\n")
                t.append("\tmovzbl %al, %eax\n")

                if "temp" not in result:
                    t.append(f"\tje {label[result]}\n")

                vars[result] = "%eax"

            case "!=":
                a1 = vars[arg1]
                a2 = vars[arg2]

                t = None
                if curArg is not None:
                    t = curArg
                else:
                    t = f

                # 两个都是常数, 直接比较, 把结果存入 eax
                if "$" in a2 and "$" in a1:
                    if a2 == a1:
                        t.append("\tmovl $0, %eax\n")
                    else:
                        t.append("\tmovl $1, %eax\n")

                    vars[result] = "%eax"
                    continue

                # 其中有一个是常数
                if "$" in a2:
                    t.append(f"\tcmpl {a2}, {a1}\n")
                elif "$" in a1:
                    t.append(f"\tcmpl {a1}, {a2}\n")
                else:
                    # 两个都是变量
                    t.append(f"\tmovl {a1}, %eax\n")
                    t.append(f"\tcmpl {a2}, %eax\n")

                t.append("\tsetne %al\n")
                t.append("\tmovzbl %al, %eax\n")

                if "temp" not in result:
                    t.append(f"\tjne {label[result]}\n")

                vars[result] = "%eax"

            case "!":
                a1 = vars[arg1]

                t = None
                if curArg is not None:
                    t = curArg
                else:
                    t = f

                t.append(f"\tcmpl $0, {a1}\n")
                t.append("\tsete %al\n")
                t.append("\tmovzbl  %al, %eax\n")

                vars[result] = "%eax"

            case "*":
                a1 = vars[arg1]
                a2 = vars[arg2]

                t = None
                if curArg is not None:
                    t = curArg
                else:
                    t = f

                if a1 != "%eax":
                    t.append(f"\tmovl {a1}, %eax\n")

                t.append(f"\timull {a2}, %eax\n")

                vars[result] = "%eax"

                # 主要是防止寄存器被覆盖, 待优化
                if idx + 1 < len(func):
                    if "int" in func[idx + 1]:
                        if result not in func[idx + 2]:
                            # 下下个四元数都没有用到当前计算值
                            # 需要把后面的汇编代码的寄存器改了
                            # 否则当前计算值被覆盖
                            reg = "%ebx"
                    elif result not in func[idx + 1]:
                        reg = "%ebx"

            case "/":
                a1 = vars[arg1]
                a2 = vars[arg2]

                t = None
                if curArg is not None:
                    t = curArg
                else:
                    t = f

                if a1 != "%eax":
                    t.append(f"\tmovl {a1}, %eax\n")

                t.append("\tcltd\n")

                if "$" in a2:
                    t.append(f"\tmovl {a2}, %ecx\n")
                    t.append("\tidivl %ecx\n")
                else:
                    t.append(f"\tidivl {a2}\n")

                vars[result] = "%eax"

            case "%":
                # 和除法类似, 不过除法结果放在 %eax 里, 求余放在 %edx 里
                a1 = vars[arg1]
                a2 = vars[arg2]

                t = None
                if curArg is not None:
                    t = curArg
                else:
                    t = f

                if a1 != "%eax":
                    t.append(f"\tmovl {a1}, %eax\n")  # 被除数

                t.append("\tcltd\n")

                if "$" in a2:
                    t.append(f"\tmovl {a2}, %ecx\n")  # 除数
                    t.append("\tidivl %ecx\n")
                else:
                    t.append(f"\tidivl {a2}\n")

                vars[result] = "%edx"

            case "-":
                a1 = vars[arg1]
                a2 = vars[arg2]

                if a1 != "%eax":
                    if a2 != "%eax":
                        f.append(f"\tmovl {a1}, %eax\n")
                        a1 = "%eax"
                    else:
                        f.append(f"\tmovl {a1}, %edx\n")
                        a1 = "%edx"

                f.append(f"\tsubl {a2}, {a1}\n")

                vars[result] = a1

                reg = "%eax"

            case "+":
                a1 = vars[arg1]
                a2 = vars[arg2]

                f.append(f"\tmovl {a1}, %edx\n")

                if a2 != "%eax":
                    f.append(f"\tmovl {a2}, %eax\n")

                f.append("\taddl %eax, %edx\n")

                if "temp" in result:
                    vars[result] = "%edx"
                elif "var" in result:
                    f.append(f"\tmovl %edx, {vars[result]}\n")

            case "PARAM":
                p = vars[arg1] = f"-{paramOffset}(%rbp)"
                f.append(f"\tmovl {getR()}, {p}\n")
                paramOffset += 4

            case "RETURN":
                a1 = vars[result]
                if "$0" not in f[-1] and a1 != "%eax":
                    f.append(f"\tmovl {a1}, %eax\n")

            case ">":
                a1 = vars[arg1]
                a2 = vars[arg2]

                if "temp" in result:
                    # 表达式需要返回值时
                    t = None
                    if curArg is not None:
                        t = curArg
                    else:
                        t = f

                    if "$" in a1 and "$" in a2:
                        if int(a1.lstrip("$")) > int(a2.lstrip("$")):
                            t.append("\tmovl $1, %eax\n")
                        else:
                            t.append("\tmovl $0, %eax\n")

                        vars[result] = "%eax"
                        continue

                    if "$" in a1:
                        t.append(f"\tcmpl {a1}, {a2}\n")
                        t.append("\tsetl %al\n")
                    elif "$" in a2:
                        t.append(f"\tcmpl {a2}, {a1}\n")
                        t.append("\tsetg %al\n")
                    else:
                        if not a1.startswith("%") and not a2.startswith("%"):
                            # 两个都不是寄存器
                            t.append(f"\tmovl {a1}, %eax\n")
                            t.append(f"\tcmpl {a2}, %eax\n")
                            t.append("\tsetg %al\n")
                        else:
                            # 至少一个是寄存器
                            t.append(f"\tcmpl {a1}, {a2}\n")
                            t.append("\tsetl %al\n")

                    t.append("\tmovzbl %al, %eax\n")

                    vars[result] = "%eax"
                else:
                    if "$" in a1 and "$" in a2:
                        # 两个都是常数, 取出来直接比较, 大了就可以直接跳转
                        if int(a1.lstrip("$")) > int(a2.lstrip("$")):
                            f.append(f"\tjmp {label[result]}\n")
                    elif "$" in a2:
                        # 第二个参数是常数
                        f.append(f"\tcmpl {a2}, {a1}\n")
                        f.append(f"\tjg {label[result]}\n")

                    elif "$" in a1:
                        # 第一个参数是常数
                        f.append(f"\tcmpl {a1}, {a2}\n")
                        f.append(f"\tjl {label[result]}\n")
                    else:
                        # 两个都不是常数
                        if not a1.startswith("%") and not a2.startswith("%"):
                            # 两个都不是寄存器
                            f.append(f"\tmovl {a1}, %eax")
                            f.append(f"\tcmpl %eax, {a2}\n")
                            f.append(f"\tjl {label[result]}\n")
                        else:
                            # 有一个是寄存器
                            f.append(f"\tcmpl {a1}, {a2}\n")
                            f.append(f"\tjl {label[result]}\n")

            case "<":
                a1 = vars[arg1]
                a2 = vars[arg2]

                if "temp" in result:
                    # 表达式需要返回值时
                    t = None
                    if curArg is not None:
                        t = curArg
                    else:
                        t = f

                    if "$" in a1 and "$" in a2:
                        if int(a1.lstrip("$")) < int(a2.lstrip("$")):
                            t.append("\tmovl $1, %eax\n")
                        else:
                            t.append("\tmovl $0, %eax\n")

                        vars[result] = "%eax"
                        continue

                    if "$" in a1:
                        t.append(f"\tcmpl {a1}, {a2}\n")
                        t.append("\tsetg %al\n")
                    elif "$" in a2:
                        t.append(f"\tcmpl {a2}, {a1}\n")
                        t.append("\tsetl %al\n")
                    else:
                        if not a1.startswith("%") and not a2.startswith("%"):
                            # 两个都不是寄存器
                            t.append(f"\tmovl {a1}, %eax")
                            t.append(f"\tcmpl {a2}, %eax\n")
                            t.append("\tsetl %al\n")
                        else:
                            # 有一个是寄存器
                            t.append(f"\tcmpl {a1}, {a2}\n")
                            t.append("\tsetg %al\n")

                    t.append("\tmovzbl %al, %eax\n")

                    vars[result] = "%eax"
                else:
                    if "$" in a1 and "$" in a2:
                        if int(a1.lstrip("$")) < int(a2.lstrip("$")):
                            f.append(f"\tjmp {label[result]}\n")
                    elif "$" in a2:
                        # 第二个参数是常数
                        f.append(f"\tcmpl {a2}, {a1}\n")
                        f.append(f"\tjl {label[result]}\n")

                    elif "$" in a1:
                        # 第一个参数是常数, 或者两个都不是常数
                        f.append(f"\tcmpl {a1}, {a2}\n")
                        f.append(f"\tjg {label[result]}\n")
                    else:
                        if not a1.startswith("%") and not a2.startswith("%"):
                            # 两个都不是寄存器
                            t.append(f"\tmovl {a1}, %eax\n")
                            t.append(f"\tcmpl %eax, {a2}\n")
                            f.append(f"\tjg {label[result]}\n")
                        else:
                            # 有一个是寄存器
                            f.append(f"\tcmpl {a1}, {a2}\n")
                            f.append(f"\tjg {label[result]}\n")

            case "GOTO":
                f.append(f"\tjmp {label[result]}\n")

            case "=":
                if "string" in arg1:
                    content = re.findall(r"string\((.*)\)", arg1)[0]  # 取出字符串

                    varLc = ""
                    if string.get(content) is not None:
                        varLc = string[content]
                    else:
                        varLc = f".LC{getLC()}"
                        string[content] = varLc
                        lc.append(f"{varLc}:\n\t.string {content}\n")

                    t = None
                    if curArg is not None:
                        t = curArg
                    else:
                        t = f

                    t.append(f"\tleaq {varLc}(%rip), %rax\n")

                    vars[result] = "%rax"

                elif "int" in arg1:
                    content = re.findall(r"int\((.*)\)", arg1)[0]  # 取出数字

                    vars[result] = f"${content}"

                elif "var" in result:
                    a1 = vars[arg1]
                    temp = vars[result]

                    t = None
                    if curArg is None:
                        t = f
                    else:
                        t = curArg

                    mov = "movl"

                    if a1.startswith("%r"):
                        mov = "movq"

                    if "var" not in arg1:
                        t.append(f"\t{mov} {a1}, {temp}\n")
                    else:
                        # (=, var0, _, var2) 需要用一个寄存器进行中转
                        t.append(f"\tmovl {a1}, %eax\n")
                        t.append(f"\tmovl %eax, {temp}\n")

            case "LEAVE":
                if rspIsChange:
                    f.append("\tleave\n\tret\n")
                else:
                    f.append("\tpopq %rbp\n\tret\n")

            case "FUNCTION":
                f.extend(
                    [
                        "\t.text\n",
                        f"\t.globl {arg1}\n",
                        f"\t.type {arg1}, @function\n",
                        f"{arg1}:\nLFB{getLFB()}:\n",
                        "\tpushq %rbp\n",
                        "\tmovq %rsp, %rbp\n",
                    ]
                )

                if callNumber != 0 and localVarNumber != 0:
                    f.append(f"\tsubq ${allocate}, %rsp\n")
                    rspIsChange = True  # 此时修改了 rsp

            case "CALL":
                if arg1 == "printf":
                    curArgs.extend(
                        [
                            "\tmovl $0, %eax\n",
                            "\tCALL printf@PLT\n",
                            "\tmovl $0, %eax\n",
                        ]
                    )
                elif result == "_":
                    # 函数没有返回值
                    curArgs.extend(
                        [
                            "\tmovl $0, %eax\n",
                            "\tCALL {arg1}\n",
                            "\tmovl $0, %eax\n",
                        ]
                    )
                else:
                    curArgs.append(f"\tCALL {arg1}\n")

                    vars[result] = "%eax"  # 函数返回值放在 eax 里

                if len(args) == 0:
                    f.append("".join(curArgs))
                    curArgs = []
                    curArg = None
                else:
                    curArg = curArgs
                    curArgs = args.pop()
                    popR()

            case "ARG":
                # 将参数移入寄存器中
                a1 = vars[arg1]

                if a1.startswith("%r"):
                    reg = getR().replace("e", "r")
                    curArg.append(f"\tmovq {a1}, {reg}\n")
                else:
                    curArg.append(f"\tmovl {a1}, {getR()}\n")

                curArgs.insert(0, "".join(curArg))

                curArg = []  # 继续收集其他参数

                reg = "%eax"

            case "&":
                # 取地址, (如 &a)
                a1 = vars[arg1]

                t = None
                if curArg is None:
                    t = f
                else:
                    t = curArg

                t.append(f"\tleaq {a1}, %rax\n")

                vars[result] = "%rax"

            case "*=":
                # 指针(如 *p = ...)
                a1 = vars[arg1]
                temp = vars[result]

                t = None
                if curArg is None:
                    t = f
                else:
                    t = curArg

                if a1 == "%rax":
                    t.append(f"\tmovq {temp}, %rdx\n")
                    t.append(f"\tmovq {a1}, (%rdx)\n")
                elif a1 == "%eax":
                    t.append(f"\tmovq {temp}, %rdx\n")
                    t.append(f"\tmovl {a1}, (%rdx)\n")
                else:
                    t.append(f"\tmovq {temp}, %rax\n")
                    t.append(f"\tmovl {a1}, (%rax)\n")

            case "=*":
                # 取值(如 xx = *p)
                a1 = vars[arg1]

                t = None
                if curArg is None:
                    t = f
                else:
                    t = curArg

                r = reg
                if "e" in r:
                    r = r.replace("e", "r")

                t.append(f"\tmovq {a1}, {r}\n")
                t.append(f"\tmovl ({r}), %ecx\n")

                vars[result] = "%ecx"

    return "".join(lc) + "".join(f)


# 暂不处理全局变量, 反正作业又不要求
def handleVarDecl(decl: list[str]):
    return ""


def readIntermediateCode(src: str) -> str:
    res: list[list[str]] = []

    with open(src, "r", encoding="utf-8") as f:
        while True:
            content = f.readline()

            if content == "":
                break

            if "FUNCTION" in content:
                function: list[str] = [content]

                while True:
                    one = f.readline()

                    function.append(one)

                    if "LEAVE" in one:
                        res.append(handleFunction(function))
                        break
            else:
                decl: list[str] = [content]

                while True:
                    n = f.tell()
                    one = f.readline()

                    if "FUNCTION" in one or one == "":
                        res.append(handleVarDecl(decl))
                        f.seek(n, 0)  # 返回上一行, 避免 FUNCTION 没法读取
                        break

                    decl.append(one)

    return "".join(res)


def writeAsm(fileName: str, res: str):
    src = os.path.join(path, "dist", fileName)  # 生成文件目录

    if not os.path.exists(src):
        os.mkdir(src)

    # Makefile 文件
    mk = os.path.join(src, "Makefile")
    with open(mk, "w", encoding="utf-8") as f:
        f.write(MK_template.replace("$", fileName))

    # 汇编文件
    asm = os.path.join(src, f"{fileName}.s")
    with open(asm, "w", encoding="utf-8") as f:
        f.write(f'\t.file "{fileName}.c"\n')
        f.write(res)


def main():
    interCode = f"../output/{fileName}.txt"  # 中间代码位置

    res = readIntermediateCode(os.path.join(path, interCode))

    writeAsm(fileName, res)


if __name__ == "__main__":
    main()
