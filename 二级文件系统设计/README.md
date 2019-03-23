
 虚拟磁盘外可用命令
 help            功能： 展示你能用的所有命令
                 语法： help  
				 实例： help

 sf              功能： 选择并进入一个虚拟磁盘
                 语法： sf [虚拟磁盘名]  
				 实例： sf disk.img

 sf --mkfs       功能： 创建或初始化一个虚拟磁盘
                 语法： sf --mkfs [虚拟磁盘名]
				 实例： sf --mkfs disk.img

 

 虚拟磁盘内可用命令
 help            功能： 展示你能用的所有命令 
                 语法： help  
				 实例： help

 dumpfs          功能： 展示文件系统格式即超级块内容                 
                 语法： dumpfs  
				 实例： dumpfs 

 ls              功能： 展示该文件系统所有文件 
                 语法： ls  
				 实例： ls

 cat             功能： 展示该文件系统某文件内容 
                 语法： cat [文件名]  
				 实例： cat 1.txt                    (1.txt为虚拟磁盘原有内容)

 cp              功能： 复制虚拟磁盘文件到虚拟磁盘
                 语法： cp [文件名1] [文件名2]
				 实例： cp 1.txt 2.txt               (其中1.txt为虚拟磁盘原有内容，且虚拟磁盘原先中不存在2.txt)

 cpin            功能： 复制宿主机文件到虚拟磁盘 
                 语法： cpin [文件名1] [文件名2]
				 实例： cpin 1.txt 2.txt             (其中1.txt为宿主机原有内容，且虚拟磁盘原先中不存在2.txt)

 cpout           功能： 复制虚拟磁盘文件到宿主机 
                 语法： cpout [文件名1] [文件名2] 
				 实例： cp 1.txt 2.txt               (其中1.txt为虚拟磁盘原有内容，且原先宿主机中不存在2.txt)

 mkdir           功能： 创建新文件夹 
                 语法： mkdir [新文件夹名]
				 实例： mkdir 1

 new             功能： 创建新文件并输入内容，以##结束 
                 语法： new [新文件名]
				 实例： new 1.txt

 rm              功能： 删除该文件系统某文件 
                 语法： rm [文件名] 
				 实例： rm 1.txt 

 rmdir           功能： 删除该文件系统某文件夹 
                 语法： mkdir [文件夹名] 
				 实例： mkdir 1

 exit            功能： 退出当前文件系统 
                 语法： exit   
				 实例： exit  
