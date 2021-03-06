# 中文句子分词程序
使用C++通过读取词典文件构建孩子兄弟链表的数据结构，进行分词。（目前支持GB2312编码，原理上也可修改成支持utf-8的）

由于是树状的数据结构，故拥有高效的分词速度。程序类贪婪匹配（尽可能匹配多的词）效率为O(n^2),如果是非贪婪匹配效率将会是O(n)。

# 快速开始
运行项目中的"comppile_and_run.sh"，分词的测试句子在"test_data_gb2312"文件中，分词结果输出在"translate_out"文件中。

# 详细设计
1. 使用数组方式（hash）构建一个包含GB2312的6763中文词头，对于分词过程直接使用16位编码就可以定位到词头，效率为o(1)。
2. 随后读取字典文件，假设某次读取到的词为"阿爸",因为这词首部为"阿"所以将"爸"字挂载到词头"阿"的孩子链上，如果挂接的时候"阿"已存在孩子链，就应该把"爸"字挂载成"阿"孩子节点的兄弟节点上（因为他俩的辈分一样）。也就是说，一个父亲可以有多个孩子，但在存储上并不会构建出好几个孩子指针，只能通过孩子拉出兄弟链。
3. 如果是多字的词，同样也是先通过首字的hash找到对应链的入口。最后矮个字依次挂载到整个链表树上。每歌词的最后一个字节点都标记成（IsWord=true），表明从头到该节点已经可以形成一个词。
4. 查找的时候总体类似KMP模式匹配一样，开始遍历句子中的字与链表树中的，依据此前构建链表树时所标记的IsWord来判断是否可以在词节点断词。本代码使用深搜的形式实现最大词匹配，所以效率是O(n^2)。也可以使用最短匹配，因为不需要做回溯，所以效率是O(n)。


# GB2312中文编码参考
<http://tools.jb51.net/table/gb2312>
