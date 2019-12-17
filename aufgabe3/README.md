find和grep命令的简单实现  
命令格式 crawl path... [-maxdepth=n] [-name=pattern] [-type={d,f}] [-size=[+|-]n] [-line=string]  
示例：  
./crawl . -line=print -name="m*"
./testdir/resources/src/myLength.c:4: printf("I’m a program with a short name\n");
./testdir/resources/src/minecraft.c:4: printf("You should not play right now!\n");  
参数必须以键值对的格式给定。  
参数说明:  
path:要查找的路径，可以为多个。  
-maxdepth:遍历文件树时的最大深度，输入的path作为根节点深度为0。默认为-1，表示遍历整个文件树直到叶子节点。   
-name:要查找的文件名，可以使用正则表达式匹配。  
-type:文件类型。只能为f普通文件或d文件夹文件。  
-size:文件大小。可以有+/-两种模式，+模式表示找到所有大于给定大小的文件，-模式与之相反。  
-line:找到包含特定字符串的文件并输出所在行和行数。使用此参数时将以只读模式遍历所有可读文件并找到给定的字符串。

