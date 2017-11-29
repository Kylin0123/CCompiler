# GNU make手册：http://www.gnu.org/software/make/manual/make.html
# ************ 遇到不明白的地方请google以及阅读手册 *************

# 编译器设定和编译选项
CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99

# 编译目标：src目录下的所有.c文件
CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax $(filter-out $(LFO),$(OBJS))
	$(CC) -o parser $(filter-out $(LFO),$(OBJS)) -lfl -ly

syntax: lexical syntax-c
	$(CC) -c $(YFC) -o $(YFO)

lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v $(YFILE)

-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: clean test
test:
	./parser ../pretest2/1.txt
	@echo 'Error type 1 at Line 4: Undefined variable "j".'
	./parser ../pretest2/2.txt
	@echo 'Error type 2 at Line 4: Undefined function "inc".'
	./parser ../pretest2/3.txt
	@echo 'Error type 3 at Line 4: Redefined variable "i".'
	./parser ../pretest2/4.txt
	@echo 'Error type 4 at Line 6: Redefined function "func".'
	./parser ../pretest2/5.txt
	@echo 'Error type 5 at Line 4: Type mismatched for assignment.'
	./parser ../pretest2/6.txt
	@echo 'Error type 6 at Line 4: The left-hand side of an assignment must be a variable.'
	./parser ../pretest2/7.txt
	@echo 'Error type 7 at Line 4: Type mismatched for operands.'
	./parser ../pretest2/8.txt
	@echo 'Error type 8 at Line 4: Type mismatched for return.'
	./parser ../pretest2/9.txt
	@echo 'Error type 9 at Line 8: Function "func(int)" is not applicable for arguments "(int, int)".'
	./parser ../pretest2/10.txt
	@echo 'Error type 10 at Line 4: "i" is not an array.'
	./parser ../pretest2/11.txt
	@echo 'Error type 11 at Line 4: "i" is not a function.'
	./parser ../pretest2/12.txt
	@echo 'Error type 12 at Line 4: "1.5" is not an integer.'
	./parser ../pretest2/13.txt
	@echo 'Error type 13 at Line 9: Illegal use of ".".'
	./parser ../pretest2/14.txt
	@echo 'Error type 14 at Line 9: Non-existent field "n".'
	./parser ../pretest2/15.txt
	@echo 'Error type 15 at Line 4: Redefined field "x".'
	./parser ../pretest2/16.txt
	@echo 'Error type 16 at Line 6: Duplicated name "Position".'
	./parser ../pretest2/17.txt
	@echo 'Error type 17 at Line 3: Undefined structure "Position".'
	./parser ../pretest2/18.txt
	@echo 'PERFECT!'
	./parser ../pretest2/19.txt
	@echo 'Error type 19 at Line 8: Inconsistent declaration of function "func".'
	@echo 'Error type 18 at Line 6: Undefined function "func" .'
	./parser ../pretest2/20.txt
	@echo 'PERFECT!'
clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~
