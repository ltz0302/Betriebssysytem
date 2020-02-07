使用pipe链接多个程序从而实现|的效果  
命令格式  
concat MAX AWK_VORSCHRIFT GREP_VORSCHRIFT OUTPUT_FILE  
等效于shell指令  
seq 1 MAX | awk AWK_VORSCHRIFT | grep GREP_VORSCHRIFT > OUTPUT_FILE  
可以选择将输出结果保存在一个文档中,若不保存直接输出,则OUTPUT_FILE应输入为-
