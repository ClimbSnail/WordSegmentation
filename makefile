CC=g++
CODING=UTF-8
# CODING=GB2312

segmentation_app:test_segmentation.o word_segmentation.o
	$(CC) -g -finput-charset=$(CODING) test_segmentation.o word_segmentation.o -o segmentation_app

test_segmentation.o:test_segmentation.cpp word_segmentation.hpp
	$(CC) -g -finput-charset=$(CODING) test_segmentation.cpp  -c

word_segmentation.o:word_segmentation.cpp word_segmentation.hpp
    $(CC) -g -finput-charset=$(CODING) word_segmentation.cpp  -c
# 删除生成文件
cleanall:
	rm -rf word_segmentation.o test_segmentation.o segmentation_app
# 删除过程文件
clean:
	rm -rf word_segmentation.o test_segmentation.o
install:
	# 写install的命令

.PHONY:clean
	

